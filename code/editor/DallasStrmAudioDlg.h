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

#if !defined(AFX_DALLASSTRMAUDIODLG_H__5D495B20_C7F1_11D2_A4E0_00A0C96ED60D__INCLUDED_)
#define AFX_DALLASSTRMAUDIODLG_H__5D495B20_C7F1_11D2_A4E0_00A0C96ED60D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DallasStrmAudioDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDallasStrmAudioDlg dialog

class CDallasStrmAudioDlg : public CDialog
{
// Construction
public:
	CString	m_Filename;

	CDallasStrmAudioDlg(CWnd* pParent = NULL);   // standard constructor

	void FillStrmAudioList(void);

// Dialog Data
	//{{AFX_DATA(CDallasStrmAudioDlg)
	enum { IDD = IDD_DALLAS_STRM_AUDIO_DIALOG };
	CListBox	m_StrmAudioBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDallasStrmAudioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDallasStrmAudioDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkStrmAudioList();
	afx_msg void OnPlayStrmAudioButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DALLASSTRMAUDIODLG_H__5D495B20_C7F1_11D2_A4E0_00A0C96ED60D__INCLUDED_)
