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
 * $Logfile: /DescentIII/Main/lib/ddvid.h $
 * $Revision: 4 $
 * $Date: 12/29/97 5:50p $
 * $Author: Samir $
 *
 *	Video library.
 *
 * $Log: /DescentIII/Main/lib/ddvid.h $
 *
 * 4     12/29/97 5:50p Samir
 * Added ability to close ddvid system.
 *
 * 3     12/22/97 7:13p Samir
 * Moved constants to grdefs.h
 *
 * 2     12/22/97 6:58p Samir
 * Restored type ddgr_color
 *
 * 1     12/22/97 12:45p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef DDVID_H
#define DDVID_H

#include "pstypes.h"
#include "grdefs.h"

class oeApplication;

//	called first to allow fullscreen video access
bool ddvid_Init(oeApplication *app, char *driver);
void ddvid_Close();

//	sets the appropriate video mode.
bool ddvid_SetVideoMode(int w, int h, int color_depth, bool paged);

//	sets screen handle
void ddvid_SetVideoHandle(unsigned handle);

//	retrieves screen information
void ddvid_GetVideoProperties(int *w, int *h, int *color_depth);

//	retrieves screen aspect ratio.
float ddvid_GetAspectRatio();

//	retreives frame buffer info for a video mode.
void ddvid_LockFrameBuffer(ubyte **data, int *pitch);
void ddvid_UnlockFrameBuffer();

//	flips screen if there's a back buffer
void ddvid_VideoFlip();

// only available to DD_ACCESS libraries.
#if defined(DD_ACCESS_RING)
#if defined(WIN32)

//	dd_obj is the DIRECTDRAW OBJECT for the system.
//	dds_obj is the DIRECTDRAWSURFACE OBJECT for the screen
void ddvid_GetVideoDDrawProps(uint *dd_obj, uint *dds_obj);

#endif // WIN32

#endif // DD_ACCESS

#endif
