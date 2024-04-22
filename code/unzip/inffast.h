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

/*
 * $Logfile: /DescentIII/Main/unzip/inffast.h $
 * $Revision: 2 $
 * $Date: 8/13/99 8:01p $
 * $Author: Jeff $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/Main/unzip/inffast.h $
 *
 * 2     8/13/99 8:01p Jeff
 * initial creation of zip class
 *
 * $NoKeywords: $
 */

/* inffast.h -- header to use inffast.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

extern int inflate_fast OF((uInt, uInt, inflate_huft *, inflate_huft *, inflate_blocks_statef *, z_streamp));
