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
 * $Logfile: /DescentIII/Main/linux/registry.h $
 * $Revision: 1.2 $
 * $Date: 2004/02/09 04:14:51 $
 * $Author: kevinb $
 *
 * Linux registry header
 *
 * $Log: registry.h,v $
 * Revision 1.2  2004/02/09 04:14:51  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 4     7/14/99 9:09p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#ifndef __REGISTRY_H_
#define __REGISTRY_H_

#define MAX_RECORD_NAME 256
#define REGT_STRING 0
#define REGT_DWORD 1

#include <stdio.h>

typedef struct tRecord {
  char name[MAX_RECORD_NAME];
  char type;
  void *data;
  tRecord *next;
} tRecord;

typedef struct tKey {
  char name[MAX_RECORD_NAME];
  tKey *next;
  tRecord *records;
} tKey;

class CRegistry {
public:
  CRegistry(char *name);
  ~CRegistry();
  void Export();
  bool Import();
  void CreateKey(char *name);
  bool LookupKey(char *name);
  bool CreateRecord(char *name, char type, void *data);
  tRecord *LookupRecord(char *record, void *data);
  int GetDataSize(char *record);
  void GetSystemName(char *name);
  void SetSystemName(char *name);

private:
  void Destroy(void);
  void DestroyKey(tKey *key);
  void DestroyRecord(tRecord *record);
  void ExportKey(tKey *key, FILE *file);
  void ExportRecord(tRecord *record, FILE *file);
  char name[MAX_RECORD_NAME];
  tKey *root;
  tKey *currentkey;
};

#endif
