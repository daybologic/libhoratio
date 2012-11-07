/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer, Daybo Logic
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
#############################################################################
# IsBad....() routines for testing validity of pointers, just a small part  #
# of one of my favourite libraries DPCRTLMM, heh heh.                       #
#############################################################################

  NOTE: 28th July 2000
  Both function can now cope with resolved or unresolved array pointers
*/
#define DPCRTLMM_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Access to internal data */
#include "dpc_trap.h" /* Trap support */
#include "dpc_safelist.h" /* List of acceptable arrays */
#include "dpc_biglock.h" /* Mutual exclusion */
#include "dpc_isbad.h"
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsBadBlockPtr(
  const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const void DPCRTLMM_FARDATA *BlockPtr
)
{
  /* Thread safe wrapper for IsBadBlockPtr() */
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_IsBadBlockPtr(PBlockArray, BlockPtr);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsBadArrayPtr(
  const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray
)
{
  /* Thread safe wrapper for IsBadArrayPtr() */
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_IsBadArrayPtr(PBlockArray);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_int_IsBadBlockPtr(
  const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const void DPCRTLMM_FARDATA *BlockPtr
)
{
  /* locals */
  unsigned int i; /* List/loop control */
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray);

  /* Test for bad block array */
  if ( dpcrtlmm_int_IsBadArrayPtr(PBlockArray) ) /* Block array bad? */
  {
    /* Fire a trap */
    char trapmsg[MAX_TRAP_STRING_LENGTH + sizeof(char)];

    sprintf(
      trapmsg,
      "The block array address %s%p is unknown, unable to search for blocks.",
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray
    );

    Trap(DPCRTLMM_TRAP_BAD_BLOCK_ARRAY, trapmsg);
  }

  for ( i = 0U; i < PRArr->Count; i++ ) /* For all the block descriptors in the list */
  {
    if (BlockPtr == PRArr->Descriptors[i].PBase) /* Block base match */
      return 0U; /* Block is not bad */
  }
  /* The block was not matched in the loop above, therefore the block
  is bad or not in the specified array. */
  return 1U; /* Yes, block is bad */
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_int_IsBadArrayPtr(
  const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray
)
{
  /* I've rewritten this function so that is can support NULL arrays
  and it does not matter whether PBlockArray is resolved or not */

  #ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY /* Build supports NULL arrays? */
  if ( !PBlockArray || PBlockArray == &_defaultArray ) /* NULL array (or resolved version) */
    return 0U; /* Not a bad array */
  #endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/

  /* Check the normal safety list */
  return !SafetyList_IsArrayPtrPresent(PBlockArray);
}
/*-------------------------------------------------------------------------*/
