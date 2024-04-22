/*
* Descent 3
* Copyright (C) 2024 Parallax Software
*
* Descent 3: Frostbite Port
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

// SoundListCombo.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "SoundListCombo.h"
#include "soundload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundListCombo

CSoundListCombo::CSoundListCombo()
{
}

CSoundListCombo::~CSoundListCombo()
{
}


BEGIN_MESSAGE_MAP(CSoundListCombo, CComboBox)
	//{{AFX_MSG_MAP(CSoundListCombo)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundListCombo message handlers

void CSoundListCombo::Init(int selected) 
{
	//Clear the combo box
	ResetContent();

	//Add the "none" sound
	AddString("<none>");
	SetItemData(0,-1);
	SetCurSel(0);		//default
	
	//Now add all the sounsd
	for (int i=0;i<MAX_SOUNDS;i++) {
		if (Sounds[i].used) {
			int index;

			index = AddString(Sounds[i].name);
			SetItemData(index,i);
			if (selected == i)
				SetCurSel(index);
		}
	}
}

void CSoundListCombo::SetSelected(int selected) 
{
	//Now add all the sounsd
	int count = GetCount();
	for (int i=0;i<count;i++) {
		if (selected == (int) GetItemData(i))
			SetCurSel(i);
	}
}

int CSoundListCombo::GetSelected() 
{
	return (int) GetItemData(GetCurSel());
}

#include "mono.h"

void CSoundListCombo::OnSetfocus() 
{
	//Update list in case the items have changed
	Init(GetSelected());
}
