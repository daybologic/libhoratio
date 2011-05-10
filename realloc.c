/**********************************************************************
 *                                                                    *
 * "DPCRTLMM" David Palmer's C-RTL Memory Manager Copyright (c) 2000  *
 * David Duncan Ross Palmer, Daybo Logic all rights reserved.         *
 * http://daybologic.com/Dev/dpcrtlmm                                 *
 *                                                                    *
 * D.D.R. Palmer's official homepage: http://daybologic.com/overlord  *
 * See the included license file for more information.                *
 *                                                                    *
 **********************************************************************
*/
#define DPCRTLMM_SOURCE
/* Created: UNKNOWN
   Last modified: 21st July 2000
   Programmer: Overlord David Duncan Ross Palmer
   Library: DPCRTLMM
   Language: ANSI C (1990 implementation)
   Purpose: DPCRTLMM's memory user-memory reallocation
*/
#include <stdlib.h>
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
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_Realloc(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA* OldBlockPtr, const size_t NewSize)
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
    dpcrtlmm_Free(PBlockArray, OldBlockPtr); /* Give the caller what they want */
    return NULL;
  }

  blockIndex = dpcrtlmm_int_IndexFromBlockPtr(PRArr, OldBlockPtr);
  if ( PRArr->Descriptors[blockIndex].Size == NewSize ) /* Same size ?! */
  {
    /* The block is already the requested size! */
    return ptr; /* Give present pointer back to caller wihout touching it */
  }
  /* The resize is valid if the block was not already the requested size */
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
