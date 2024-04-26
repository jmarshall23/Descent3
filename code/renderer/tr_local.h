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

#pragma once

#include <vector>
#include "pserror.h"
#include "byteswap.h"
#include "DDAccess.h"
#include "pstypes.h"
#include "pserror.h"
#include "mono.h"
#include "3d.h"
#include "renderer.h"
#include "ddvid.h"
#include "ddio.h"
#include "application.h"
#include "bitmap.h"
#include "lightmap.h"
#include "rend_opengl.h"
#include "grdefs.h"
#include "mem.h"
#include "rtperformance.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gl/glew.h"
#include "../Descent3/args.h"
#include "../libimgui/imgui.h"
#include "../libimgui/imgui_impl_win32.h"
#include "../libimgui/imgui_impl_opengl3.h"
#include "../devcon/DevConsole.h"

class d3image;
#include "gl_rendertexture.h"

#define MAX_POINTS_IN_POLY 100

// These structs are for drawing with OpenGL vertex arrays
// Useful for fast indexing
typedef struct {
  float r, g, b, a;
} color_array;

typedef struct {
  float s, t, r, w;
} tex_array;

void FreeTempPoint(g3Point *p);
void InitFreePoints(void);
void ClipLine(g3Point **p0, g3Point **p1, ubyte codes_or);

// Verify that all the temp points are free, and free them it they are not.
#ifdef _DEBUG
void CheckTempPoints();
#else
#define CheckTempPoints()
#endif

#define _USE_OGL_ACTIVE_TEXTURES

extern int Window_width, Window_height; // the actual integer width & height
extern float Window_w2, Window_h2;      // width,height/2
extern float View_zoom;
extern vector View_position, Matrix_scale;
extern matrix View_matrix, Unscaled_matrix;
extern float Far_clip_z;

// For custom clipping plane
extern ubyte Clip_custom;
extern float Clip_plane_distance;
extern vector Clip_plane;

extern float gTransformViewPort[4][4];
extern float gTransformProjection[4][4];
extern float gTransformModelView[4][4];
extern float gTransformFull[4][4];
void g3_UpdateFullTransform();
void g3_ForceTransformRefresh(void);

void rend_TransformSetToPassthru(void);
void rend_TransformSetViewport(int lx, int ty, int width, int height);
void rend_TransformSetProjection(float trans[4][4]);
void rend_TransformSetModelView(float trans[4][4]);

void opengl_ChangeChunkedBitmap(int bm_handle, chunked_bitmap *chunk);

extern ubyte opengl_Framebuffer_ready;
extern chunked_bitmap opengl_Chunked_bitmap;


extern d3Image *OpenGL_bitmap_remap[MAX_BITMAPS * 2];
extern d3Image *OpenGL_lightmap_remap[MAX_LIGHTMAPS * 2];
extern ubyte *OpenGL_bitmap_states;
extern ubyte *OpenGL_lightmap_states;
extern uint *opengl_Upload_data;
extern rendering_state OpenGL_state;
extern float Alpha_multiplier;
extern PIXELFORMATDESCRIPTOR pfd_copy;
extern renderer_preferred_state OpenGL_preferred_state;
extern vector GL_verts[100];
extern color_array GL_colors[100];
extern tex_array GL_tex_coords[100];
extern tex_array GL_tex_coords2[100];

#define GET_WRAP_STATE(x) (x >> 4)
#define GET_FILTER_STATE(x) (x & 0x0f)

#define SET_WRAP_STATE(x, s)                                                                                           \
  {                                                                                                                    \
    x &= 0x0F;                                                                                                         \
    x |= (s << 4);                                                                                                     \
  }
#define SET_FILTER_STATE(x, s)                                                                                         \
  {                                                                                                                    \
    x &= 0xF0;                                                                                                         \
    x |= (s);                                                                                                          \
  }

extern int OpenGL_window_initted;
extern int OpenGL_polys_drawn;
extern int OpenGL_verts_processed;
extern int OpenGL_uploads;
extern int OpenGL_sets_this_frame[10];
extern int OpenGL_cache_initted;
extern d3Image *OpenGL_last_bound[2];
extern int Last_texel_unit_set;
extern int OpenGL_last_frame_polys_drawn;
extern int OpenGL_last_frame_verts_processed;
extern int OpenGL_last_uploaded;
extern float OpenGL_Alpha_factor;
extern bool OpenGL_Imgui_FirstRender;
extern bool OpenGL_editor_renderSelected;
extern int Cur_texture_object_num;
extern ubyte Fast_test_render;

#define CHECK_ERROR(x)

extern rendering_state OpenGL_state;

extern vector GL_verts[100];
extern color_array GL_colors[100];
extern tex_array GL_tex_coords[100];
extern tex_array GL_tex_coords2[100];

extern float Alpha_multiplier;

extern HWND hOpenGLWnd;
extern HWND hOpenGLWndOLD;
extern HDC hOpenGLDC;
extern HDC hOpenGLDCOLD;
extern HGLRC ResourceContext;

extern int Overlay_map;
extern int Bump_map;
extern int Bumpmap_ready;
extern ubyte Overlay_type;
extern float Z_bias;
extern ubyte Renderer_close_flag;
extern ubyte Renderer_initted;
extern renderer_type Renderer_type;
extern int WindowGL;

extern bool OpenGL_multitexture_state;
extern int Already_loaded;
extern int Imgui_Already_loaded;
extern bool opengl_Blending_on;

extern oeApplication *ParentApplication;

// Function to get the alpha multiplier for transparency effects
float opengl_GetAlphaMultiplier(void);

// Function to make a bitmap current in the OpenGL context
int opengl_MakeBitmapCurrent(int handle, int map_type, int tn);

// Function to set the wrapping type for a texture
void opengl_MakeWrapTypeCurrent(int textureId, int mapType, int textureUnit);

// Function to translate a generic bitmap to an OpenGL-compatible format
void opengl_TranslateBitmapToOpenGL(int texnum, int bm_handle, int map_type, int replace, int tn);
