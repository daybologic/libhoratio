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
#############################################################################
# Block array creation and destruction functions                            #
# Normally each module or section of a program will make it's own array     #
# using the functions herin, if it is too much hastle (because of cleaning  #
# up each module) or the program wants to hide behind a normal allocation   #
# function re-routed to us via a hack then only one block array exists per  #
# per program.                                                              #
# 24/11/2001 (DDRP): Attention, block arrays need to start supporting file/ #
# line info soon.
#############################################################################
*/

#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset() */

#ifdef DPCRTLMM_WANTFARDATA
# ifdef HAVE_ALLOC_H
#  include <alloc.h>
# endif /*HAVE_ALLOC_H*/
#endif /*DPCRTLMM_WANTFARDATA*/

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Access to internal data */
#include "dpc_log.h" /* LOG macro */
#include "dpc_trap.h" /* Trap() */
#include "dpc_safelist.h" /* Safety list support functions */
#include "dpc_dbghooks.h" /* For the debug hook executive */
#include "dpc_biglock.h" /* For total library mutual exclusion */
#include "dpc_blkarray.h"
/*-------------------------------------------------------------------------*/
static PS_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_int_CreateBlockArray(void);
static unsigned int dpcrtlmm_int_IsDefaultBlockArray(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray
);
/*-------------------------------------------------------------------------*/
PS_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_CreateBlockArray()
{
  /* Thread safe wrapper for CreateBlockArray() */
  PS_DPCRTLMM_BLOCKDESCARRAY ret;

  LOCK
  ret = dpcrtlmm_int_CreateBlockArray();
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_DestroyBlockArray(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray
)
{
  /* Thread safe wrapper for DestroyBlockArray() */

  LOCK
  dpcrtlmm_int_DestroyBlockArray(PBlockArray);
  UNLOCK
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsDefaultBlockArray(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray
)
{
  /* Thread safe wrapper for IsDefaultBlockArray() */

  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_IsDefaultBlockArray(PBlockArray);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static PS_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_int_CreateBlockArray()
{
  PS_DPCRTLMM_BLOCKDESCARRAY Parray; /* Pointer for caller */
  #ifdef DPCRTLMM_LOG
  char logMsg[MAX_TRAP_STRING_LENGTH+1];
  #endif /*DPCRTLMM_LOG*/
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  /* Init debug hook info */
  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
  debugHookInfo.HookType = DPCRTLMM_HOOK_CREATEBLOCKARRAY;
  /* Ha, this is only vaugely relevant, this will do */
  debugHookInfo.AllocReq = (unsigned int)sizeof(S_DPCRTLMM_BLOCKDESCARRAY);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  /*
    Alloc the array for the caller
  */
  Parray = (S_DPCRTLMM_BLOCKDESCARRAY*)malloc( sizeof(S_DPCRTLMM_BLOCKDESCARRAY) );
  if (!Parray) /* Failed to alloc */
  {
    /* Memory outages while in memory manager mode must be warned about! */
    WARNING("CreateBlockArray(): Couldn\'t allocate the new block array!");
    #ifdef DPCRTLMM_DEBUGHOOKS
    /* PRelArr is nothing, we couldn't allocate one :( */
    /* PRelDesc is nothing, there is no related descriptor */
    debugHookInfo.Success = 0U; /* Ahh, no failure! */
    /* The rest are reserved or not used */
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_CREATEBLOCKARRAY, &debugHookInfo);   /* Call the debug hook executive */
    #endif /*DPCRTLMM_DEBUGHOOKS*/
    return Parray; /* Give the NULL pointer back to the caller */
  }
  Parray->Count = 0U; /* No descriptors in list */
  Parray->Descriptors = NULL; /* Nothing in block list */

  /* The array base must be added to the list of acceptable arrays,
     (the so called safety list)
  */
  if ( !SafetyList_AddBase(Parray) ) /* Add to safety list */
  {
    /* Failed to add to the list?!  Memory outages while in memory manager must be warned about */
    WARNING("CreateBlockArray(): The array base address could not be added to the safety list");
    DPCRTLMM_FREE(Parray); /* Free the array again */
    Parray = NULL; /* So caller sees there's nothing allocated */
  }

  #ifdef DPCRTLMM_LOG
  /* Safe, log progress */
  sprintf(logMsg, "CreateBlockArray() returns base %s%p", DPCRTLMM_FMTPTRPFX, (void*)Parray);
  MESSAGE(__FILE__, __LINE__, logMsg);
  #endif /*DPCRTLMM_LOG*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  /* Set up more hook information to indicate success */
  debugHookInfo.PRelArr = Parray; /* The relavant array in this case is the one allocated */
  debugHookInfo.Success = 1U; /* Yay, success! */
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_CREATEBLOCKARRAY, &debugHookInfo);   /* Call the debug hook executive */
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  return Parray; /* Give new pointer to the caller */
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_DestroyBlockArray( PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray )
{
  /* locals */
  unsigned int sli; /* Safety list loop processing */
  char trapStr[MAX_TRAP_STRING_LENGTH+1]; /* Sometimes used for creating trap strings */
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo; /* Used for calling the debug hook executive */
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  /* Set up common stuff for the debug hook info */
  debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
  /* There is no relavent descriptor */
  debugHookInfo.HookType = DPCRTLMM_HOOK_DESTROYBLOCKARRAY;
  /* There is no allocation request */
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ ) /* For all the possible items in the safety list */
  {
    if (_safetyList[sli]) /* Is this entry used? */
    {
      if (_safetyList[sli] == PBlockArray) /* Pointer match! */
      {
        if (_safetyList[sli]->Count) /* Any descriptors remaining? */
        {
          unsigned long totBytes = 0UL;
          unsigned int li;

          for ( li = 0; li < _safetyList[sli]->Count; li++ ) /* All blocks */
          {
            totBytes += _safetyList[sli]->Descriptors[li].Size; /* Add size of block to total */
          }

          sprintf(
            trapStr,
            "DestroyBlockArray(): %u blocks of memory not freed from array based at %s%p\n                      Total bytes leakage for this array: %lu",
            _safetyList[sli]->Count,
            DPCRTLMM_FMTPTRPFX, (void*)_safetyList[sli],
            totBytes
          );
          Trap(DPCRTLMM_TRAP_UNFREED_BLOCKS, trapStr);
        }
        if (_safetyList[sli]->Descriptors) /* Descriptors not zero? */
        {
          sprintf(
            trapStr,
            "DestroyBlockArray(): Base of raw descriptor array not freed!\n%s%p->%s%p (PBlockArray->Descriptors must be NULL)",
            DPCRTLMM_FMTPTRPFX, (void*)_safetyList[sli],
            DPCRTLMM_FMTPTRPFX, (void*)_safetyList[sli]->Descriptors
          );
          Trap(DPCRTLMM_TRAP_BASENONZERO, trapStr);
        }
        DPCRTLMM_FREE(_safetyList[sli]); /* BUG FIX: Forgot to release the memory for the array block pointer */
        _safetyList[sli] = NULL; /* Remove this array from the safety list */
        #ifdef DPCRTLMM_LOG
        sprintf(
          trapStr,
          "DestroyBlockArray(): The array at base %s%p was destroyed",
          DPCRTLMM_FMTPTRPFX, (void*)PBlockArray
        ); /* Prepare log message */
        MESSAGE(__FILE__, __LINE__, trapStr);
        #endif /*DPCRTLMM_LOG*/

        #ifdef DPCRTLMM_DEBUGHOOKS
        /* Success, call hooks */
        debugHookInfo.Success = 1U; /* Wicked! */
        dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_DESTROYBLOCKARRAY, &debugHookInfo);
        #endif /*DPCRTLMM_DEBUGHOOKS*/
        return; /* Exit to caller */
      }
    }
  }
  /* Entire list processed, array base specified not found */
  #ifdef DPCRTLMM_DEBUGHOOKS
  /* Call hooks */
  debugHookInfo.Success = 0U; /* Failed */
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_DESTROYBLOCKARRAY, &debugHookInfo);
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  /* Fire trap */
  sprintf(
    trapStr,
    "DestroyBlockArray(): Attempt to destroy unknown array (%s%p)!\n",
    DPCRTLMM_FMTPTRPFX, (void*)PBlockArray
  );
  Trap(DPCRTLMM_TRAP_BAD_BLOCK_ARRAY, trapStr);
  return;
}
/*-------------------------------------------------------------------------*/
static unsigned int dpcrtlmm_int_IsDefaultBlockArray( PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray )
{
  #ifdef DPCRTLMM_NONULL_BLOCKDESCARRAY
  return 0; /* Default (NULL) array does not exist */
  #else
  if (!PBlockArray || PBlockArray == &_defaultArray)
    return 1U; /* TRUE */
  return 0U; /* FALSE */
  #endif
}
/*-------------------------------------------------------------------------*/
