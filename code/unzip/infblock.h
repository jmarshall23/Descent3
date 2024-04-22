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
 * $Logfile: /DescentIII/Main/unzip/infblock.h $
 * $Revision: 2 $
 * $Date: 8/13/99 8:01p $
 * $Author: Jeff $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/Main/unzip/infblock.h $
 *
 * 2     8/13/99 8:01p Jeff
 * initial creation of zip class
 *
 * $NoKeywords: $
 */

/* infblock.h -- header to use infblock.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

struct inflate_blocks_state;
typedef struct inflate_blocks_state FAR inflate_blocks_statef;

extern inflate_blocks_statef *inflate_blocks_new OF((z_streamp z, check_func c, /* check function */
                                                     uInt w));                  /* window size */

extern int inflate_blocks OF((inflate_blocks_statef *, z_streamp, int)); /* initial return code */

extern void inflate_blocks_reset OF((inflate_blocks_statef *, z_streamp, uLongf *)); /* check value on output */

extern int inflate_blocks_free OF((inflate_blocks_statef *, z_streamp));

extern void inflate_set_dictionary OF((inflate_blocks_statef * s, const Bytef *d, /* dictionary */
                                       uInt n));                                  /* dictionary length */

extern int inflate_blocks_sync_point OF((inflate_blocks_statef * s));
