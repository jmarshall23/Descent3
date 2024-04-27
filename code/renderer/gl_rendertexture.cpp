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

/*
========================
d3RenderTexture::d3RenderTexture
========================
*/
d3RenderTexture::d3RenderTexture(d3Image *colorImage, d3Image *depthImage) {
  deviceHandle = -1;
  if (colorImage != nullptr) {
    AddRenderImage(colorImage);
  }
  this->depthImage = depthImage;
}

/*
========================
d3RenderTexture::~d3RenderTexture
========================
*/
d3RenderTexture::~d3RenderTexture() {
  if (deviceHandle != -1) {
    glDeleteFramebuffers(1, &deviceHandle);
    deviceHandle = -1;
  }
}
/*
================
d3RenderTexture::AddRenderImage
================
*/
void d3RenderTexture::AddRenderImage(d3Image *image) {
  if (deviceHandle != -1) {
     assert(!"d3RenderTexture::AddRenderImage: Can't add render image after FBO has been created!");
     Int3();
  }

  colorImages.push_back(image);
}

/*
================
d3RenderTexture::InitRenderTexture
================
*/
void d3RenderTexture::InitRenderTexture(void) {
  glGenFramebuffers(1, &deviceHandle);
  glBindFramebuffer(GL_FRAMEBUFFER, deviceHandle);

  bool isTexture3D = false;
  if ((colorImages.size() > 0 && colorImages[0]->GetFormat() == ImageFormat::Cubemap) ||
      ((depthImage != nullptr) && depthImage->GetFormat() == ImageFormat::Cubemap)) {
    isTexture3D = true;
  }

  if (!isTexture3D) {
    for (int i = 0; i < colorImages.size(); i++) {
      if (colorImages[i]->GetNumMultipleSamples() == 0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorImages[i]->GetHandle(), 0);
      } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, colorImages[i]->GetHandle(), 0);
      }
    }

    if (depthImage != nullptr) {
      if (depthImage->GetNumMultipleSamples() == 0) {
        if (depthImage->GetFormat() == ImageFormat::Depth) {
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthImage->GetHandle(), 0);
        } else if (depthImage->GetFormat() == ImageFormat::Depth24Stencil8 ) {
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthImage->GetHandle(),0);
        } else {
          assert(!"d3RenderTexture::InitRenderTexture: Unknown depth buffer format!");
        }
      } else {
        if (depthImage->GetFormat() == ImageFormat::Depth) {
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthImage->GetHandle(), 0);
        } else if (depthImage->GetFormat() == ImageFormat::Depth24Stencil8) {
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthImage->GetHandle(), 0);
        } else {
          assert(!"d3RenderTexture::InitRenderTexture: Unknown depth buffer format!");
        }
      }
    }

    if (colorImages.size() > 0) {
      GLenum DrawBuffers[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
                               GL_COLOR_ATTACHMENT4};
      if (colorImages.size() >= 5) {
        assert(!"InitRenderTextures: Too many render targets!");
      }
      glDrawBuffers(colorImages.size(), &DrawBuffers[0]);
    }
  } else {
    if (colorImages.size() > 0) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                             colorImages[0]->GetHandle(), 0);
    }

    if (depthImage != nullptr) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                             depthImage->GetHandle(),
                             0);
    }
  }

  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    char txt[512];
    sprintf(txt, "Failure result: %d\n", result);
    OutputDebugStringA(txt);
    assert(!"d3RenderTexture::InitRenderTexture: Failed to create rendertexture!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
================
d3RenderTexture::MakeCurrent
================
*/
void d3RenderTexture::MakeCurrent(void) { glBindFramebuffer(GL_FRAMEBUFFER, deviceHandle); }

/*
================
d3RenderTexture::BindNull
================
*/
void d3RenderTexture::BindNull(void) { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

/*
================
d3RenderTexture::GetWidth
================
*/
int d3RenderTexture::GetWidth() const {
  if (colorImages.size() > 0) {
    return colorImages[0]->GetWidth();
  }

  return depthImage->GetWidth();
}

/*
================
d3RenderTexture::GetHeight
================
*/
int d3RenderTexture::GetHeight() const {
  if (colorImages.size() > 0) {
    return colorImages[0]->GetHeight();
  }

  return depthImage->GetHeight();
}

/*
================
d3RenderTexture::Resize
================
*/
void d3RenderTexture::Resize(int width, int height) {
  if (GetWidth() == width && GetHeight() == height) {
    return;
  }

  for (int i = 0; i < colorImages.size(); i++) {
    colorImages[i]->Resize(width, height);
  }

  if (depthImage != nullptr) {
    depthImage->Resize(width, height);
  }

  if (deviceHandle != -1) {
    glDeleteFramebuffers(1, &deviceHandle);
    deviceHandle = -1;
  }

  InitRenderTexture();
}