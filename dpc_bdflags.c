/*
    DPCRTLMM Memory management library : Descriptor flags modifiers
    Copyright (C) 2000-2002 David Duncan Ross Palmer, Daybo Logic.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


Contact me: Overlord@DayboLogic.co.uk
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/
#define DPCRTLMM_SOURCE
/* Raw block descriptor flag modifiers : Overlord David Duncan Ross Palmer
Copyright Y2K Daybo Logic, all rights reserved.
Now supports NULL arrays
*/
#include <stddef.h>
#include <string.h> /* memset() */
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_vptrap.h" /* _VerifyPtrs() (replaces BBA/BBP traps) */
#include "dpc_iblkptr.h" /* For getting array descriptor index for a particular block */
#include "dpc_dbghooks.h" /* The debug hook executive */
#include "dpc_biglock.h" /* Library's mutual exclusion */
#include "dpc_bdflags.h"
/*-------------------------------------------------------------------------*/
/* NOTE: Adding of the hook caller in here has caused two variables
both holding the index of the block, this should be optimised away when
I can be bothered */
/*-------------------------------------------------------------------------*/
unsigned char dpcrtlmm_ModifyDescriptorFlags(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned char* PNewFlags)
{
  unsigned char ret;

  LOCK
  ret = dpcrtlmm_int_ModifyDescriptorFlags(PBlockArray, Ptr, PNewFlags);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
unsigned char dpcrtlmm_int_ModifyDescriptorFlags(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned char* PNewFlags)
{
  /* locals */
  const char funcName[] = "ModifyDescriptorFlags()"; /* Name of this function */
  unsigned int blockIndex; /* Index of block descriptor into the array */
  unsigned char oldFlags; /* Old flags, returned to caller */
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  unsigned int indexOfBlock;
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr; /* Holds resolved pointer array */

  /* Bah, this is a pain supporting this particular hook */
  #ifdef DPCRTLMM_DEBUGHOOKS
  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));

  debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
  indexOfBlock = dpcrtlmm_int_IndexFromBlockPtr(PBlockArray, Ptr);
  debugHookInfo.PRelDesc = &_ResolveArrayPtr(PBlockArray)->Descriptors[indexOfBlock]; /* Looked up the right descriptor to suit hook requirements */
  debugHookInfo.HookType = DPCRTLMM_HOOK_MODIFYDESCFLAGS;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  _VerifyPtrs(funcName, PBlockArray, Ptr); /* Make sure invalid pointers don't get past here */

  PRArr = _ResolveArrayPtr(PBlockArray); /* Makes NULL goto &_defaultArray */
  blockIndex = _IndexFromBlockPtr(PRArr, Ptr); /* Get the index */
  oldFlags = PRArr->Descriptors[blockIndex].Flags; /* Save current flags */
  if (PNewFlags) /* Caller passed new flags */
    PRArr->Descriptors[blockIndex].Flags = *PNewFlags; /* Modify the flags */

  #ifdef DPCRTLMM_DEBUGHOOKS
  debugHookInfo.Success = 1U;
  debugHookInfo.Misc0 = (unsigned long)oldFlags;
  debugHookInfo.Misc1 = (unsigned long)( (PNewFlags) ? (*PNewFlags) : (oldFlags) );
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_MODIFYDESCFLAGS, &debugHookInfo);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  return oldFlags; /* Give the old flags back to the caller */
}
/*-------------------------------------------------------------------------*/
