/*
 * $Logfile: /DescentIII/Main/Dmfc/dmfctimer.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * DMFC timer API functions
 *
 * $Log: dmfctimer.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 8     5/23/99 2:21a Jeff
 * fixed timer bugs with intervals
 *
 * 7     5/13/99 4:56p Ardussi
 * changes for compiling on the Mac
 *
 * 6     3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 5     2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 4     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern char *_DMFCErrorString;
extern DMFCBase *basethis;
char *DMFCGetString(int d);

// DMFCBase::InitTimers
//
//	Inializes the timers
void DMFCBase::InitTimers(void) {
  for (int i = 0; i < MAX_DMFC_TIMERS; i++) {
    Timers[i].inuse = false;
  }
}

// DMFCBase::KillAllTimers
//
//	Kills all running timers
void DMFCBase::KillAllTimers(void) {
  for (int i = 0; i < MAX_DMFC_TIMERS; i++) {
    KillTimer(i);
  }
}

// DMFCBase::SetTimerInterval
//
//	Starts a timer event running
int DMFCBase::SetTimerInterval(void (*func)(void), float intval, float longevity, void (*onkill)()) {
  int i;

  // Find a free timer
  for (i = 0; i < MAX_DMFC_TIMERS; i++) {
    if (!Timers[i].inuse)
      break;
  }

  if (i >= MAX_DMFC_TIMERS) {
    return -1; // no free timers
  }

  // insert this timer information
  Timers[i].inuse = true;
  Timers[i].handler = func;
  Timers[i].starttime = *Gametime;
  Timers[i].longevity = longevity;
  Timers[i].intval = intval;
  Timers[i].nexttime = Timers[i].starttime + Timers[i].intval;
  Timers[i].onkill = onkill;
  return i;
}

// DMFCBase::KillTimer
//
// Kills a timer
void DMFCBase::KillTimer(int handle) {
  if ((handle >= 0) && (handle < MAX_DMFC_TIMERS)) {
    if (Timers[handle].inuse) {
      Timers[handle].inuse = false;
      if (Timers[handle].onkill)
        (*Timers[handle].onkill)();
    }
  }
}

// DMFCBase::ProcessTimers
//
//	Goes through all the timers and updates them if they need to be updated
void DMFCBase::ProcessTimers(void) {
  for (int i = 0; i < MAX_DMFC_TIMERS; i++) {
    if (Timers[i].inuse) {
      // we have a running timer here
      float currtime = *Gametime;
      if (Timers[i].nexttime <= currtime) {
        // time to call
        Timers[i].nexttime += Timers[i].intval;
        if (Timers[i].handler)
          (*Timers[i].handler)();
      }
      if (Timers[i].longevity != -1) {
        if (Timers[i].longevity + Timers[i].starttime < currtime) {
          // this timer is over with
          KillTimer(i);
        }
      }
    }
  } // end for
}
