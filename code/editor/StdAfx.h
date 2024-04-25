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

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxole.h>			// MFC OLE support (ADDED BY SAMIR)
#include <windows.h>

void rend_SetupPixelFormatForTools(HDC hDC);
void rend_MakeCurrent(HWND hwnd, HDC hdc);

// mouse state
typedef struct {
  int x, y;       // current position
  int oldx, oldy; // last position
  bool left;      // left button
  bool right;     // right button
  bool mid;
} stMouse;

typedef struct {
  bool up;
  bool down;
  bool left;
  bool right;
  bool subtract;
  bool add;
  bool num0;
  bool num5;
  bool num1;
  bool num3;
  bool num7;
  bool num9;
  bool shift;
  bool ctrl;
  bool alt;
  int zoom;
} stKeys;

// Various 64bit wrapper functions.
#undef GetWindowLong
#undef SetWindowLong
#undef SetClassLong
#undef LONG

#define GWL_WNDPROC GWLP_WNDPROC
#define SetClassLong SetClassLongPtr
#define GetWindowLong GetWindowLongPtr
#define SetWindowLong SetWindowLongPtr
#define GWL_USERDATA GWLP_USERDATA
#define GCL_HICON GCLP_HICON
#define DWL_MSGRESULT DWLP_MSGRESULT
#define DWL_DLGPROC DWLP_DLGPROC

#ifndef GCL_HBRBACKGROUND
#define GCL_HBRBACKGROUND (-10)
#endif