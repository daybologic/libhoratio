/*
Horatio's Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer (M6KVM), Daybo Logic
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
#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <stddef.h>
#include <string.h> /* memset() */
#include <stdio.h>
#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library data */
#include "hvptrap.h" /* _VerifyPtrs() (replaces BBA/BBP traps) */
#include "hiblkptr.h" /* For getting array descriptor index for a particular block */
#include "hdbghook.h" /* The debug hook executive */
#include "hbiglock.h" /* Library's mutual exclusion */
#include "hbdflags.h"

/*
  NOTE: Adding of the hook caller in here has caused two variables
  both holding the index of the block, this should be optimised away when
  I can be bothered
*/

unsigned char horatio_ModifyDescriptorFlags(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *Ptr,
  const unsigned char *PNewFlags
) {
  unsigned char ret;

  LOCK
  ret = horatio_int_ModifyDescriptorFlags(
    PBlockArray,
    Ptr,
    PNewFlags
  );
  UNLOCK

  return ret;
}

unsigned char horatio_int_ModifyDescriptorFlags(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *Ptr,
  const unsigned char *PNewFlags
) {
  /* locals */
  const char funcName[] = "ModifyDescriptorFlags()"; /* Name of this function */
  unsigned int blockIndex; /* Index of block descriptor into the array */
  unsigned char oldFlags; /* Old flags, returned to caller */
  #ifdef HORATIO_DEBUGHOOKS
  S_HORATIO_DEBUGHOOKINFO debugHookInfo;
  unsigned int indexOfBlock;
  #endif /*HORATIO_DEBUGHOOKS*/
  PS_HORATIO_BLOCKDESCARRAY PRArr; /* Holds resolved pointer array */

  /* Bah, this is a pain supporting this particular hook */
  #ifdef HORATIO_DEBUGHOOKS
  memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));

  debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
  indexOfBlock = horatio_int_IndexFromBlockPtr(PBlockArray, Ptr);
  /* Looked up the right descriptor to suit hook requirements */
  debugHookInfo.PRelDesc = &_ResolveArrayPtr(PBlockArray)->Descriptors[indexOfBlock];
  debugHookInfo.HookType = HORATIO_HOOK_MODIFYDESCFLAGS;
  #endif /*HORATIO_DEBUGHOOKS*/

  _VerifyPtrs(funcName, PBlockArray, Ptr); /* Make sure invalid pointers don't get past here */

  PRArr = _ResolveArrayPtr(PBlockArray); /* Makes NULL goto &_defaultArray */
  blockIndex = _IndexFromBlockPtr(PRArr, Ptr); /* Get the index */
  oldFlags = PRArr->Descriptors[blockIndex].Flags; /* Save current flags */
  if (PNewFlags) /* Caller passed new flags */
    PRArr->Descriptors[blockIndex].Flags = *PNewFlags; /* Modify the flags */

  #ifdef HORATIO_DEBUGHOOKS
  debugHookInfo.Success = 1U;
  debugHookInfo.Misc0 = (unsigned long)oldFlags;
  debugHookInfo.Misc1 = (unsigned long)( (PNewFlags) ? (*PNewFlags) : (oldFlags) );
  horatio_int_CallDebugHook(HORATIO_HOOK_MODIFYDESCFLAGS, &debugHookInfo);
  #endif /*HORATIO_DEBUGHOOKS*/

  return oldFlags; /* Give the old flags back to the caller */
}


