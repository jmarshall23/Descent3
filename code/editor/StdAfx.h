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