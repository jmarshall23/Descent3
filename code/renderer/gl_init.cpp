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

// Check for OpenGL support,
int opengl_Setup(HDC glhdc) {
  // Finds an acceptable pixel format to render to
  PIXELFORMATDESCRIPTOR pfd;
  int pf;

  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
  pfd.iPixelType = PFD_TYPE_RGBA;

  /*if (!WindowGL)
  {
  if (OpenGL_preferred_state.bit_depth==32)
  {
  pfd.cColorBits   = 32;
  pfd.cDepthBits   = 32;
  }
  else
  {
  pfd.cColorBits   = OpenGL_preferred_state.bit_depth;
  pfd.cDepthBits   =OpenGL_preferred_state.bit_depth;
  }

  pfd.cColorBits   = 16;
  pfd.cDepthBits   =16;

  }
  else
  {
  pfd.cColorBits   = 16;
  pfd.cDepthBits   =16;
  }*/

  // Find the user's "best match" PFD
  pf = ChoosePixelFormat(glhdc, &pfd);
  if (pf == 0) {
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  mprintf((0, "Choose pixel format successful!\n"));

  // Try and set the new PFD
  if (SetPixelFormat(glhdc, pf, &pfd) == FALSE) {
    DWORD ret = GetLastError();
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  mprintf((0, "SetPixelFormat successful!\n"));

  // Get a copy of the newly set PFD
  if (DescribePixelFormat(glhdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd_copy) == 0) {
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  // Check the returned PFD to see if it is hardware accelerated
  if ((pfd_copy.dwFlags & PFD_GENERIC_ACCELERATED) == 0 && (pfd_copy.dwFlags & PFD_GENERIC_FORMAT) != 0) {
    Int3();
    // FreeLibrary(opengl_dll_handle);
    return NULL;
  }

  // Create an OpenGL context, and make it the current context
  if (ResourceContext == NULL) {
    ResourceContext = wglCreateContext((HDC)glhdc);
    if (ResourceContext == NULL) {
      DWORD ret = GetLastError();
      // FreeLibrary(opengl_dll_handle);
      Int3();
      return NULL;
    }
  }

  ASSERT(ResourceContext != NULL);
  mprintf((0, "Making context current\n"));
  wglMakeCurrent((HDC)glhdc, ResourceContext);

  if (glewInit() != GLEW_OK) {
    Int3();
    return NULL;
  }

  if (frameRenderTexture == nullptr) {
    frameAlbedoTexture = new d3Image();
    frameDepthTexture = new d3Image();
    frameAlbedoTexture->Init(nullptr, OpenGL_state.screen_width, OpenGL_state.screen_height, ImageFormat::RGBA, 0);
    frameDepthTexture->Init(nullptr, OpenGL_state.screen_width, OpenGL_state.screen_height, ImageFormat::Depth, 0);

    frameRenderTexture = new d3RenderTexture(frameAlbedoTexture, frameDepthTexture);
    frameRenderTexture->InitRenderTexture();

    frameAlbedoTextureMSAA = new d3Image();
    frameDepthTextureMSAA = new d3Image();
    frameAlbedoTextureMSAA->Init(nullptr, OpenGL_state.screen_width, OpenGL_state.screen_height, ImageFormat::RGBA, 4);
    frameDepthTextureMSAA->Init(nullptr, OpenGL_state.screen_width, OpenGL_state.screen_height, ImageFormat::Depth, 4);
    
    frameRenderTextureMSAA = new d3RenderTexture(frameAlbedoTextureMSAA, frameDepthTextureMSAA);
    frameRenderTextureMSAA->InitRenderTexture();
  }
  
  if (!Imgui_Already_loaded) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_InitForOpenGL(hOpenGLWnd);
    ImGui_ImplOpenGL3_Init("#version 130"); // GLSL version, change if necessary
    io.Fonts->AddFontDefault();
    Imgui_Already_loaded = 1;
  }

  Already_loaded = 1;

  return 1;
}



// Sets up our OpenGL rendering context
// Returns 1 if ok, 0 if something bad
int opengl_Init(oeApplication *app, renderer_preferred_state *pref_state) {
  int width, height;
  int retval = 1;
  int i;

  mprintf((0, "Setting up opengl mode!\n"));

  if (pref_state) {
    OpenGL_preferred_state = *pref_state;
  }

  if (app != NULL) {
    ParentApplication = app;
  }

  int windowX = 0, windowY = 0;
#if defined(WIN32)
  /***********************************************************
   *               WINDOWS OPENGL
   ***********************************************************
   */
  static HWND hwnd = NULL;
  if (ParentApplication != NULL) {
    hwnd = static_cast<HWND>(reinterpret_cast<oeWin32Application *>(ParentApplication)->m_hWnd);
  }

  if (!WindowGL) {
    // First set our display mode
    // Create direct draw surface

    DEVMODE devmode;

    devmode.dmSize = sizeof(devmode);
    devmode.dmBitsPerPel = 16;
    // devmode.dmBitsPerPel=OpenGL_preferred_state.bit_depth;
    devmode.dmPelsWidth = OpenGL_preferred_state.width;
    devmode.dmPelsHeight = OpenGL_preferred_state.height;
    devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

#ifdef CHANGE_RESOLUTION_IN_FULLSCREEN
    int retval = ChangeDisplaySettings(&devmode, 0);
#else
    int retval = DISP_CHANGE_SUCCESSFUL;
#endif
    if (retval != DISP_CHANGE_SUCCESSFUL) {
      mprintf((0, "Display mode change failed (err=%d), trying default!\n", retval));
      retval = -1;
      devmode.dmBitsPerPel = 16;
      devmode.dmPelsWidth = 640;
      devmode.dmPelsHeight = 480;
      devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

      retval = ChangeDisplaySettings(&devmode, 0);
      if (retval != DISP_CHANGE_SUCCESSFUL) {
        mprintf((0, "OpenGL_INIT:Change display setting failed failed!\n"));
        rend_SetErrorMessage("OGL: ChangeDisplaySettings failed.  Make sure your desktop is set to 16bit mode!");
        ChangeDisplaySettings(NULL, 0);
        opengl_Close();
        return 0;
      } else {
        OpenGL_preferred_state.bit_depth = 16;
        OpenGL_preferred_state.width = 640;
        OpenGL_preferred_state.height = 480;
      }
    } else {
      mprintf((0, "Setdisplaymode to %d x %d (%d bits) is successful!\n", OpenGL_preferred_state.width,
               OpenGL_preferred_state.height, OpenGL_preferred_state.bit_depth));
    }
  }

  memset(&OpenGL_state, 0, sizeof(rendering_state));

  //	These values are set here - samir
  if (app != NULL) {
    hOpenGLWnd = (HWND)((oeWin32Application *)app)->m_hWnd;
  }

  hOpenGLDC = GetDC(hOpenGLWnd);

  if (WindowGL) {
    RECT rect;
    POINT topLeft;
    GetClientRect((HWND)hOpenGLWnd, &rect);

    topLeft.x = rect.left;
    topLeft.y = rect.top;
    ClientToScreen((HWND)hOpenGLWnd, &topLeft);

    width = rect.right - rect.left + 1;
    height = rect.bottom - rect.top + 1;
    windowX = topLeft.x;
    windowY = topLeft.y;
  } else {
    SetWindowPos(hOpenGLWnd, HWND_TOPMOST, 0, 0, OpenGL_preferred_state.width, OpenGL_preferred_state.height,
                 SWP_FRAMECHANGED);
    width = OpenGL_preferred_state.width;
    height = OpenGL_preferred_state.height;
    RECT rect;
    GetWindowRect((HWND)hOpenGLWnd, &rect);
    mprintf((0, "rect=%d %d %d %d\n", rect.top, rect.right, rect.bottom, rect.left));
  }

  OpenGL_state.screen_width = width;
  OpenGL_state.screen_height = height;

  if (!opengl_Setup(hOpenGLDC)) {
    opengl_Close();
    return 0;
  }

#elif defined(__LINUX__)
  /***********************************************************
   *               LINUX OPENGL
   ***********************************************************
   */
  // Setup OpenGL_state.screen_width & OpenGL_state.screen_height & width & height
  width = OpenGL_preferred_state.width;
  height = OpenGL_preferred_state.height;

  if (!opengl_Setup(app, &width, &height)) {
    opengl_Close();
    return 0;
  }

  memset(&OpenGL_state, 0, sizeof(rendering_state));
  OpenGL_state.screen_width = width;
  OpenGL_state.screen_height = height;
#else
  // Setup OpenGL_state.screen_width & OpenGL_state.screen_height & width & height

#endif
  // Get some info
  opengl_GetInformation();

  mprintf((0, "Setting up multitexture...\n"));

  // Determine if Multitexture is supported
  bool supportsMultiTexture = true;

  if (FindArg("-NoMultitexture")) {
    supportsMultiTexture = false;
  }

  if (supportsMultiTexture) {
    // attempt to grab Multitexture functions
    opengl_GetDLLFunctions();
  } else {
    // No multitexture at all
    UseMultitexture = false;
  }

  opengl_InitCache();

  if (UseMultitexture) {
    mprintf((0, "Using multitexture."));
  } else {
    mprintf((0, "Not using multitexture."));
  }

  opengl_Upload_data = (uint *)mem_malloc(2048 * 2048 * 4);
  ASSERT(opengl_Upload_data);

  opengl_SetDefaults();

  g3_ForceTransformRefresh();

  CHECK_ERROR(4)

  OpenGL_state.initted = 1;

  mprintf((0, "OpenGL initialization at %d x %d was successful.\n", width, height));

  return retval;
}

// Releases the rendering context
void opengl_Close() {
  CHECK_ERROR(5)

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
  OpenGL_Imgui_FirstRender = false;
  Imgui_Already_loaded = 0;

  if (frameRenderTexture != nullptr) {
    delete frameAlbedoTextureMSAA;
    frameAlbedoTextureMSAA = nullptr;

    delete frameDepthTextureMSAA;
    frameDepthTextureMSAA = nullptr;

    delete frameAlbedoTexture;
    frameAlbedoTexture = nullptr;

    delete frameDepthTexture;
    frameDepthTexture = nullptr;

    delete frameRenderTextureMSAA;
    frameRenderTextureMSAA = nullptr;

    delete frameRenderTexture;
    frameRenderTexture = nullptr;
  }

#if defined(WIN32)
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(ResourceContext);
  ResourceContext = nullptr;

  // Change our display back
  if (!WindowGL) {
#ifdef CHANGE_RESOLUTION_IN_FULLSCREEN
    ChangeDisplaySettings(NULL, 0);
#endif
  }
#elif defined(__LINUX__)
  // SDL_Quit() handles this for us.
#else

#endif

  if (OpenGL_cache_initted) {
    for (int i = 0; i < MAX_LIGHTMAPS * 2; i++) {
      if (OpenGL_lightmap_remap[i] != nullptr) {
        delete OpenGL_lightmap_remap[i];
        OpenGL_lightmap_remap[i] = nullptr;
      }
    }

    for (int i = 0; i < MAX_BITMAPS * 2; i++) {
      if (OpenGL_bitmap_remap[i] != nullptr) {
        delete OpenGL_bitmap_remap[i];
        OpenGL_bitmap_remap[i] = nullptr;
      }
    }

    mem_free(OpenGL_lightmap_states);
    mem_free(OpenGL_bitmap_states);
    OpenGL_cache_initted = 0;
  }

#if defined(WIN32)
  //	I'm freeing the DC here - samir
  ReleaseDC(hOpenGLWnd, hOpenGLDC);
#elif defined(__LINUX__)

#else

#endif
  // mod_FreeModule (OpenGLDLLHandle);
  OpenGL_state.initted = 0;
}


// Init our renderer
int rend_Init(renderer_type state, oeApplication *app, renderer_preferred_state *pref_state) {
#ifndef DEDICATED_ONLY
  int retval = 0;
  rend_SetRendererType(state);
  if (!Renderer_initted) {
    if (!Renderer_close_flag) {
      atexit(rend_Close);
      Renderer_close_flag = 1;
    }

    Renderer_initted = 1;
  }

  if (OpenGL_window_initted) {
    rend_CloseOpenGLWindow();
    OpenGL_window_initted = 0;
  }

  mprintf((0, "Renderer init is set to %d\n", Renderer_initted));

#ifndef OEM_V3
  int flags = app->flags();
  if (flags & OEAPP_WINDOWED) {
    // initialize for windowed
    retval = rend_InitOpenGLWindow(app, pref_state);
  } else {
    // initialize for full screen
    retval = opengl_Init(app, pref_state);
  }
#endif

  return retval;
#else
  return 0;
#endif // #ifdef DEDICATED_ONLY
}

void rend_Close(void) {
  mprintf((0, "CLOSE:Renderer init is set to %d\n", Renderer_initted));
  if (!Renderer_initted)
    return;

  if (OpenGL_window_initted) {
    if (Renderer_type == RENDERER_OPENGL) {
      rend_CloseOpenGLWindow();
    }
    OpenGL_window_initted = 0;
  }

  opengl_Close();

  Renderer_initted = 0;
}