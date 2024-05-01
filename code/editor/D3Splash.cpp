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

// D3Splash.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "D3Splash.h"

#include <stdarg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int SPLASHTEXT_LEFT = 10;
const int SPLASHTEXT_TOP = 180;
const int SPLASHTEXT_RIGHT = 310;
const int SPLASHTEXT_BOTTOM = 230;

CD3Splash *D3_splash_screen;

/////////////////////////////////////////////////////////////////////////////
// CD3Splash dialog


CD3Splash::CD3Splash(CWnd* pParent /*=NULL*/)
	: CDialog(CD3Splash::IDD, pParent)
{
	//{{AFX_DATA_INIT(CD3Splash)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ParentWnd = pParent;
}


void CD3Splash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CD3Splash)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BOOL CD3Splash::Create()
{
	m_TextLines[0][0] = 0;
	m_TextLines[1][0] = 0;
	m_TextLines[2][0] = 0;
	m_CurLine = 0;

	D3_splash_screen = this;

	m_SplashBmp.LoadBitmap(IDB_D3SPLASH);

	return CDialog::Create(CD3Splash::IDD, m_ParentWnd);
}


void CD3Splash::PutText(char *buf)
{
	RECT rect;

	if (m_CurLine == 3) {
		for (int i = 0; i < 2; i++)
			lstrcpy(&m_TextLines[i][0], &m_TextLines[i+1][0]);
	}
	else m_CurLine++;

	lstrcpy(&m_TextLines[m_CurLine-1][0], buf);

//	mprintf((1, "%d:%s",m_CurLine-1, m_TextLines[m_CurLine-1]));
	
	SetRect(&rect, SPLASHTEXT_LEFT, SPLASHTEXT_TOP, SPLASHTEXT_RIGHT, SPLASHTEXT_BOTTOM);
	InvalidateRect(&rect, FALSE);
	UpdateWindow();
}



BEGIN_MESSAGE_MAP(CD3Splash, CDialog)
	//{{AFX_MSG_MAP(CD3Splash)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CD3Splash message handlers

void CD3Splash::OnCancel()
{
//	Absolutely nothing
}


void CD3Splash::OnOK()
{
//	Absolutely nothing
}


void CD3Splash::PostNcDestroy() 
{
//	needed for modeless dialogs
	delete this;
}

void CD3Splash::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  CDC sdc;           // source dc
  CBitmap *bmp;
  CFont fnt, smallFnt, rightTextFnt; // Two fonts, original and a smaller one
  CFont *oldFont;
  CSize textdim;
  BITMAP bm;
  RECT uprect;
  int x, y;

  GetClientRect(&uprect);

  m_SplashBmp.GetObject(sizeof(bm), &bm);

  sdc.CreateCompatibleDC(NULL);
  bmp = sdc.SelectObject(&m_SplashBmp);
  dc.StretchBlt(uprect.left, uprect.top, uprect.right - uprect.left, uprect.bottom - uprect.top, &sdc, 0, 0, bm.bmWidth,
                bm.bmHeight, SRCCOPY);
  sdc.SelectObject(bmp);

  // Original font setup
  fnt.CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Times New Roman");
  oldFont = dc.SelectObject(&fnt);
  dc.SetBkMode(TRANSPARENT);
  dc.SetTextColor(RGB(255, 255, 255));

  // Draw the main title text
  const char *mainTitle = "Descent 3 Apex Editor";
  textdim = dc.GetTextExtent(mainTitle, strlen(mainTitle));
  y = uprect.bottom - (textdim.cy + 20); // Adjusted for padding
  dc.TextOut(uprect.left + 10, y, mainTitle, strlen(mainTitle));

  // Smaller font setup for copyright text, 30% smaller than previously, using Times New Roman
  smallFnt.CreateFont(12, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                      DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Times New Roman");
  dc.SelectObject(&smallFnt);
  dc.SetTextColor(RGB(128, 128, 128)); // Gray color

  // Draw the copyright text
 // const char *copyrightText = "(c) 2024 Justin Marshall";
  char versionText[512];
  sprintf(versionText, "Build: %s %s", __DATE__, __TIME__);
  textdim = dc.GetTextExtent(versionText, strlen(versionText));
  y += textdim.cy + 5; // Slight padding between lines
  dc.TextOut(uprect.left + 10, y, versionText, strlen(versionText));

   // Right text font setup
  rightTextFnt.CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Times New Roman");
  dc.SelectObject(&rightTextFnt);
  dc.SetTextColor(RGB(255, 255, 255)); // White color for the side text

  const char *copyrightText = "(c) 2024 Justin Marshall";
  textdim = dc.GetTextExtent(copyrightText, strlen(copyrightText));
  x = uprect.right - textdim.cx - 10;          // Right align with padding
  y = uprect.bottom - (textdim.cy + 20); // Adjusted for padding
  dc.TextOut(x, y, copyrightText, strlen(copyrightText));

  // Re-select the old font to clean up
  dc.SelectObject(oldFont);
}


void CD3Splash::OnDestroy() 
{
	CDialog::OnDestroy();

	D3_splash_screen = NULL;
}
