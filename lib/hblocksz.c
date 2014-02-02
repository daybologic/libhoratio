/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2014, David Duncan Ross Palmer, Daybo Logic
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
  Gets size of a block
*/
#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <stdio.h>
#include <stddef.h>

#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library data */
#include "htrap.h" /* Trap() - Trap support */
#include "hvptrap.h" /* _VerifyPtrs() */
#include "hiblkptr.h" /* IndexFromBlockPtr() */
#include "hbiglock.h" /* Library mutual exclusion */
#include "hisbad.h" /* Internal interface to block testers */

static size_t horatio_int_GetBlockSize(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *const BlockPtr
);

size_t horatio_GetBlockSize(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *const BlockPtr
) {
  size_t ret;

  LOCK
  ret = horatio_int_GetBlockSize(PBlockArray, BlockPtr);
  UNLOCK

  return ret;
}

static size_t horatio_int_GetBlockSize(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *const BlockPtr
) {
  /* First test pointers */
  _VerifyPtrs("GetBlockSize()", PBlockArray, NULL);

  if (horatio_int_IsBadBlockPtr(PBlockArray, BlockPtr)) { /* Block pointer is invalid? */
    char trapMsg[MAX_TRAP_STRING_LENGTH+1];

    sprintf(
      trapMsg,
      #ifdef HAVE_SNPRINTF
      MAX_TRAP_STRING_LENGTH,
      #endif /*HAVE_SNPRINTF*/
      "Attempt to GetBlockSize() unknown block at base %s%p, in array base: %s%p",
      HORATIO_FMTPTRPFX, BlockPtr,
      HORATIO_FMTPTRPFX, (void*)PBlockArray
    );
    Trap(HORATIO_TRAP_BAD_BLOCK, trapMsg);
  }
  return _ResolveArrayPtr(PBlockArray)->Descriptors[ _IndexFromBlockPtr(PBlockArray, BlockPtr) ].Size;
}

