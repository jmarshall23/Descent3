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

int FindArg(char *);
void rend_SetLightingState(light_state state);
bool IsEditor();

#define CHANGE_RESOLUTION_IN_FULLSCREEN

// The font characteristics
static float rend_FontRed[4], rend_FontBlue[4], rend_FontGreen[4], rend_FontAlpha[4];
char Renderer_error_message[256] = "Generic renderer error";
// The following values are referenced by the game code, but only
// UseMultitexture is actually referenced and used in this code - and
// even that can probably go away and we just assume there is more than
// one texture unit (which, since I believe OpenGL 1.2 is a requirement)
bool UseHardware = true;
bool NoLightmaps = false;
bool StateLimited = false;
bool UseMultitexture = false;
bool UseWBuffer = false;

// General renderer states
int Overlay_map = -1;
int Bump_map = 0;
int Bumpmap_ready = 0;
ubyte Overlay_type = OT_NONE;
float Z_bias = 0.0f;
ubyte Renderer_close_flag = 0;
ubyte Renderer_initted = 0;
renderer_type Renderer_type = RENDERER_OPENGL;
int WindowGL = 0;

extern matrix Unscaled_matrix;
extern vector View_position;

#ifndef GL_UNSIGNED_SHORT_5_5_5_1
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#endif

#ifndef GL_UNSIGNED_SHORT_4_4_4_4
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#endif

//	Moved from DDGR library
HWND hOpenGLWnd = NULL;
HWND hOpenGLWndOLD = NULL;
HDC hOpenGLDC = NULL;
HDC hOpenGLDCOLD = NULL;
HGLRC ResourceContext = NULL;

//	OpenGL Stuff
int OpenGL_window_initted = 0;
int OpenGL_polys_drawn = 0;
int OpenGL_verts_processed = 0;
int OpenGL_uploads = 0;
int OpenGL_sets_this_frame[10];
int OpenGL_packed_pixels = 0;
int OpenGL_cache_initted = 0;
d3Image *OpenGL_last_bound[2];
int Last_texel_unit_set = -1;
int OpenGL_last_frame_polys_drawn = 0;
int OpenGL_last_frame_verts_processed = 0;
int OpenGL_last_uploaded = 0;
float OpenGL_Alpha_factor = 1.0f;
bool OpenGL_Imgui_FirstRender = false;
bool OpenGL_editor_renderSelected = false;
int Cur_texture_object_num = 1;

#ifndef RELEASE
// This is for the Microsoft OpenGL reference driver
// Setting this will turn off bilinear filtering and zbuffer so we can get decent
// framerates to discern driver problems
ubyte Fast_test_render = 0;
#endif

rendering_state OpenGL_state;
float Alpha_multiplier = 1.0f;

PIXELFORMATDESCRIPTOR pfd_copy;
renderer_preferred_state OpenGL_preferred_state = {0, 1, 1.5};

vector GL_verts[100];
color_array GL_colors[100];
tex_array GL_tex_coords[100];
tex_array GL_tex_coords2[100];

bool OpenGL_multitexture_state = false;
int Already_loaded = 0;
int Imgui_Already_loaded = 0;
bool opengl_Blending_on = 0;

oeApplication *ParentApplication = NULL;

// Sets up multi-texturing using ARB extensions
void opengl_GetDLLFunctions(void) {
  UseMultitexture = true;
  return;
}

// Gets some specific information about this particular flavor of opengl
void opengl_GetInformation() {
  mprintf((0, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR)));
  mprintf((0, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER)));
  mprintf((0, "OpenGL Version: %s\n", glGetString(GL_VERSION)));
  mprintf((0, "OpenGL Extensions: %s\n", glGetString(GL_EXTENSIONS)));
}

void rend_EditorFaceSelected(bool IsSelected) { 
    OpenGL_editor_renderSelected = IsSelected; 
}

void rend_GetScreenDimensions(int *width, int *height) {
  *width = OpenGL_state.screen_width;
  *height = OpenGL_state.screen_height;
}

// Sets default states for our renderer
void opengl_SetDefaults() {
  mprintf((0, "Setting states\n"));

  OpenGL_state.cur_color = 0x00FFFFFF;
  OpenGL_state.cur_bilinear_state = -1;
  OpenGL_state.cur_zbuffer_state = -1;
  OpenGL_state.cur_texture_quality = -1;
  OpenGL_state.cur_light_state = LS_GOURAUD;
  OpenGL_state.cur_color_model = CM_MONO;
  OpenGL_state.cur_bilinear_state = -1;
  OpenGL_state.cur_alpha_type = AT_TEXTURE;

  // Enable some states
  glAlphaFunc(GL_GREATER, 0);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_DITHER);
  opengl_Blending_on = true;

#ifndef RELEASE
  if (Fast_test_render) {
    glDisable(GL_DITHER);
  }
#endif

  rend_SetAlphaType(AT_ALWAYS);
  rend_SetAlphaValue(255);
  rend_SetFiltering(1);
  rend_SetLightingState(LS_NONE);
  rend_SetTextureType(TT_FLAT);
  rend_SetColorModel(CM_RGB);
  rend_SetZBufferState(1);
  rend_SetZValues(0, 3000);
  opengl_SetGammaValue(OpenGL_preferred_state.gamma);
  OpenGL_last_bound[0] = nullptr;
  OpenGL_last_bound[1] = nullptr;
  Last_texel_unit_set = -1;
  OpenGL_multitexture_state = false;

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, GL_verts);
  glColorPointer(4, GL_FLOAT, 0, GL_colors);
  glTexCoordPointer(4, GL_FLOAT, 0, GL_tex_coords);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_FOG_HINT, GL_NICEST);
  glEnable(GL_SCISSOR_TEST);
  glScissor(0, 0, OpenGL_state.screen_width, OpenGL_state.screen_height);
  glDisable(GL_SCISSOR_TEST);
  glDepthRange(0.0f, 1.0f);

  if (UseMultitexture) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
    glActiveTextureARB(GL_TEXTURE0_ARB + 1);
    glClientActiveTextureARB(GL_TEXTURE0_ARB + 1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(4, GL_FLOAT, 0, GL_tex_coords2);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_FOG_HINT, GL_NICEST);

    glClientActiveTextureARB(GL_TEXTURE0_ARB + 0);

    glDisable(GL_TEXTURE_2D);
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_DITHER);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glActiveTextureARB(GL_TEXTURE0_ARB + 0);
#endif
  }
}


extern bool Force_one_texture;


// Sets the gamma correction value
void opengl_SetGammaValue(float val) {
 // removed

}

// Tells the software renderer whether or not to use mipping
void rend_SetMipState(sbyte mipstate) {}


#ifdef DEDICATED_ONLY

void rend_DrawPolygon3D(int, g3Point **, int, int) {}
void rend_DrawPolygon2D(int, g3Point **, int, int) {}

#else

static inline float __recip(float x) {
#if MACOSX && __i386__
  __asm__ __volatile__("fres %0, %1  \n\t" : "=f"(x) : "f"(x));
  return (x);
#else
  return (1.0f / x);
#endif
}


#endif // DEDICATED_ONLY

void rend_SetFlatColor(ddgr_color color) { OpenGL_state.cur_color = color; }

// Sets the fog state to TRUE or FALSE
void rend_SetFogState(sbyte state) {
  if (state == OpenGL_state.cur_fog_state)
    return;

  OpenGL_state.cur_fog_state = state;
  if (state == 1) {
    glEnable(GL_FOG);
  } else {
    glDisable(GL_FOG);
  }
}

void rend_MakeCurrent(HWND hwnd, HDC hdc) {
  

   if (hdc == NULL)
   {
       RECT rect;
       POINT topLeft;
       GetClientRect((HWND)hOpenGLWndOLD, &rect);
       
       topLeft.x = rect.left;
       topLeft.y = rect.top;
       ClientToScreen((HWND)hOpenGLWndOLD, &topLeft);
       
       int width = rect.right - rect.left + 1;
       int height = rect.bottom - rect.top + 1;
       int windowX = topLeft.x;
       int windowY = topLeft.y;
       
       OpenGL_state.screen_width = width;
       OpenGL_state.screen_height = height;

      if (!wglMakeCurrent(hOpenGLDCOLD, ResourceContext)) {
         OutputDebugStringA("rend_MakeCurrent reset failed!!\n");
       }

      hOpenGLWnd = hOpenGLWndOLD;
       hOpenGLDC = hOpenGLDCOLD;
       return;
   }

   hOpenGLWndOLD = hOpenGLWnd;
   hOpenGLDCOLD = hOpenGLDC;
   hOpenGLDC = hdc;
   hOpenGLWnd = hwnd;

   RECT rect;
   POINT topLeft;
   GetClientRect((HWND)hOpenGLWnd, &rect);

   topLeft.x = rect.left;
   topLeft.y = rect.top;
   ClientToScreen((HWND)hOpenGLWnd, &topLeft);

   int width = rect.right - rect.left + 1;
   int height = rect.bottom - rect.top + 1;
   int windowX = topLeft.x;
   int windowY = topLeft.y;

   OpenGL_state.screen_width = width;
   OpenGL_state.screen_height = height;

   if (!wglMakeCurrent(hOpenGLDC, ResourceContext)) {
  //   Int3();
     OutputDebugStringA("rend_MakeCurrent failed!!\n");
   }
}

void rend_SetupPixelFormatForTools(HDC hDC) {
  int pixelFormat = ChoosePixelFormat(hDC, &pfd_copy);
  if (pixelFormat > 0) {
    if (SetPixelFormat(hDC, pixelFormat, &pfd_copy) == NULL) {
      Int3();
    }
  } else {
    Int3();
  }
}

// Sets the near and far plane of fog
void rend_SetFogBorders(float nearz, float farz) {
  // Sets the near and far plane of fog
  float fogStart = nearz;
  float fogEnd = farz;

  OpenGL_state.cur_fog_start = fogStart;
  OpenGL_state.cur_fog_end = fogEnd;

  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_START, fogStart);
  glFogf(GL_FOG_END, fogEnd);
}

void rend_SetRendererType(renderer_type state) {
  Renderer_type = state;
  mprintf((0, "RendererType is set to %d.\n", state));
}

void rend_SetLighting(light_state state) {
  if (state == OpenGL_state.cur_light_state)
    return; // No redundant state setting
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (UseMultitexture && Last_texel_unit_set != 0) {
    glActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
  }
#endif

  OpenGL_sets_this_frame[4]++;

  switch (state) {
  case LS_NONE:
    glShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_NONE;
    break;
  case LS_FLAT_GOURAUD:
    glShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_FLAT_GOURAUD;
    break;
  case LS_GOURAUD:
  case LS_PHONG:
    glShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_GOURAUD;
    break;
  default:
    Int3();
    break;
  }

  CHECK_ERROR(13)
}

void rend_SetColorModel(color_model state) {
  switch (state) {
  case CM_MONO:
    OpenGL_state.cur_color_model = CM_MONO;
    break;
  case CM_RGB:
    OpenGL_state.cur_color_model = CM_RGB;
    break;
  default:
    Int3();
    break;
  }
}

void rend_StartFrame(int x1, int y1, int x2, int y2, int clear_flags) {
  if (clear_flags & RF_CLEAR_ZBUFFER) {
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  OpenGL_state.clip_x1 = x1;
  OpenGL_state.clip_y1 = y1;
  OpenGL_state.clip_x2 = x2;
  OpenGL_state.clip_y2 = y2;
}

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
static void slownessAbort(void) {

#ifdef __LINUX__
  SDL_Surface *surface = SDL_GetVideoSurface();
  SDL_SetVideoMode(surface->w, surface->h, surface->format->BitsPerPixel, surface->flags & ~SDL_OPENGL);

  sdl_ShowMessage("Your OpenGL driver is too slow to play this game.\n"
                  "Driver used: [ %s ]\n"
                  "Please change your driver!\n"
                  "Email support@lokigames.com for help,\n"
                  "or call 1-714-508-2140 (9-5 PM US Pacific Time).\n",
                  loadedLibrary);

  SDL_GL_SwapBuffers();
  Sleep(10000);
  SDL_Quit();
  _exit(99);
#else
#error Fill in an aborting notice for your platform.
#endif

} // slownessAbort
#endif

// Flips the screen
void rend_Flip(void) {  
#ifndef RELEASE
  int i;

  RTP_INCRVALUE(texture_uploads, OpenGL_uploads);
  RTP_INCRVALUE(polys_drawn, OpenGL_polys_drawn);

  mprintf_at(
      (1, 1, 0, "Uploads=%d    Polys=%d   Verts=%d   ", OpenGL_uploads, OpenGL_polys_drawn, OpenGL_verts_processed));
  mprintf_at((1, 2, 0, "Sets= 0:%d   1:%d   2:%d   3:%d   ", OpenGL_sets_this_frame[0], OpenGL_sets_this_frame[1],
              OpenGL_sets_this_frame[2], OpenGL_sets_this_frame[3]));
  mprintf_at((1, 3, 0, "Sets= 4:%d   5:%d  ", OpenGL_sets_this_frame[4], OpenGL_sets_this_frame[5]));
  for (i = 0; i < 10; i++) {
    OpenGL_sets_this_frame[i] = 0;
  }
#endif

  OpenGL_last_frame_polys_drawn = OpenGL_polys_drawn;
  OpenGL_last_frame_verts_processed = OpenGL_verts_processed;
  OpenGL_last_uploaded = OpenGL_uploads;

  OpenGL_uploads = 0;
  OpenGL_polys_drawn = 0;
  OpenGL_verts_processed = 0;  
  if (OpenGL_Imgui_FirstRender) {
    console.Draw("Descent 3 Developer Console");

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  SwapBuffers((HDC)hOpenGLDC);

   glClear(GL_COLOR_BUFFER_BIT);
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  OpenGL_Imgui_FirstRender = true;


#ifdef __PERMIT_GL_LOGGING
  if (__glLog == true) {
    DGL_LogNewFrame();
  }
#endif

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
  if (minimumAcceptableRender > 0) {
    Uint32 newticks = SDL_GetTicks();
    if ((newticks - lastSwapTicks) > minimumAcceptableRender) {
      tooSlowCount++;
      if (tooSlowCount >= 3) {
        slownessAbort();
      } // if
    }   // if

    // disable check?
    tooSlowChecksLeft--;
    if (tooSlowChecksLeft <= 0) {
      minimumAcceptableRender = -1;
    }
    lastSwapTicks = newticks;
  } // if
#endif
}

void rend_EndFrame(void) {}


// Sets where the software renderer should write to
void rend_SetSoftwareParameters(float aspect, int width, int height, int pitch, ubyte *framebuffer) {}

// Sets the state of z-buffering to on or off
void rend_SetZBufferState(sbyte state) {
#ifndef RELEASE
  if (Fast_test_render) {
    state = 0;
  }
#endif

  if (state == OpenGL_state.cur_zbuffer_state)
    return; // No redundant state setting

  OpenGL_sets_this_frame[5]++;
  OpenGL_state.cur_zbuffer_state = state;

  //	mprintf ((0,"OPENGL: Setting zbuffer state to %d.\n",state));

  if (state) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  CHECK_ERROR(14)
}

// Sets the near and far planes for z buffer
void rend_SetZValues(float nearz, float farz) {
  OpenGL_state.cur_near_z = nearz;
  OpenGL_state.cur_far_z = farz;
  //	mprintf ((0,"OPENGL:Setting depth range to %f - %f\n",nearz,farz));

  // JEFF: glDepthRange must take parameters [0,1]
  // It is set in init
  //@@glDepthRange (0,farz);
}

void rend_SetOverlayType(ubyte type) { Overlay_type = type; }

// Clears the display to a specified color
void rend_ClearScreen(ddgr_color color) {
  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  glClearColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Clears the zbuffer for the screen
void rend_ClearZBuffer(void) { glClear(GL_DEPTH_BUFFER_BIT); }

// Clears the zbuffer for the screen
void rend_ResetCache(void) {
  mprintf((0, "Resetting texture cache!\n"));
  opengl_ResetCache();
}

// Fills a rectangle on the display
void rend_FillRect(ddgr_color color, int x1, int y1, int x2, int y2) {
  int r = GR_COLOR_RED(color);
  int g = GR_COLOR_GREEN(color);
  int b = GR_COLOR_BLUE(color);

  int width = x2 - x1;
  int height = y2 - y1;

  x1 += OpenGL_state.clip_x1;
  y1 += OpenGL_state.clip_y1;

  glEnable(GL_SCISSOR_TEST);
  glScissor(x1, OpenGL_state.screen_height - (height + y1), width, height);
  glClearColor((float)r / 255.0, (float)g / 255.0, (float)b / 255.0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  width = OpenGL_state.clip_x2 - OpenGL_state.clip_x1;
  height = OpenGL_state.clip_y2 - OpenGL_state.clip_y1;

  glScissor(OpenGL_state.clip_x1, OpenGL_state.screen_height - (OpenGL_state.clip_y1 + height), width, height);
  glDisable(GL_SCISSOR_TEST);
}

// Sets a pixel on the display
void rend_SetPixel(ddgr_color color, int x, int y) {
  int r = (color >> 16 & 0xFF);
  int g = (color >> 8 & 0xFF);
  int b = (color & 0xFF);

  g3_RefreshTransforms(true);

  glColor3ub(r, g, b);

  glBegin(GL_POINTS);
  glVertex2i(x, y);
  glEnd();
}

// Sets a pixel on the display
ddgr_color rend_GetPixel(int x, int y) {
  ddgr_color color[4];
  glReadPixels(x, (OpenGL_state.screen_height - 1) - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)color);
  return color[0];
}

void rend_FillCircle(ddgr_color col, int x, int y, int rad) {}

void rend_DrawCircle(int x, int y, int rad) {}

// Draws a line
void rend_DrawLine(int x1, int y1, int x2, int y2) {
  sbyte atype;
  light_state ltype;
  texture_type ttype;
  int color = OpenGL_state.cur_color;

  g3_RefreshTransforms(true);

  int r = GR_COLOR_RED(color);
  int g = GR_COLOR_GREEN(color);
  int b = GR_COLOR_BLUE(color);

  atype = OpenGL_state.cur_alpha_type;
  ltype = OpenGL_state.cur_light_state;
  ttype = OpenGL_state.cur_texture_type;

  rend_SetAlphaType(AT_ALWAYS);
  rend_SetLighting(LS_NONE);
  rend_SetTextureType(TT_FLAT);

  glBegin(GL_LINES);
  glColor4ub(r, g, b, 255);
  glVertex2i(x1 + OpenGL_state.clip_x1, y1 + OpenGL_state.clip_y1);
  glColor4ub(r, g, b, 255);
  glVertex2i(x2 + OpenGL_state.clip_x1, y2 + OpenGL_state.clip_y1);
  glEnd();

  rend_SetAlphaType(atype);
  rend_SetLighting(ltype);
  rend_SetTextureType(ttype);
}

// Sets the argb characteristics of the font characters.  color1 is the upper left and proceeds clockwise
void rend_SetCharacterParameters(ddgr_color color1, ddgr_color color2, ddgr_color color3, ddgr_color color4) {
  rend_FontRed[0] = (float)(GR_COLOR_RED(color1) / 255.0f);
  rend_FontRed[1] = (float)(GR_COLOR_RED(color2) / 255.0f);
  rend_FontRed[2] = (float)(GR_COLOR_RED(color3) / 255.0f);
  rend_FontRed[3] = (float)(GR_COLOR_RED(color4) / 255.0f);
  rend_FontGreen[0] = (float)(GR_COLOR_GREEN(color1) / 255.0f);
  rend_FontGreen[1] = (float)(GR_COLOR_GREEN(color2) / 255.0f);
  rend_FontGreen[2] = (float)(GR_COLOR_GREEN(color3) / 255.0f);
  rend_FontGreen[3] = (float)(GR_COLOR_GREEN(color4) / 255.0f);
  rend_FontBlue[0] = (float)(GR_COLOR_BLUE(color1) / 255.0f);
  rend_FontBlue[1] = (float)(GR_COLOR_BLUE(color2) / 255.0f);
  rend_FontBlue[2] = (float)(GR_COLOR_BLUE(color3) / 255.0f);
  rend_FontBlue[3] = (float)(GR_COLOR_BLUE(color4) / 255.0f);
  rend_FontAlpha[0] = (color1 >> 24) / 255.0f;
  rend_FontAlpha[1] = (color2 >> 24) / 255.0f;
  rend_FontAlpha[2] = (color3 >> 24) / 255.0f;
  rend_FontAlpha[3] = (color4 >> 24) / 255.0f;
}

// Sets the color of fog
void rend_SetFogColor(ddgr_color color) {
  if (color == OpenGL_state.cur_fog_color)
    return;

  float fc[4];
  fc[0] = GR_COLOR_RED(color);
  fc[1] = GR_COLOR_GREEN(color);
  fc[2] = GR_COLOR_BLUE(color);
  fc[3] = 1;

  fc[0] /= 255.0f;
  fc[1] /= 255.0f;
  fc[2] /= 255.0f;

  glFogfv(GL_FOG_COLOR, fc);
}

// Sets the lighting state of opengl
void rend_SetLightingState(light_state state) {
  if (state == OpenGL_state.cur_light_state)
    return; // No redundant state setting

  if (UseMultitexture && Last_texel_unit_set != 0) {
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
    glActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
#endif
  }

  OpenGL_sets_this_frame[4]++;

  switch (state) {
  case LS_NONE:
    glShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_NONE;
    break;
  case LS_FLAT_GOURAUD:
    glShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_FLAT_GOURAUD;
    break;
  case LS_GOURAUD:
  case LS_PHONG:
    glShadeModel(GL_SMOOTH);
    OpenGL_state.cur_light_state = LS_GOURAUD;
    break;
  default:
    Int3();
    break;
  }

  CHECK_ERROR(13)
}

void rend_SetAlphaType(sbyte atype) {
  if (atype == OpenGL_state.cur_alpha_type)
    return; // don't set it redundantly
#if (defined(_USE_OGL_ACTIVE_TEXTURES))
  if (UseMultitexture && Last_texel_unit_set != 0) {
    glActiveTextureARB(GL_TEXTURE0_ARB + 0);
    Last_texel_unit_set = 0;
  }
#endif
  OpenGL_sets_this_frame[6]++;

  if (atype == AT_ALWAYS) {
    if (opengl_Blending_on) {
      glDisable(GL_BLEND);
      opengl_Blending_on = false;
    }
  } else {
    if (!opengl_Blending_on) {
      glEnable(GL_BLEND);
      opengl_Blending_on = true;
    }
  }

  switch (atype) {
  case AT_ALWAYS:
    rend_SetAlphaValue(255);
    glBlendFunc(GL_ONE, GL_ZERO);
    break;
  case AT_CONSTANT:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_TEXTURE:
    rend_SetAlphaValue(255);
    glBlendFunc(GL_ONE, GL_ZERO);
    break;
  case AT_CONSTANT_TEXTURE:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_VERTEX:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_CONSTANT_TEXTURE_VERTEX:
  case AT_CONSTANT_VERTEX:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_TEXTURE_VERTEX:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case AT_LIGHTMAP_BLEND:
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    break;
  case AT_SATURATE_TEXTURE:
  case AT_LIGHTMAP_BLEND_SATURATE:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SATURATE_VERTEX:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SATURATE_CONSTANT_VERTEX:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SATURATE_TEXTURE_VERTEX:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    break;
  case AT_SPECULAR:
    break;
  default:
    Int3(); // no type defined,get jason
    break;
  }
  OpenGL_state.cur_alpha_type = atype;
  Alpha_multiplier = opengl_GetAlphaMultiplier();
  CHECK_ERROR(15)
}

// Sets the alpha value for constant alpha
void rend_SetAlphaValue(ubyte val) {
  OpenGL_state.cur_alpha = val;
  Alpha_multiplier = opengl_GetAlphaMultiplier();
}

// Sets the overall alpha scale factor (all alpha values are scaled by this value)
// usefull for motion blur effect
void rend_SetAlphaFactor(float val) {
  if (val < 0.0f)
    val = 0.0f;
  if (val > 1.0f)
    val = 1.0f;
  OpenGL_Alpha_factor = val;
}

// Returns the current Alpha factor
float rend_GetAlphaFactor(void) { return OpenGL_Alpha_factor; }

// Draws a line using the states of the renderer
void rend_DrawSpecialLine(g3Point *p0, g3Point *p1) {
  g3_RefreshTransforms(true);

  int x_add = OpenGL_state.clip_x1;
  int y_add = OpenGL_state.clip_y1;
  float fr, fg, fb, alpha;
  int i;

  fr = GR_COLOR_RED(OpenGL_state.cur_color);
  fg = GR_COLOR_GREEN(OpenGL_state.cur_color);
  fb = GR_COLOR_BLUE(OpenGL_state.cur_color);

  fr /= 255.0f;
  fg /= 255.0f;
  fb /= 255.0f;

  alpha = Alpha_multiplier * OpenGL_Alpha_factor;

  // And draw!
  glBegin(GL_LINES);
  for (i = 0; i < 2; i++) {
    g3Point *pnt = p0;

    if (i == 1)
      pnt = p1;

    if (OpenGL_state.cur_alpha_type & ATF_VERTEX)
      alpha = pnt->p3_a * Alpha_multiplier * OpenGL_Alpha_factor;

    // If we have a lighting model, apply the correct lighting!
    if (OpenGL_state.cur_light_state != LS_NONE) {
      if (OpenGL_state.cur_light_state == LS_FLAT_GOURAUD) {
        glColor4f(fr, fg, fb, alpha);
      } else {
        // Do lighting based on intesity (MONO) or colored (RGB)
        if (OpenGL_state.cur_color_model == CM_MONO)
          glColor4f(pnt->p3_l, pnt->p3_l, pnt->p3_l, alpha);
        else {
          glColor4f(pnt->p3_r, pnt->p3_g, pnt->p3_b, alpha);
        }
      }
    } else {
      glColor4f(fr, fg, fb, alpha);
    }

    // Finally, specify a vertex
    float z = max(0, min(1.0, 1.0 - (1.0 / (pnt->p3_z + Z_bias))));
    glVertex3f(pnt->p3_sx + x_add, pnt->p3_sy + y_add, -z);
  }

  glEnd();
}

// Takes a screenshot of the current frame and puts it into the handle passed
void rend_Screenshot(int bm_handle) {
  ushort *dest_data;
  uint *temp_data;
  int i, t;
  int total = OpenGL_state.screen_width * OpenGL_state.screen_height;

  ASSERT((bm_w(bm_handle, 0)) == OpenGL_state.screen_width);
  ASSERT((bm_h(bm_handle, 0)) == OpenGL_state.screen_height);

  int w = bm_w(bm_handle, 0);
  int h = bm_h(bm_handle, 0);

  temp_data = (uint *)mem_malloc(total * 4);
  ASSERT(temp_data); // Ran out of memory?

  dest_data = bm_data(bm_handle, 0);

  glReadPixels(0, 0, OpenGL_state.screen_width, OpenGL_state.screen_height, GL_RGBA, GL_UNSIGNED_BYTE,
                (GLvoid *)temp_data);

  for (i = 0; i < h; i++) {
    for (t = 0; t < w; t++) {
      uint spix = temp_data[i * w + t];

      int r = spix & 0xff;
      int g = (spix >> 8) & 0xff;
      int b = (spix >> 16) & 0xff;

      dest_data[(((h - 1) - i) * w) + t] = GR_RGB16(r, g, b);
    }
  }

  mem_free(temp_data);
}

void rend_SetZBias(float z_bias) {
  if (Z_bias != z_bias) {
    Z_bias = z_bias;

    // Force refresh our transforms to take the Zbias into account
    g3_GetModelViewMatrix(&View_position, &Unscaled_matrix, (float *)gTransformModelView);
    g3_UpdateFullTransform();
    g3_ForceTransformRefresh();
  }
}

// Enables/disables writes the depth buffer
void rend_SetZBufferWriteMask(int state) {
  OpenGL_sets_this_frame[5]++;
  if (state) {
    glDepthMask(GL_TRUE);
  } else {
    glDepthMask(GL_FALSE);
  }
}

// Gets a pointer to a linear frame buffer
void rend_GetLFBLock(renderer_lfb *lfb) {}

// Releases an lfb lock
void rend_ReleaseLFBLock(renderer_lfb *lfb) {}

// Returns the aspect ratio of the physical screen
void rend_GetProjectionParameters(int *width, int *height) {
  *width = OpenGL_state.clip_x2 - OpenGL_state.clip_x1;
  *height = OpenGL_state.clip_y2 - OpenGL_state.clip_y1;
}

void rend_GetProjectionScreenParameters(int &screenLX, int &screenTY, int &screenW, int &screenH) {
  screenLX = OpenGL_state.clip_x1;
  screenTY = OpenGL_state.clip_y1;
  screenW = OpenGL_state.clip_x2 - OpenGL_state.clip_x1 + 1;
  screenH = OpenGL_state.clip_y2 - OpenGL_state.clip_y1 + 1;
}

// Returns the aspect ratio of the physical screen
float rend_GetAspectRatio(void) {
  float aspect_ratio = (float)((3.0f * OpenGL_state.screen_width) / (4.0f * OpenGL_state.screen_height));
  return aspect_ratio;
}

// Sets some global preferences for the renderer
int rend_SetPreferredState(renderer_preferred_state *pref_state) {
  int retval = 1;
  renderer_preferred_state old_state = OpenGL_preferred_state;

  OpenGL_preferred_state = *pref_state;
  if (OpenGL_state.initted) {
    int reinit = 0;
    mprintf((0, "Inside pref state!\n"));

    // Change gamma if needed
    if (pref_state->width != OpenGL_state.screen_width || pref_state->height != OpenGL_state.screen_height ||
        old_state.bit_depth != pref_state->bit_depth) {
      reinit = 1;
    }

    if (reinit) {
      opengl_Close();
      retval = opengl_Init(NULL, &OpenGL_preferred_state);
    } else {
      if (old_state.gamma != pref_state->gamma) {
        opengl_SetGammaValue(pref_state->gamma);
      }
    }
  } else {
    OpenGL_preferred_state = *pref_state;
  }

  return retval;
}

// Sets the gamma for this display
void rend_SetGammaValue(float val) {}



// Fills in the passed in pointer with the current rendering state
void rend_GetRenderState(rendering_state *rstate) { memcpy(rstate, &OpenGL_state, sizeof(rendering_state)); }


// Gets a renderer ready for a framebuffer copy, or stops a framebuffer copy
void rend_SetFrameBufferCopyState(bool state) {
  if (state) {
    ASSERT(opengl_Framebuffer_ready == 0);
    opengl_Framebuffer_ready = 1;
  } else {
    ASSERT(opengl_Framebuffer_ready != 0);
    opengl_Framebuffer_ready = 0;

    if (opengl_Framebuffer_ready == 2) {
      bm_DestroyChunkedBitmap(&opengl_Chunked_bitmap);
      opengl_ResetCache();
    }
  }
}

// Changes the resolution of the renderer
void rend_SetResolution(int width, int height) {}

// Gets OpenGL ready to work in a window
int rend_InitOpenGLWindow(oeApplication *app, renderer_preferred_state *pref_state) {
  WindowGL = 1;
  return opengl_Init(app, pref_state);
}

// Shuts down OpenGL in a window
void rend_CloseOpenGLWindow(void) {
  opengl_Close();
  WindowGL = 0;
  OpenGL_window_initted = 0;
  mprintf((1, "SHUTTING DOWN WINDOWED OPENGL!"));
}

// Sets the hardware bias level for coplanar polygons
// This helps reduce z buffer artifacts
void rend_SetCoplanarPolygonOffset(float factor) {
  if (factor == 0.0f) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  } else {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);
  }
}


// Retrieves an error message
char *rend_GetErrorMessage() { return (char *)Renderer_error_message; }

// Sets an error message
void rend_SetErrorMessage(char *str) {
  ASSERT(strlen(str) < 256);
  strcpy(Renderer_error_message, str);
}

// Returns 1 if there is mid video memory, 2 if there is low vid memory, or 0 if there is large vid memory
int rend_LowVidMem(void) { return 0; }

// Returns 1 if the renderer supports bumpmapping
int rend_SupportsBumpmapping(void) { return 0; }

// Sets a bumpmap to be rendered, or turns off bumpmapping altogether
void rend_SetBumpmapReadyState(int state, int map) {}

// returns the direct draw object
void *rend_RetrieveDirectDrawObj(void **frontsurf, void **backsurf) {
  *frontsurf = NULL;
  *backsurf = NULL;
  return NULL;
}

// returns rendering statistics for the frame
void rend_GetStatistics(tRendererStats *stats) {
  if (Renderer_initted) {
    stats->poly_count = OpenGL_last_frame_polys_drawn;
    stats->vert_count = OpenGL_last_frame_verts_processed;
    stats->texture_uploads = OpenGL_last_uploaded;
  } else {
    memset(stats, 0, sizeof(tRendererStats));
  }
}

void rend_TransformSetToPassthru(void) {
  int width = OpenGL_state.screen_width;
  int height = OpenGL_state.screen_height;

  // Projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho((GLfloat)0.0f, (GLfloat)(width), (GLfloat)(height), (GLfloat)0.0f, 0.0f, 1.0f);

  // Viewport
  glViewport(0, 0, width, height);

  // ModelView
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void rend_TransformSetViewport(int lx, int ty, int width, int height) {
  glViewport(lx, OpenGL_state.screen_height - (ty + height - 1), width, height);
}

void rend_TransformSetProjection(float trans[4][4]) {
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(&trans[0][0]);
}

void rend_TransformSetModelView(float trans[4][4]) {
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(&trans[0][0]);
}

