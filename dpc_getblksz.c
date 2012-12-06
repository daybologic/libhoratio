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
  Gets size of a block
*/
#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <stdio.h>
#include <stddef.h>

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_trap.h" /* Trap() - Trap support */
#include "dpc_vptrap.h" /* _VerifyPtrs() */
#include "dpc_iblkptr.h" /* IndexFromBlockPtr() */
#include "dpc_biglock.h" /* Library mutual exclusion */
#include "dpc_isbad.h" /* Internal interface to block testers */
/*-------------------------------------------------------------------------*/
static size_t dpcrtlmm_int_GetBlockSize(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  void DPCRTLMM_FARDATA *BlockPtr
);
/*-------------------------------------------------------------------------*/
size_t dpcrtlmm_GetBlockSize(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  void DPCRTLMM_FARDATA *BlockPtr
)
{
  size_t ret;

  LOCK
  ret = dpcrtlmm_int_GetBlockSize(PBlockArray, BlockPtr);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static size_t dpcrtlmm_int_GetBlockSize(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA *BlockPtr
)
{
  /* First test pointers */
  _VerifyPtrs("GetBlockSize()", PBlockArray, NULL);

  if (dpcrtlmm_int_IsBadBlockPtr(PBlockArray, BlockPtr)) /* Block pointer is invalid? */
  {
    char trapMsg[MAX_TRAP_STRING_LENGTH+1];

    sprintf(
      trapMsg,
      "Attempt to GetBlockSize() unknown block at base %s%p, in array base: %s%p",
      DPCRTLMM_FMTPTRPFX, BlockPtr,
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray
    );
    Trap(DPCRTLMM_TRAP_BAD_BLOCK, trapMsg);
  }
  return _ResolveArrayPtr(PBlockArray)->Descriptors[ _IndexFromBlockPtr(PBlockArray, BlockPtr) ].Size;
}
/*-------------------------------------------------------------------------*/
