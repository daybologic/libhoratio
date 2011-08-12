/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2012, David Duncan Ross Palmer, Daybo Logic
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
      
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
      
    * Neither the name of the Daybo Logic nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Raw block descriptor flag modifiers
  Now supports NULL arrays
*/
#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
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
/*
  NOTE: Adding of the hook caller in here has caused two variables
  both holding the index of the block, this should be optimised away when
  I can be bothered
*/
/*-------------------------------------------------------------------------*/
unsigned char dpcrtlmm_ModifyDescriptorFlags(
  const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const void DPCRTLMM_FARDATA *Ptr,
  const unsigned char *PNewFlags
)
{
  unsigned char ret;

  LOCK
  ret = dpcrtlmm_int_ModifyDescriptorFlags(
    PBlockArray,
    Ptr,
    PNewFlags
  );
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
unsigned char dpcrtlmm_int_ModifyDescriptorFlags(
  const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const void DPCRTLMM_FARDATA *Ptr,
  const unsigned char *PNewFlags
)
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
  /* Looked up the right descriptor to suit hook requirements */
  debugHookInfo.PRelDesc = &_ResolveArrayPtr(PBlockArray)->Descriptors[indexOfBlock];
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

