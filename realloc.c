/*
    DPCRTLMM Memory Manager Library reallocator
    Copyright (C) 2000 David Duncan Ross Palmer, Daybo Logic.

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
/* Created: UNKNOWN
   Last modified: 31st July 2001
   Programmer: Overlord David Duncan Ross Palmer
   Library: DPCRTLMM
   Language: ANSI C (1990 implementation)
   Purpose: DPCRTLMM's memory user-memory reallocation

   1st Dec 2000: To fix a possible porting problem I no longer assume
                 that realloc() will allocate when called with NULL, newSize

   31st July 2001: Added support for using the big library lock.
*/
#include <stdlib.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#ifdef DPCRTLMM_WANTFARDATA
#  include <alloc.h>
#endif /*DPCRTLMM_WANTFARDATA*/
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "vptrap.h" /* _VerifyPtrs() */
#include "locktrap.h" /* _LockTrap() */
#include "iblkptr.h"
#include "dbghooks.h" /* Debug hooking routines, we use the executive */
#include "log.h"
#include "biglock.h" /* Mutual exclusion */
/*-------------------------------------------------------------------------*/
static void DPCRTLMM_FARDATA* dpcrtlmm_int_Realloc(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA* OldBlockPtr, const size_t NewSize);
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_Realloc(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA* OldBlockPtr, const size_t NewSize)
{
  void DPCRTLMM_FARDATA* ret;

  LOCK
  ret = dpcrtlmm_int_Realloc(PBlockArray, OldBlockPtr, NewSize);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static void DPCRTLMM_FARDATA* dpcrtlmm_int_Realloc(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA* OldBlockPtr, const size_t NewSize)
{
  void DPCRTLMM_FARDATA* ptr = OldBlockPtr; /* Pointer that is returned to the caller (modified later) */
  const char funcName[] = "Realloc()"; /* Name of our function */
  unsigned int blockIndex; /* Index of block in the array */
  char DPCRTLMM_FARDATA* sizePtr; /* Pointer used during resizing */
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr;
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo; /* Used for advanced debug hooks! */
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  PRArr = _ResolveArrayPtr(PBlockArray);
  _VerifyPtrs(funcName, PBlockArray, OldBlockPtr); /* Do trap if passed pointers are invalid */
  if (_LockTrap(funcName, PBlockArray, OldBlockPtr)) /* Do trap if block is locked */
    return NULL;

  if (!NewSize) /* No new size, hmm, must be wanting free() really */
  {
    WARNING("Dynamic possibly non-portable use of realloc() as a free-er");
    dpcrtlmm_Free(PBlockArray, OldBlockPtr); /* Give the caller what they want */
    return NULL;
  }

  if ( !OldBlockPtr ) /* This is a non-portable attempt to use realloc as an initial allocator */
  {
    WARNING("Dynamic possibly non-portable use of realloc() as an initial allocator");
    return dpcrtlmm_Alloc(PBlockArray, NewSize);
  }

  blockIndex = dpcrtlmm_int_IndexFromBlockPtr(PRArr, OldBlockPtr);
  if ( PRArr->Descriptors[blockIndex].Size == NewSize ) /* Same size ?! */
  {
    /* The block is already the requested size! */
    return ptr; /* Give present pointer back to caller wihout touching it */
  }
  /* The resize is valid */

  sizePtr = DPCRTLMM_REALLOC( OldBlockPtr, NewSize ); /* Attempt to resize the block */
  if (!sizePtr) return NULL; /* If the block cannot be enlarged return NULL to the caller to indicate the failure */

  #ifdef DPCRTLMM_DEBUGHOOKS
  /* Call debug hooks */
  debugHookInfo.PRelArr = PRArr;
  debugHookInfo.PRelDesc = OldBlockPtr; /* Do NOT dereference, base may have changed */
  debugHookInfo.HookType = DPCRTLMM_HOOK_REALLOC;
  /* Set AllocReq to size difference */
  debugHookInfo.AllocReq = DPCRTLMM_MAX(PRArr->Descriptors[blockIndex].Size, NewSize) - DPCRTLMM_MIN(PRArr->Descriptors[blockIndex].Size, NewSize);
  if ( NewSize < PRArr->Descriptors[blockIndex].Size ) /* Negate number */
  {
    debugHookInfo.Misc0 |= 1; /* Set bit 0 */
  }
  else /* Positive number */
  {
    debugHookInfo.Misc0 &= ~1; /* Clear bit 0 */
  }
  /* Misc1 points to the new block, the hook routine can dereference it if it wants to */
  debugHookInfo.Misc1 = (unsigned long)sizePtr; /* Hook routine should cast it back to void* */
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_REALLOC, &debugHookInfo);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  ptr = sizePtr; /* Set new pointer as the return value */
  /* Update block descriptor */
  PRArr->Descriptors[blockIndex].PBase = ptr;
  PRArr->Descriptors[blockIndex].Size = NewSize;

  return ptr; /* Give new pointer to caller */
}
/*-------------------------------------------------------------------------*/
