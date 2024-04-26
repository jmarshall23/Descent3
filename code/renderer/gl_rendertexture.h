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

/*
================================================================================================

        Render Texture

================================================================================================
*/
#if 0
/*
================================================
d3RenderTexture holds both the color and depth images that are made
resident on the video hardware.
================================================
*/
class d3RenderTexture {
public:
  d3RenderTexture(idImage *colorImage, idImage *depthImage);
  ~d3RenderTexture();

  int GetWidth() const;
  int GetHeight() const;

  idImage *GetColorImage(int idx) const { return colorImages[idx]; }
  idImage *GetDepthImage() const { return depthImage; }

  int GetNumColorImages() const { return colorImages.Num(); }

  void Resize(int width, int height);

  void MakeCurrent(void);
  static void BindNull(void);

  GLuint GetDeviceHandle(void) { return deviceHandle; }

  void AddRenderImage(idImage *image);
  void InitRenderTexture(void);

private:
  std::vector<idImage *> colorImages;
  idImage *depthImage;
  GLuint deviceHandle;
};
#endif