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
 * $Logfile: /DescentIII/Main/renderer/renderer.cpp $
 * $Revision: 125 $
 * $Date: 4/19/00 5:24p $
 * $Author: Matt $
 *
 * Renderer abstraction layer
 *
 * $Log: /DescentIII/Main/renderer/renderer.cpp $
 *
 * 125   4/19/00 5:24p Matt
 * From Duane for 1.4
 * Removed ATI flag
 *
 * 124   3/20/00 12:31p Matt
 * Merge of Duane's post-1.3 changes.
 * Removed defines for Mac.
 *
 * 123   10/21/99 3:18p Kevin
 * Mac Merge
 *
 * 122   9/18/99 9:26p Jeff
 * added alpha factor (for overriding all alpha on polygons drawn)
 *
 * 121   7/29/99 2:01p Kevin
 *
 * 120   7/29/99 12:31p Kevin
 * Mac merge fixes
 *
 * 119   7/28/99 2:06p Kevin
 * Macintosh Changes
 *
 * 118   7/15/99 6:38p Jeff
 * created function to get rendering stats
 *
 * 117   6/24/99 8:13p Jeff
 * OpenGL added for Linux
 *
 * 116   6/22/99 7:04p Jeff
 * added Glide support for Linux
 *
 * 115   5/13/99 3:47p Ardussi
 * changes for compiling on the Mac
 *
 * 114   5/04/99 4:34p Jason
 * changes for bumpmapping
 *
 * 113   5/01/99 9:26p Kevin
 * ifdef should have been ifndef
 *
 * 112   5/01/99 4:56p Kevin
 * made critical renderer code do nothing in DEDICATED_ONLY builds
 *
 * 111   4/16/99 6:45p Kevin
 * removed ifdef demo
 *
 * 110   4/16/99 3:22a Jeff
 * added ifdefs to specify what renderers to compile in (initially for
 * linux use)
 *
 * 109   4/14/99 1:44a Jeff
 * fixed case mismatched #includes
 *
 * 108   4/13/99 4:59p Jason
 * changes for font renderings
 *
 * 107   4/02/99 3:02p Samir
 * rend_RetrieveDDrawProps modified so renderer none works.
 *
 * 106   4/01/99 10:56a Jason
 * added better support for movie rendering
 *
 * 105   3/31/99 3:48p Jason
 * changes for cinematics
 *
 * 104   3/30/99 3:39p Jason
 * fixed some fog issues
 *
 * 103   3/29/99 7:29p Jason
 * made renderer handle default resolution more gracefully
 *
 * 102   3/24/99 11:55a Jason
 * added S3 texture compression
 *
 * 101   3/22/99 5:51p Jason
 * enhancements to mirrors
 *
 * 100   3/02/99 6:03p Jason
 * fixed opengl problem
 *
 * 99    2/26/99 3:32p Jason
 * made OpenGL/D3D not work with Voodoo3 OEM
 *
 * 98    2/17/99 1:05p Jason
 * revamped object/face/terrain selection code
 *
 * 97    2/16/99 11:36a Jason
 * fixed up some FindArg stuff
 *
 * 96    2/15/99 3:47p Jason
 * added screenshots to opengl
 *
 * 95    2/06/99 10:34p Jason
 * added renderer error message if no renderer is set
 *
 * 94    1/29/99 6:29p Jason
 * first pass at adding bumpmaps
 *
 * 93    1/22/99 1:08a Jason
 * added vid mem checking
 *
 * 92    1/18/99 10:45a Samir
 * added function to get DirectDraw object from Win32 apps.
 *
 * 91    12/08/98 2:30p Jason
 * fog rendering changes for patch
 *
 * 90    12/03/98 5:02p Jason
 * made glide work good with 128x128 lightmaps
 *
 * 89    11/30/98 2:27p Jason
 * more opengl fixes
 *
 * 88    11/30/98 12:03p Jason
 * fixed some opengl issues
 *
 * 87    11/30/98 11:15a Jason
 * fixed some renderer issues for the demo patch
 *
 * 86    11/19/98 6:55p Jason
 * hurray! Fixed TNT problems with Direct3d
 *
 * 85    11/11/98 2:07p Jason
 * added direct3d screenshots
 *
 * 84    10/22/98 10:31a Sean
 * fixed lighting in a window with opengl
 *
 * 83    10/22/98 12:27a Sean
 * put breaks in switch statements
 *
 * 82    10/21/98 9:28p Jason
 * Made no lightmaps work globally
 *
 * 81    10/21/98 12:06p Jason
 * changes for data paging
 *
 * 80    10/17/98 2:46p Jason
 * better error string reports
 *
 * 79    10/14/98 6:28p Jason
 * added Coplanar polygon stuff
 *
 * 78    10/12/98 7:20p Jason
 * fixed gamma for direct3d
 *
 * 77    10/08/98 3:36p Jason
 * fixes for the demo
 *
 * 76    9/25/98 9:25p Jason
 * did some graphics optimizations
 *
 * 75    9/25/98 1:17a Jason
 * added rend_SetCoplanarPolygonOffset functions
 *
 * 74    9/24/98 12:57p Jason
 * more state limited optimizations
 *
 * 73    9/18/98 1:28p Jason
 * cleaned up renderer initting
 *
 * 72    9/15/98 12:11p Jason
 * made special lines work in opengl
 *
 * 71    9/11/98 4:03p Jason
 * added better multitexture support
 *
 * 70    9/08/98 5:20p Jason
 * more state changing optimizations
 *
 * 69    9/02/98 5:41p Jason
 * adding lightmaps back to opengl
 *
 * 68    9/02/98 2:26p Jason
 * changes for state limiting
 *
 * 67    8/31/98 5:30p Jason
 * first pass at textures with Direct3d
 *
 * 66    7/16/98 2:30p Jason
 * added first pass at Direct3D support
 *
 * 65    6/11/98 3:38p Jason
 * added a general StateLimit render boolean for APIs that are state
 * limited (like OpenGL or D3D)
 *
 * 64    6/10/98 1:08p Jason
 * Fixed yet more OpenGL bugs
 *
 * 63    6/10/98 12:23p Jason
 * fixed some more bugs with windowed opengl
 *
 * 62    6/09/98 7:32p Jason
 * fixed windowed OpenGl bug
 *
 * 61    6/09/98 6:16p Jason
 * added some features for windowed opengl
 *
 * 60    6/09/98 4:47p Jason
 * added windowed openGL mode
 *
 * 59    5/20/98 5:44p Jason
 * incremental checkin for bumpmapping
 *
 * 58    5/19/98 2:23p Jeff
 * put in the rend_SetZBufferState(0) into DrawChunkedBitmap
 *
 * 57    5/18/98 7:07p Jeff
 * rend_DrawChunkBitmap now calls rend_DrawSimpleBitmap instead of
 * rend_DrawScaledBitmap
 *
 * 56    5/11/98 6:14p Jason
 * some minor fixes for opengl
 *
 * 55    5/06/98 1:39p Jason
 * added rend_SetResolution
 *
 * 54    5/05/98 3:02p Jason
 * attempting to add different screen resolutions
 *
 * 53    5/05/98 1:01p Jeff
 * Added a rend_DrawScaledChunkBitmap
 *
 * 52    4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 51    4/17/98 4:27p Jason
 * added alpha per vertex stuff to drawscaledbitmap
 *
 * 50    4/18/98 3:25a Samir
 * changed some flag in rend_DrawChunkedBitmap
 *
 * 49    4/08/98 3:07p Jeff
 * changed the zvalue parameter within DrawSimpleBitmap
 *
 * 48    4/08/98 12:27p Jason
 * added rend_drawsimplebitmap
 *
 * 47    4/01/98 12:02p Jason
 * incremental checkin for rendering changes
 *
 * 46    3/13/98 1:22p Jason
 * Moved UseHardware flag to the renderer lib where it belongs
 *
 * 45    3/02/98 5:53p Jason
 * added gamma functionality
 *
 * 44    2/27/98 4:46p Jason
 * added more opengl support
 *
 * 43    2/26/98 11:00a Jason
 * incremental opengl functionality
 *
 * 42    2/17/98 2:02p Jason
 * fixed clamping bug for chunked bitmaps
 *
 * 41    2/16/98 11:43a Jason
 * more opengl stuff
 *
 * 40    2/14/98 10:48p Jason
 * got preferred rendering working
 *
 * 39    2/13/98 3:56p Jason
 * added much better opengl support
 *
 * 38    2/12/98 1:32p Jason
 * got mipmapping working
 *
 * 37    1/28/98 5:37p Jason
 * added streamer weapons
 *
 * 36    1/28/98 12:54p Jason
 * made renderer auto shutdown on program close
 *
 *
 * 35    1/19/98 2:32p Jason
 * added the ability to set clear flags on rend_StartFrame
 *
 * 34    1/16/98 11:54a Samir
 * Added support for rendering chunked bitmaps.
 *
 * 33    1/14/98 5:49p Jeff
 * tex_LFBLock stuff added (samir.)
 *
 * 32    12/29/97 5:51p Samir
 * rend_Flip should call ddvid_Flip for software flip.
 *
 * 31    12/22/97 7:34p Samir
 * Removed instances of gr.h include.  Replaced with grdefs.h
 *
 * 30    12/19/97 5:22p Samir
 * Added more drawing primatives for software.
 *
 * 29    12/19/97 2:36p Jason
 * more fixes for 2d/3d integration
 *
 * 28    12/19/97 12:20p Jason
 * changes for better 2d/3d system integration
 *
 * 27    11/14/97 3:44p Jason
 * added transparency to blitting function
 *
 * 26    11/13/97 4:06p Jason
 * added rend_DrawLFBBitmap
 *
 * 25    11/13/97 3:52p Jason
 * added lfb stuff for renderer
 *
 * 24    10/29/97 12:36p Jason
 * remove byte/bool conflicts in the renderer lib
 *
 * 23    10/24/97 4:39p Jason
 * added support to not write into the zbuffer
 *
 * 22    10/22/97 5:01p Jason
 * added rend_SetZBias
 *
 * 21    10/20/97 4:46p Jason
 * changes for explosions
 *
 * 20    10/13/97 3:56p Jason
 * made a better 3d bitmap system
 *
 * 19    9/16/97 6:23p Jason
 * took out software z buffer from rend_SetZBuffer
 *
 * 18    9/16/97 4:09p Jason
 * implemented software zbuffer
 *
 * 17    9/09/97 12:34p Jason
 * made software transparency work correctly with new alpha type
 *
 * 16    9/09/97 11:45a Jason
 * changed the way alpha works with the renderer
 *
 * 15    8/29/97 5:42p Jason
 * fixed hud rendering problems
 *
 * 14    8/29/97 11:59a Jason
 * implemented screenshot functions
 *
 * 13    8/24/97 2:45p Jason
 * implemented texture wrapping
 *
 * 12    8/07/97 11:46a Jason
 * implemented tmap overlay system
 *
 * 11    8/04/97 6:46p Jason
 * added code for a lightmap system
 *
 * 10    8/04/97 3:28p Jason
 * added alpha blending per texture
 *
 * 9     7/20/97 7:36p Jason
 * added support for colored fog
 *
 * 8     7/18/97 12:57p Jason
 * added line drawing for glide
 *
 * 13    6/16/97 5:02p Jason
 * added flip function
 *
 * 12    6/16/97 3:44p Jason
 * added line drawing
 *
 * 11    6/16/97 2:54p Jason
 * added rend_DrawFontCharacter function for drawing fonts
 *
 * 10    6/16/97 2:34p Jason
 * added 3dfx support
 *
 * 9     6/06/97 11:35a Jason
 * added missing functions for pixel primitives
 *
 * 8     6/03/97 12:19p Jason
 * more functions added for opengl
 *
 * 7     5/29/97 6:26p Jason
 * incremental changes for opengl/lighting support
 *
 * 6     5/27/97 4:39p Jason
 * changes to support OpenGL
 *
 * 5     5/22/97 11:59a Jason
 * add a ScaleBitmap function to the abstracted render library
 *
 * 4     5/21/97 7:35p Matt
 * Move Current_surface & viewport locking from 3D to renderer library
 *
 * $NoKeywords: $
 */
#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ddvid.h"
#include "pstypes.h"
#include "mono.h"
#include "pserror.h"
#include "ddio.h"
#include "bitmap.h"
#include "application.h"
#include "3d.h"
#include "tr_local.h"

// Renderers for Windows version.  Mac gets these through a global include file.
#if defined(WIN32)
#define USE_OPENGL
// #define USE_GLIDE
//#define USE_D3D
// #define USE_SOFTWARE
#endif

// Sets the state of the OpenGLWindow to on or off
static renderer_type Save_rend;
static bool Save_state_limit;
void rend_SetOpenGLWindowState(int state, oeApplication *app, renderer_preferred_state *pref_state) {
#ifdef USE_OPENGL
  if (state) {
   // if (!OpenGL_window_initted) {
    rend_InitOpenGLWindow(app, pref_state);
 //     else
 //       return;
 //   }
    UseHardware = 1;
    Save_rend = Renderer_type;
    Save_state_limit = StateLimited;
    Renderer_type = RENDERER_OPENGL;
    StateLimited = 1;
    NoLightmaps = false;
  } else {
    if (OpenGL_window_initted) {
      UseHardware = 0;
      Renderer_type = RENDERER_SOFTWARE_16BIT;
      StateLimited = Save_state_limit;
    }
  }
#endif
}
