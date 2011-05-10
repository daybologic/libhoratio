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
/*
#############################################################################
# Block array creation and destruction functions                            #
# Normally each module or section of a program will make it's own array     #
# using the functions herin, if it is too much hastle (because of cleaning  #
# up each module) or the program wants to hide behind a normal allocation   #
# function re-routed to us via a hack then only one block array exists per  #
# per program.                                                              #
#              DPCRTLMM (C)Copyright 2000, OverlordDDRP, Daybo Logic        #
#              All rights reserved.  Overlord@DayboLogic.co.uk              #
#############################################################################
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset() */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#ifdef DPCRTLMM_WANTFARDATA
#  include <alloc.h>
#endif /*DPCRTLMM_WANTFARDATA*/
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Access to internal data */
#include "log.h" /* LOG macro */
#include "trap.h" /* _Trap() */
#include "safelist.h" /* Safety list support functions */
#include "dbghooks.h" /* For the debug hook executive */
/*-------------------------------------------------------------------------*/
PS_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_CreateBlockArray()
{
  PS_DPCRTLMM_BLOCKDESCARRAY Parray; /* Pointer for caller */
  #ifdef DPCRTLMM_LOG
  char logMsg[MAX_TRAP_STRING_LENGTH+1];
  #endif /*DPCRTLMM_LOG*/
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO)); /* Init debug hook info */
  debugHookInfo.HookType = DPCRTLMM_HOOK_CREATEBLOCKARRAY;
  debugHookInfo.AllocReq = (unsigned int)sizeof(S_DPCRTLMM_BLOCKDESCARRAY); /* Ha, this is only vaugly relavant, this will do */
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  LOG("Program called CreateBlockArray()")
  Parray = (S_DPCRTLMM_BLOCKDESCARRAY*)malloc( sizeof(S_DPCRTLMM_BLOCKDESCARRAY) ); /* Alloc the array for the caller */
  if (!Parray) /* Failed to alloc */
  {
    LOG("CreateBlockArray(): Couldn\'t allocate the new block array!")
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
    /* Failed to add to the list?! */
    LOG("CreateBlockArray(): The array base address could not be added to the safety list")
    DPCRTLMM_FREE(Parray); /* Free the array again */
    Parray = NULL; /* So caller sees there's nothing allocated */
  }

  /* Safe, log progress */
  #ifdef DPCRTLMM_LOG
  sprintf(logMsg, "CreateBlockArray() returns base 0x%p", Parray);
  #endif /*DPCRTLMM_LOG*/
  LOG(logMsg)

  #ifdef DPCRTLMM_DEBUGHOOKS
  /* Set up more hook information to indicate success */
  debugHookInfo.PRelArr = Parray; /* The relavant array in this case is the one allocated */
  debugHookInfo.Success = 1U; /* Yay, success! */
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_CREATEBLOCKARRAY, &debugHookInfo);   /* Call the debug hook executive */
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  return Parray; /* Give new pointer to the caller */
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_DestroyBlockArray( PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray )
{
  /* locals */
  unsigned int sli; /* Safety list loop processing */
  char trapStr[MAX_TRAP_STRING_LENGTH+1]; /* Sometimes used for creating trap strings */
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo; /* Used for calling the debug hook executive */
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  #ifdef DPCRTLMM_LOG
  sprintf(trapStr, "Program called DestroyBlockArray(0x%p)", PBlockArray);
  #endif /*DPCRTLMM_LOG*/
  LOG(trapStr)

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

          sprintf(trapStr, "DestroyBlockArray(): %u blocks of memory not freed from array based at 0x%p\n                      Total bytes leakage for this array: %lu",
                  _safetyList[sli]->Count,
                  _safetyList[sli],
                  totBytes
          );
          _Trap(DPCRTLMM_TRAP_UNFREED_BLOCKS, trapStr);
        }
        if (_safetyList[sli]->Descriptors) /* Descriptors not zero? */
        {
          sprintf(trapStr, "DestroyBlockArray(): Base of raw descriptor array not freed!\n0x%p->0x%p (PBlockArray->Descriptors must be NULL)",
                  _safetyList[sli],
                  _safetyList[sli]->Descriptors
          );
          _Trap(DPCRTLMM_TRAP_BASENONZERO, trapStr);
        }
        DPCRTLMM_FREE(_safetyList[sli]); /* BUG FIX: Forgot to release the memory for the array block pointer */
        _safetyList[sli] = NULL; /* Remove this array from the safety list */
        #ifdef DPCRTLMM_LOG
        sprintf(trapStr, "DestroyBlockArray(): The array at base 0x%p was destroyed", PBlockArray); /* Prepare log message */
        #endif /*DPCRTLMM_LOG*/
        LOG(trapStr) /* Pass to logger */

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
  sprintf(trapStr, "DestroyBlockArray(): Attempt to destroy unknown array (0x%p)!",
          PBlockArray
  );
  _Trap(DPCRTLMM_TRAP_BAD_BLOCK_ARRAY, trapStr);
  return;
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsDefaultBlockArray( PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray )
{
  if (!PBlockArray || PBlockArray == &_defaultArray)
    return 1U; /* TRUE */
  return 0U; /* FALSE */
}
/*-------------------------------------------------------------------------*/
