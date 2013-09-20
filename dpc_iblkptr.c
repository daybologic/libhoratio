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

/*******************************************************
 * Internal function for getting the index of a block  *
 * within an array, now supports NULL arrays           *
 *******************************************************/
#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>

#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "restricted_horatio.h" /* Main library header */
#include "dpc_intdata.h" /* Internal data */
#include "dpc_trap.h" /* Trap support */
#include "dpc_vptrap.h" /* For _VerifyPtrs() */
#include "dpc_iblkptr.h"

unsigned int dpcrtlmm_int_IndexFromBlockPtr(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *BlockPtr
) {
  unsigned int i;
  PS_HORATIO_BLOCKDESCARRAY PRArr; /* Resolved array pointer */

  _VerifyPtrs("IndexFromBlockPtr()", PBlockArray, NULL);

  PRArr = _ResolveArrayPtr(PBlockArray);
  for ( i = 0U; i < PRArr->Count; i++ ) {
    if ( PRArr->Descriptors[i].PBase == BlockPtr ) /* Found it */
      return i; /* Give index to caller */
  }

  Trap(
    HORATIO_TRAP_INDEX_GEN_FAILED,
    "IndexFromBlockPtr(): Block not found, index not generated"
  );
  return ~0; /* Buggered */
}

