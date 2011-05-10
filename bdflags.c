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
/* Raw block descriptor flag modifiers : Overlord David Duncan Ross Palmer
Copyright Y2K Daybo Logic, all rights reserved.
Now supports NULL arrays
*/
#include <stddef.h>
#include <string.h> /* memset() */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "vptrap.h" /* _VerifyPtrs() (replaces BBA/BBP traps) */
#include "iblkptr.h" /* For getting array descriptor index for a particular block */
#include "dbghooks.h" /* The debug hook executive */
/*-------------------------------------------------------------------------*/
/* NOTE: Adding of the hook caller in here has caused two variables
both holding the index of the block, this should be optimized away when
I can be bothered */
unsigned char dpcrtlmm_ModifyDescriptorFlags(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned char* PNewFlags)
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
