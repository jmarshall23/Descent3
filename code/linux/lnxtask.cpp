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
 * $Logfile: /DescentIII/Main/linux/lnxtask.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2000/04/18 00:00:39 $
 * $Author: icculus $
 *
 * Linux multitasking routines
 *
 * $Log: lnxtask.cpp,v $
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 3     7/14/99 9:09p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include "DDAccess.h"
#include "TaskSystem.h"
#include "pserror.h"

osMutex::osMutex() {}

osMutex::~osMutex() { Destroy(); }

bool osMutex::Create() { return false; }

void osMutex::Destroy() {}

bool osMutex::Acquire(int timeout) {}

void osMutex::Release() {}