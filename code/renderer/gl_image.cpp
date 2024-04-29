/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * Descent 3: Apex
 * Copyright (C) 2024 by Justin Marshall
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

#include "tr_local.h"

extern bool Force_one_texture;

d3Image *OpenGL_bitmap_remap[MAX_BITMAPS * 2];
d3Image *OpenGL_lightmap_remap[MAX_LIGHTMAPS * 2];

ubyte *OpenGL_bitmap_states = NULL;
ubyte *OpenGL_lightmap_states = NULL;

uint *opengl_Upload_data = NULL;

d3Image::d3Image() {
    //deviceHandle = Cur_texture_object_num++;
    glGenTextures(1, &deviceHandle);
    numMultipleSamples = 0;
}

d3Image::d3Image(bool pixelpack, bool linear, bool repeat) { 
  //deviceHandle = Cur_texture_object_num++;
  glGenTextures(1, &deviceHandle);

  format = ImageFormat::RGBA;
  numMultipleSamples = 0;
  uploadType = GL_TEXTURE_2D;

  glBindTexture(GL_TEXTURE_2D, deviceHandle);
  if (pixelpack) {
   // glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
  }  

  if (repeat) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }
  
  if (linear) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }  
}

void d3Image::Init(const void *data, int w, int h, ImageFormat format, int samples, bool useMipmaps) {
  ImageFormatInfo formatInfo = convertFormat(format);
  auto internalFormat = formatInfo.internalFormat;
  auto externalFormat = formatInfo.externalFormat;
  auto dataType = formatInfo.dataType;

  this->format = format;
  numMultipleSamples = samples;
  width = w;
  height = h;

  GL_CheckDriver();
  if (samples > 0) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, deviceHandle);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height,
                            GL_TRUE); // Using 4 samples per pixel.
    GL_CheckDriver();
    uploadType = GL_TEXTURE_2D_MULTISAMPLE;
  } else {
    glBindTexture(GL_TEXTURE_2D, deviceHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, externalFormat, dataType, data);
    GL_CheckDriver();
    uploadType = GL_TEXTURE_2D;
    if (useMipmaps) {
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexParameteri(uploadType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(uploadType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(uploadType, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(uploadType, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  GL_CheckDriver();
  glBindTexture(uploadType, 0); // Unbind the texture.
  GL_CheckDriver();
}

d3Image::~d3Image() { 
    glDeleteTextures(1, &deviceHandle); 
}

int d3Image::GetWidth() const { return width; }
int d3Image::GetHeight() const { return height; }
unsigned int d3Image::GetHandle() const { return deviceHandle; }


ImageFormatInfo d3Image::convertFormat(ImageFormat format) {
  switch (format) {
  case ImageFormat::DXT1:
    return {GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, GL_UNSIGNED_BYTE};
  case ImageFormat::DXT5:
    return {GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, GL_UNSIGNED_BYTE};
  case ImageFormat::RGB:
    return {GL_RGB, GL_RGB, GL_UNSIGNED_BYTE};
  case ImageFormat::RGBA:
    return {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE};
  case ImageFormat::Cubemap:
    return {GL_RGB, GL_RGB, GL_UNSIGNED_BYTE};
  case ImageFormat::Depth:
    return {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT};
  case ImageFormat::Depth24Stencil8:
    return {GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8};
  default:
    // Error handling
    // Returning a default format
    return {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE};
  }
}

void d3Image::InitCubemap(const void* data[6], int size) {
  ImageFormatInfo formatInfo = convertFormat(format);
  auto internalFormat = formatInfo.internalFormat;
  auto externalFormat = formatInfo.externalFormat;
  auto dataType = formatInfo.dataType;

  glBindTexture(GL_TEXTURE_CUBE_MAP, deviceHandle);
  width = size;
  height = size;
  format = ImageFormat::Cubemap;

  for (GLuint i = 0; i < 6; ++i) {
    // Load each face of the cubemap
    if (data != nullptr) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, externalFormat, dataType, data[i]);
    } else {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, externalFormat, dataType, nullptr);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void d3Image::Resize(int width, int height) {
  ImageFormatInfo formatInfo = convertFormat(format);
  auto internalFormat = formatInfo.internalFormat;
  auto externalFormat = formatInfo.externalFormat;
  auto dataType = formatInfo.dataType;

  if (numMultipleSamples > 0) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, deviceHandle);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numMultipleSamples, internalFormat, width, height, GL_TRUE);
  } else {
    glBindTexture(GL_TEXTURE_2D, deviceHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, externalFormat, dataType, NULL);
  }
}

// Converts a 1555 RGB format to RGBA
uint32_t convert555ToRGBA(uint16_t pixel) {
  int r = (pixel >> 10) & 0x1F;
  int g = (pixel >> 5) & 0x1F;
  int b = pixel & 0x1F;

  // Calculate RGB
  uint32_t rgba = ((b * 255 / 31) << 16) | ((g * 255 / 31) << 8) | (r * 255 / 31);

  // Set alpha based on OPAQUE_FLAG
  if (pixel & OPAQUE_FLAG) {
    rgba |= (255 << 24); // Full opacity
  } else {
    rgba = 0; // Fully transparent
  }

  return INTEL_INT(rgba);
}

// Converts a 4444 ARGB format to RGBA
uint32_t convert4444ToRGBA(uint16_t pixel) {
  int a = (pixel >> 12) & 0xF;
  int r = (pixel >> 8) & 0xF;
  int g = (pixel >> 4) & 0xF;
  int b = pixel & 0xF;

  uint32_t rgba = ((a * 255 / 15) << 24) | ((b * 255 / 15) << 16) | ((g * 255 / 15) << 8) | (r * 255 / 15);
  return INTEL_INT(rgba);
}

// Takes our 16bit format and converts it into the memory scheme that OpenGL wants
void d3Image::TranslateBitmapToOpenGL(int bm_handle, int map_type, int replace, int tn) {
  ushort *bm_ptr;
  int texnum = deviceHandle;

  int w, h;
  int size;

  if (UseMultitexture && Last_texel_unit_set != tn) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
    glActiveTextureARB(GL_TEXTURE0_ARB + tn);
    Last_texel_unit_set = tn;
#endif
  }

  if (map_type == MAP_TYPE_LIGHTMAP) {
    if (GameLightmaps[bm_handle].flags & LF_BRAND_NEW)
      replace = 0;

    bm_ptr = lm_data(bm_handle);
    GameLightmaps[bm_handle].flags &= ~(LF_CHANGED | LF_BRAND_NEW);

    w = lm_w(bm_handle);
    h = lm_h(bm_handle);
    size = GameLightmaps[bm_handle].square_res;
  } else {
    if (GameBitmaps[bm_handle].flags & BF_BRAND_NEW)
      replace = 0;

    bm_ptr = bm_data(bm_handle, 0);
    GameBitmaps[bm_handle].flags &= ~(BF_CHANGED | BF_BRAND_NEW);
    w = bm_w(bm_handle, 0);
    h = bm_h(bm_handle, 0);
    size = w;
  }

  if (OpenGL_last_bound[tn] != this) {
    glBindTexture(GL_TEXTURE_2D, texnum);
    OpenGL_sets_this_frame[0]++;
    OpenGL_last_bound[tn] = this;
  }

  int i;

   if (map_type == MAP_TYPE_LIGHTMAP) {
    uint *left_data = (uint *)opengl_Upload_data;
    int bm_left = 0;

    for (int i = 0; i < h; i++, left_data += size, bm_left += w) {
      uint *dest_data = left_data;
      for (int t = 0; t < w; t++) {
        *dest_data++ = convert555ToRGBA(bm_ptr[bm_left + t]);
      }
    }
    if (size > 0) {
      if (replace) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      }
    }
  } else {
    int limit = 0;

    if (bm_mipped(bm_handle)) {
      limit = NUM_MIP_LEVELS + 3; // ryan added +3.
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // Trilinear filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
      limit = 1;
    }

    int m = 0;
    bm_ptr = bm_data(bm_handle, m);
    w = bm_w(bm_handle, m);
    h = bm_h(bm_handle, m);

    if (bm_format(bm_handle) == BITMAP_FORMAT_4444) {
      // Do 4444

      if (bm_mipped(bm_handle)) {
        for (i = 0; i < w * h; i++)
          opengl_Upload_data[i] = INTEL_INT((255 << 24)) | convert4444ToRGBA(bm_ptr[i]);
      } else {
        for (i = 0; i < w * h; i++)
          opengl_Upload_data[i] = convert4444ToRGBA(bm_ptr[i]);
      }
    } else {
      // Do 1555

      for (i = 0; i < w * h; i++)
        opengl_Upload_data[i] = convert555ToRGBA(bm_ptr[i]);
    }

    // rcg06262000 my if wrapper.
    if ((w > 0) && (h > 0)) {
      if (replace) {
        glTexSubImage2D(GL_TEXTURE_2D, m, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      } else {
        glTexImage2D(GL_TEXTURE_2D, m, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_Upload_data);
      }
    }

    // The stored mips dont look that great, just let the driver generate it. 
    if (bm_mipped(bm_handle)) {
      glGenerateMipmap(GL_TEXTURE_2D); // Automatically generate remaining mip levels
    }
  }

  // mprintf ((1,"Doing slow upload to opengl!\n"));

  if (map_type == MAP_TYPE_LIGHTMAP) {
    GameLightmaps[bm_handle].flags &= ~LF_LIMITS;
  }

  CHECK_ERROR(6)
  OpenGL_uploads++;
}

void d3Image::Bind(int tn) {
  if (OpenGL_last_bound[tn] != this) {
    if (UseMultitexture && Last_texel_unit_set != tn) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
      glActiveTextureARB(GL_TEXTURE0_ARB + tn);
      Last_texel_unit_set = tn;
#endif
    }

    glBindTexture(uploadType, deviceHandle);
    OpenGL_last_bound[tn] = this;
    OpenGL_sets_this_frame[0]++;
  }
}

void d3Image::BindNull(void) {
    glBindTexture(uploadType, 0);
}

// Sets up an appropriate wrap type for the current bound texture
void opengl_MakeWrapTypeCurrent(int handle, int map_type, int tn) {
  int uwrap;
  wrap_type dest_wrap;
  
  if (currentBoundShader) {
    char param[512];
    sprintf(param, "texture%d", tn);

    if (map_type == MAP_TYPE_LIGHTMAP) {
      currentBoundShader->bindTexture(param, OpenGL_lightmap_remap[handle]->GetHandle(), tn);
    } else {
      currentBoundShader->bindTexture(param, OpenGL_bitmap_remap[handle]->GetHandle(), tn);
    }
  }

  if (tn == 1)
    dest_wrap = WT_CLAMP;
  else
    dest_wrap = OpenGL_state.cur_wrap_type;

  if (map_type == MAP_TYPE_LIGHTMAP)
    uwrap = GET_WRAP_STATE(OpenGL_lightmap_states[handle]);
  else
    uwrap = GET_WRAP_STATE(OpenGL_bitmap_states[handle]);

  if (uwrap == dest_wrap)
    return;

  if (UseMultitexture && Last_texel_unit_set != tn) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
    glActiveTextureARB(GL_TEXTURE0_ARB + tn);
    Last_texel_unit_set = tn;
#endif
  }

  OpenGL_sets_this_frame[1]++;

  if (OpenGL_state.cur_wrap_type == WT_CLAMP) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  } else if (OpenGL_state.cur_wrap_type == WT_WRAP_V) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if (map_type == MAP_TYPE_LIGHTMAP) {
    SET_WRAP_STATE(OpenGL_lightmap_states[handle], dest_wrap);
  } else {
    SET_WRAP_STATE(OpenGL_bitmap_states[handle], dest_wrap);
  }

  CHECK_ERROR(8)
}

// returns the alpha that we should use
float opengl_GetAlphaMultiplier(void) {
  switch (OpenGL_state.cur_alpha_type) {
  case AT_ALWAYS:
    return 1.0;
  case AT_CONSTANT:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_TEXTURE:
    return 1.0;
  case AT_CONSTANT_TEXTURE:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_VERTEX:
    return 1.0;
  case AT_CONSTANT_TEXTURE_VERTEX:
  case AT_CONSTANT_VERTEX:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_TEXTURE_VERTEX:
    return 1.0;
  case AT_LIGHTMAP_BLEND:
  case AT_LIGHTMAP_BLEND_SATURATE:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_SATURATE_TEXTURE:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_SATURATE_VERTEX:
    return 1.0;
  case AT_SATURATE_CONSTANT_VERTEX:
    return OpenGL_state.cur_alpha / 255.0;
  case AT_SATURATE_TEXTURE_VERTEX:
    return 1.0;
  case AT_SPECULAR:
    return 1.0;
  default:
    // Int3();		// no type defined,get jason
    return 0;
  }
}

int opengl_InitCache(void) {
  OpenGL_bitmap_states = (ubyte *)mem_malloc(MAX_BITMAPS);
  ASSERT(OpenGL_bitmap_states);
  OpenGL_lightmap_states = (ubyte *)mem_malloc(MAX_LIGHTMAPS);
  ASSERT(OpenGL_lightmap_states);

  Cur_texture_object_num = 1;

  // Setup textures and cacheing
  int i;
  for (i = 0; i < MAX_BITMAPS; i++) {
    OpenGL_bitmap_remap[i] = nullptr;
    OpenGL_bitmap_states[i] = 255;
    GameBitmaps[i].flags |= BF_CHANGED | BF_BRAND_NEW;
  }

  for (i = 0; i < MAX_LIGHTMAPS; i++) {
    OpenGL_lightmap_remap[i] = nullptr;
    OpenGL_lightmap_states[i] = 255;
    GameLightmaps[i].flags |= LF_CHANGED | LF_BRAND_NEW;
  }

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  if (UseMultitexture) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
    glActiveTextureARB(GL_TEXTURE0_ARB + 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glActiveTextureARB(GL_TEXTURE0_ARB + 0);
#endif
  }

  CHECK_ERROR(3)

  OpenGL_cache_initted = 1;
  return 1;
}

// draws a scaled 2d bitmap to our buffer
void rend_DrawScaledBitmap(int x1, int y1, int x2, int y2, int bm, float u0, float v0, float u1, float v1, int color, float *alphas) {
  g3Point *ptr_pnts[4];
  g3Point pnts[4];
  float r, g, b;
  if (color != -1) {
    r = GR_COLOR_RED(color) / 255.0;
    g = GR_COLOR_GREEN(color) / 255.0;
    b = GR_COLOR_BLUE(color) / 255.0;
  }
  for (int i = 0; i < 4; i++) {
    if (color == -1)
      pnts[i].p3_l = 1.0;
    else {
      pnts[i].p3_r = r;
      pnts[i].p3_g = g;
      pnts[i].p3_b = b;
    }
    if (alphas) {
      pnts[i].p3_a = alphas[i];
    }

    pnts[i].p3_z = 1.0f;
    pnts[i].p3_flags = PF_PROJECTED;
  }

  pnts[0].p3_sx = x1;
  pnts[0].p3_sy = y1;
  pnts[0].p3_u = u0;
  pnts[0].p3_v = v0;
  pnts[1].p3_sx = x2;
  pnts[1].p3_sy = y1;
  pnts[1].p3_u = u1;
  pnts[1].p3_v = v0;
  pnts[2].p3_sx = x2;
  pnts[2].p3_sy = y2;
  pnts[2].p3_u = u1;
  pnts[2].p3_v = v1;
  pnts[3].p3_sx = x1;
  pnts[3].p3_sy = y2;
  pnts[3].p3_u = u0;
  pnts[3].p3_v = v1;
  ptr_pnts[0] = &pnts[0];
  ptr_pnts[1] = &pnts[1];
  ptr_pnts[2] = &pnts[2];
  ptr_pnts[3] = &pnts[3];
  rend_SetTextureType(TT_LINEAR);
  rend_DrawPolygon2D(bm, ptr_pnts, 4);
}

void rend_Draw2DImage(d3Image *image, int x1, int y1, int x2, int y2, float u0, float v0, float u1, float v1, float r, float g, float b, float a) {
  g3Point *ptr_pnts[4];
  g3Point pnts[4];
  for (int i = 0; i < 4; i++) {
    pnts[i].p3_r = r;
    pnts[i].p3_g = g;
    pnts[i].p3_b = b;
    pnts[i].p3_a = a;
    pnts[i].p3_z = 1.0f;
    pnts[i].p3_flags = PF_PROJECTED;
  }

  pnts[0].p3_sx = x1;
  pnts[0].p3_sy = y1;
  pnts[0].p3_u = u0;
  pnts[0].p3_v = v0;
  pnts[1].p3_sx = x2;
  pnts[1].p3_sy = y1;
  pnts[1].p3_u = u1;
  pnts[1].p3_v = v0;
  pnts[2].p3_sx = x2;
  pnts[2].p3_sy = y2;
  pnts[2].p3_u = u1;
  pnts[2].p3_v = v1;
  pnts[3].p3_sx = x1;
  pnts[3].p3_sy = y2;
  pnts[3].p3_u = u0;
  pnts[3].p3_v = v1;
  ptr_pnts[0] = &pnts[0];
  ptr_pnts[1] = &pnts[1];
  ptr_pnts[2] = &pnts[2];
  ptr_pnts[3] = &pnts[3];
  rend_SetTextureType(TT_LINEAR);
  rend_DrawPolygon2D(image, ptr_pnts, 4);
}

// Draws a simple bitmap at the specified x,y location
void rend_DrawSimpleBitmap(int bm_handle, int x, int y) {
  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(255);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetFiltering(0);
  rend_DrawScaledBitmap(x, y, x + bm_w(bm_handle, 0), y + bm_h(bm_handle, 0), bm_handle, 0, 0, 1, 1);
  rend_SetFiltering(1);
}

// Takes a bitmap and blits it to the screen using linear frame buffer stuff
// X and Y are the destination X,Y
void rend_CopyBitmapToFramebuffer(int bm_handle, int x, int y) {
  if (opengl_Framebuffer_ready == 1) {
    bm_CreateChunkedBitmap(bm_handle, &opengl_Chunked_bitmap);
    opengl_Framebuffer_ready = 2;
  } else {
    opengl_ChangeChunkedBitmap(bm_handle, &opengl_Chunked_bitmap);
  }

  rend_DrawChunkedBitmap(&opengl_Chunked_bitmap, 0, 0, 255);
}


// Preuploads a texture to the video card
void rend_PreUploadTextureToCard(int handle, int map_type) {}

// Frees an uploaded texture from the video card
void rend_FreePreUploadedTexture(int handle, int map_type) {}

// Utilizes a LRU cacheing scheme to select/upload textures the opengl driver
int opengl_MakeBitmapCurrent(int handle, int map_type, int tn) {
  int w, h;
  d3Image *texnum;

  if (map_type == MAP_TYPE_LIGHTMAP) {
    w = GameLightmaps[handle].square_res;
    h = GameLightmaps[handle].square_res;
  } else {
    if (Force_one_texture) {
      handle = 0;
    }

    w = bm_w(handle, 0);
    h = bm_h(handle, 0);
  }

  // See if the bitmaps is already in the cache
  if (map_type == MAP_TYPE_LIGHTMAP) {
    if (OpenGL_lightmap_remap[handle] == nullptr) {
      texnum = new d3Image(true, true, false);
      SET_WRAP_STATE(OpenGL_lightmap_states[handle], 1);
      SET_FILTER_STATE(OpenGL_lightmap_states[handle], 0);
      OpenGL_lightmap_remap[handle] = texnum;
      texnum->TranslateBitmapToOpenGL(handle, map_type, 0, tn);
    } else {
      texnum = OpenGL_lightmap_remap[handle];
      if (GameLightmaps[handle].flags & LF_CHANGED)
        texnum->TranslateBitmapToOpenGL(handle, map_type, 1, tn);
    }
  } else {
    if (OpenGL_bitmap_remap[handle] == nullptr) {
      texnum = new d3Image(true, false, true);
      SET_WRAP_STATE(OpenGL_bitmap_states[handle], 1);
      SET_FILTER_STATE(OpenGL_bitmap_states[handle], 0);
      OpenGL_bitmap_remap[handle] = texnum;
      texnum->TranslateBitmapToOpenGL(handle, map_type, 0, tn);
    } else {
      texnum = OpenGL_bitmap_remap[handle];
      if (GameBitmaps[handle].flags & BF_CHANGED) {
        texnum->TranslateBitmapToOpenGL(handle, map_type, 1, tn);
      }
    }
  }

  texnum->Bind(tn);

  CHECK_ERROR(7)
  return 1;
}



void rend_SetTextureType(texture_type state) {
  if (state == OpenGL_state.cur_texture_type)
    return; // No redundant state setting
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (UseMultitexture && Last_texel_unit_set != 0) {
    glActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
  }
#endif
  OpenGL_sets_this_frame[3]++;

  switch (state) {
  case TT_FLAT:
    glDisable(GL_TEXTURE_2D);
    OpenGL_state.cur_texture_quality = 0;
    break;
  case TT_LINEAR:
  case TT_LINEAR_SPECIAL:
  case TT_PERSPECTIVE:
  case TT_PERSPECTIVE_SPECIAL:
    glEnable(GL_TEXTURE_2D);
    OpenGL_state.cur_texture_quality = 2;
    break;
  default:
    Int3(); // huh? Get Jason
    break;
  }

  CHECK_ERROR(12)
  OpenGL_state.cur_texture_type = state;
}


// Sets the state of bilinear filtering for our textures
void rend_SetFiltering(sbyte state) {
#ifndef RELEASE
  if (Fast_test_render) {
    state = 0;
  }
#endif

  OpenGL_state.cur_bilinear_state = state;
}

// Sets the texture wrapping type
void rend_SetWrapType(wrap_type val) { OpenGL_state.cur_wrap_type = val; }

// Sets a bitmap as a overlay map to rendered on top of the next texture map
// a -1 value indicates no overlay map
void rend_SetOverlayMap(int handle) { Overlay_map = handle; }


// Sets up a font character to draw.  We draw our fonts as pieces of textures
void rend_DrawFontCharacter(int bm_handle, int x1, int y1, int x2, int y2, float u, float v, float w, float h) {
  g3Point *ptr_pnts[4];
  g3Point pnts[4];
  for (int i = 0; i < 4; i++) {
    pnts[i].p3_z = 1; // Make REALLY close!
    pnts[i].p3_flags = PF_PROJECTED;
    ptr_pnts[i] = &pnts[i];
  }
  pnts[0].p3_sx = x1;
  pnts[0].p3_sy = y1;
  pnts[0].p3_u = u;
  pnts[0].p3_v = v;
  pnts[1].p3_sx = x2;
  pnts[1].p3_sy = y1;
  pnts[1].p3_u = u + w;
  pnts[1].p3_v = v;
  pnts[2].p3_sx = x2;
  pnts[2].p3_sy = y2;
  pnts[2].p3_u = u + w;
  pnts[2].p3_v = v + h;
  pnts[3].p3_sx = x1;
  pnts[3].p3_sy = y2;
  pnts[3].p3_u = u;
  pnts[3].p3_v = v + h;
  rend_DrawPolygon2D(bm_handle, ptr_pnts, 4);
}

// Given a source x,y and width,height, draws any sized bitmap into the renderer lfb
void rend_DrawLFBBitmap(int sx, int sy, int w, int h, int dx, int dy, ushort *data, int rowsize) {}


//	given a chunked bitmap, renders it.
void rend_DrawChunkedBitmap(chunked_bitmap *chunk, int x, int y, ubyte alpha) {
  int *bm_array = chunk->bm_array;
  int w = chunk->w;
  int h = chunk->h;
  int piece_w = bm_w(bm_array[0], 0);
  int piece_h = bm_h(bm_array[0], 0);
  int screen_w, screen_h;
  int i, t;
  rend_SetZBufferState(0);
  rend_GetProjectionParameters(&screen_w, &screen_h);
  for (i = 0; i < h; i++) {
    for (t = 0; t < w; t++) {
      int dx = x + (piece_w * t);
      int dy = y + (piece_h * i);
      int dw, dh;
      if ((dx + piece_w) > screen_w)
        dw = piece_w - ((dx + piece_w) - screen_w);
      else
        dw = piece_w;
      if ((dy + piece_h) > screen_h)
        dh = piece_h - ((dy + piece_h) - screen_h);
      else
        dh = piece_h;

      float u2 = (float)dw / (float)piece_w;
      float v2 = (float)dh / (float)piece_h;
      rend_DrawSimpleBitmap(bm_array[i * w + t], dx, dy);
    }
  }
  rend_SetZBufferState(1);
}

//	given a chunked bitmap, renders it.scaled
void rend_DrawScaledChunkedBitmap(chunked_bitmap *chunk, int x, int y, int neww, int newh, ubyte alpha) {
  int *bm_array = chunk->bm_array;
  int w = chunk->w;
  int h = chunk->h;
  int piece_w;
  int piece_h;
  int screen_w, screen_h;
  int i, t;

  float scalew, scaleh;

  scalew = ((float)neww) / ((float)chunk->pw);
  scaleh = ((float)newh) / ((float)chunk->ph);
  piece_w = scalew * ((float)bm_w(bm_array[0], 0));
  piece_h = scaleh * ((float)bm_h(bm_array[0], 0));
  rend_GetProjectionParameters(&screen_w, &screen_h);
  rend_SetOverlayType(OT_NONE);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetZBufferState(0);
  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(alpha);
  rend_SetWrapType(WT_WRAP);
  for (i = 0; i < h; i++) {
    for (t = 0; t < w; t++) {
      int dx = x + (piece_w * t);
      int dy = y + (piece_h * i);
      int dw, dh;
      if ((dx + piece_w) > screen_w)
        dw = piece_w - ((dx + piece_w) - screen_w);
      else
        dw = piece_w;
      if ((dy + piece_h) > screen_h)
        dh = piece_h - ((dy + piece_h) - screen_h);
      else
        dh = piece_h;

      float u2 = (float)dw / (float)piece_w;
      float v2 = (float)dh / (float)piece_h;
      rend_DrawScaledBitmap(dx, dy, dx + dw, dy + dh, bm_array[i * w + t], 0, 0, u2, v2);
    }
  }
  rend_SetZBufferState(1);
}


// Resets the texture cache
void opengl_ResetCache(void) {
  if (OpenGL_cache_initted) {
    mem_free(OpenGL_lightmap_remap);
    mem_free(OpenGL_bitmap_remap);
    mem_free(OpenGL_lightmap_states);
    mem_free(OpenGL_bitmap_states);
    OpenGL_cache_initted = 0;
  }

  opengl_InitCache();
}

ubyte opengl_Framebuffer_ready = 0;
chunked_bitmap opengl_Chunked_bitmap;

void opengl_ChangeChunkedBitmap(int bm_handle, chunked_bitmap *chunk) {
  int bw = bm_w(bm_handle, 0);
  int bh = bm_h(bm_handle, 0);

  // determine optimal size of the square bitmaps
  float fopt = 128.0f;
  int iopt;

  // find the smallest dimension and base off that
  int smallest = min(bw, bh);

  if (smallest <= 32)
    fopt = 32;
  else if (smallest <= 64)
    fopt = 64;
  else
    fopt = 128;

  iopt = (int)fopt;

  // Get how many pieces we need across and down
  float temp = bw / fopt;
  int how_many_across = temp;
  if ((temp - how_many_across) > 0)
    how_many_across++;

  temp = bh / fopt;
  int how_many_down = temp;
  if ((temp - how_many_down) > 0)
    how_many_down++;

  ASSERT(how_many_across > 0);
  ASSERT(how_many_down > 0);

  // Now go through our big bitmap and partition it into pieces
  ushort *src_data = bm_data(bm_handle, 0);
  ushort *sdata;
  ushort *ddata;

  int shift;
  switch (iopt) {
  case 32:
    shift = 5;
    break;
  case 64:
    shift = 6;
    break;
  case 128:
    shift = 7;
    break;
  default:
    Int3(); // Get Jeff
    break;
  }
  int maxx, maxy;
  int windex, hindex;
  int s_y, s_x, d_y, d_x;

  for (hindex = 0; hindex < how_many_down; hindex++) {
    for (windex = 0; windex < how_many_across; windex++) {
      // loop through the chunks
      // find end x and y
      if (windex < how_many_across - 1)
        maxx = iopt;
      else
        maxx = bw - (windex << shift);
      if (hindex < how_many_down - 1)
        maxy = iopt;
      else
        maxy = bh - (hindex << shift);

      // find the starting source x and y
      s_x = (windex << shift);
      s_y = (hindex << shift);

      // get the pointers pointing to the right spot
      ddata = bm_data(chunk->bm_array[hindex * how_many_across + windex], 0);
      GameBitmaps[chunk->bm_array[hindex * how_many_across + windex]].flags |= BF_CHANGED;
      sdata = &src_data[s_y * bw + s_x];

      // copy the data
      for (d_y = 0; d_y < maxy; d_y++) {
        for (d_x = 0; d_x < maxx; d_x++) {
          ddata[d_x] = sdata[d_x];
        } // end for d_x
        sdata += bw;
        ddata += iopt;
      } // end for d_y

    } // end for windex
  }   // end for hindex
}