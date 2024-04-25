/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Adecode.h"

#include "libacm.h"

using namespace AudioDecoder;

namespace {

class InternalAudioDecoder : public IAudioDecoder {
public:
  InternalAudioDecoder(ReadDataFunction readerFunction, void *pReaderData);
  ~InternalAudioDecoder();

  // Initialize the decoder
  bool Initialize();

  // Read data from the audio decoder.
  //   pBuffer: The buffer to receive the data into
  //    amount: How much data to read
  // Returns the number of bytes read - zero when we're at the end of the file
  uint32 Read(void *pBuffer, uint32 amount);

  ACMStream *m_acm = nullptr;
  ReadDataFunction m_readerFunction;
  void *m_pReaderData;
};

/**************************************************************/
/*                         Construction                       */
/**************************************************************/

int AcmReadFunc(void *ptr, int size, int n, void *datasrc) {
  InternalAudioDecoder *iad = reinterpret_cast<InternalAudioDecoder *>(datasrc);
  int ret = iad->m_readerFunction(iad->m_pReaderData, ptr, (unsigned int)size * n);
  // ret < 0: error, ret == 0: EOF, ret > 0: read ret bytes of data
  // apparently acm_io_callbacks::read() expects pretty much the same behavior,
  // except that for > 0 it's not number of bytes but number of items (like in
  // fread())
  if (ret > 0) {
    ret /= size;
  }
  return ret;
}

InternalAudioDecoder::InternalAudioDecoder(ReadDataFunction readerFunction, void *pReaderData)
    : m_readerFunction(readerFunction), m_pReaderData(pReaderData) {}

// Initialize the decoder
bool InternalAudioDecoder::Initialize() {

  acm_io_callbacks io = {AcmReadFunc}; // set the read function, the others are optional

  int force_channels = 0; // 0 = let libacm figure out how many channels the file has
  // TODO: the old libacm.cpp was more optimistic about the numbers of channel
  // from the file header
  //       than libacm's decode.c, which assumes that channels are always >= 2
  //       (unless it's WAVC instead of "plain ACM"), i.e. that a file header
  //       specifying 1 is wrong. If it turns out that we really have ACM files
  //       with just one channel (not unusual for ingame sounds?), we might have
  //       to either patch acm_open_decoder() or somehow detect the number of
  //       channels here and set force_channels accordingly
  int ret = acm_open_decoder(&m_acm, this, io, force_channels);
  return ret == ACM_OK;
}

/**************************************************************/
/*                         Destruction                        */
/**************************************************************/

InternalAudioDecoder::~InternalAudioDecoder() {
  if (m_acm != nullptr)
    acm_close(m_acm);
}

/**************************************************************/
/*                           Reading                          */
/**************************************************************/

// Read data from the audio decoder.
//   pBuffer: The buffer to receive the data into
//    amount: How much data to read
// Returns the number of bytes read - zero when we're at the end of the file
uint32 InternalAudioDecoder::Read(void *pBuffer, uint32 amount) {
  const int bigendianp = 0; // we want little endian samples - TODO: or only on little endian platforms?
  const int wordlen = 2;    // the only supported value
  const int sgned = 1;      // we want signed samples
  uint32 totalBytesRead = 0;
  uint8 *pBuf = reinterpret_cast<uint8 *>(pBuffer);

  while (totalBytesRead < amount) {
    int numRead = acm_read(m_acm, pBuf, amount - totalBytesRead, bigendianp, wordlen, sgned);
    // numRead < 0: error, numRead == 0: EOF, numRead > 0: amount of bytes read
    if (numRead <= 0)
      break;
    totalBytesRead += numRead;
    pBuf += numRead;
  }

  return totalBytesRead;
}

} // namespace

/**************************************************************/
/*                   Interface Functions                      */
/**************************************************************/

// Create an audio decoder
// You supply a function for reading bytes from the compressed data via a
// void* pData handle, and the handle itself (typically a FILE *).
// Create_AudioDecoder returns a new AudioDecoder which can be used to
// read uncompressed decoded data from the compressed stream,
// and also returns the number of channels (1 or 2), the sample rate
// (e.g. 22050), and the number of samples contained in the compressed file
// (in case you want to pre-allocate a buffer to load them all into memory).
IAudioDecoder *AudioDecoder::CreateDecoder(ReadDataFunction readerFunction, void *pReaderData, uint32 &numChannels,
                                           uint32 &sampleRate, uint32 &sampleCount) {
  // allocate our decoder
  InternalAudioDecoder *pDecoder = new InternalAudioDecoder(readerFunction, pReaderData);
  if (pDecoder == nullptr)
    return nullptr;

  // initialize
  if (!pDecoder->Initialize()) {
    // Failed
    delete pDecoder;
    return nullptr;
  }

  // extract the header information for the caller
  numChannels = pDecoder->m_acm->info.channels;
  sampleRate = pDecoder->m_acm->info.rate;
  sampleCount = pDecoder->m_acm->total_values;

  // return the decoder back to the user
  return pDecoder;
}