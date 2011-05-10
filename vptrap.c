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
/* 19th July 2000: This module replaces bbatrap.c and bbptrap.c, which no
longer exist.  It's an internal library issue, prebuilt traps for invalid
pointers.  In version 1.0 through all the betas these two finctions which
were always called at the same time where in two separate modules.
I decided it would be cleaner to move them into the same module.
If there is no block pointer in the array to check, pass NULL as arg three

Creation: 19th July 2000
Last mod: 28th July 2000
*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "trap.h" /* _Trap() */
#include "log.h" /* LOG macro */
#include "safelist.h" /* Safety list support functions */
#include "vptrap.h"

/* Functions only we can see */
static void TrapOnBadBlockArray(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray);
static void TrapOnBadBlockPtr(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr);
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_VerifyPtrs(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr)
{
  /* CAUTION: Do not pass a resolved array pointer to this function */
  TrapOnBadBlockArray(FuncName, PBlockArray); /* First trap invalid array pointers */
  if (BlockPtr)
    TrapOnBadBlockPtr(FuncName, PBlockArray, BlockPtr); /* Then trap invalid block pointers within the array */
}
/*-------------------------------------------------------------------------*/
static void TrapOnBadBlockArray(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray)
{
  /* locals */
  const char* cTrapMsg0 = ": The array base ptr 0x%p was not found in the internal safety list";
  char* dynMsg; /* Dynamically allocated message string */

  if ( !dpcrtlmm_IsBadArrayPtr(PBlockArray) )
    return;

  /* The array base is bad */
  dynMsg = (char*)malloc( ((FuncName) ? (strlen(FuncName)) : (0)) + strlen(cTrapMsg0) + 32 /* Enough space for address and NULL (and more) */ );
  if (dynMsg)
  {
    if (FuncName)
      strcpy(dynMsg, FuncName);
    sprintf((FuncName) ? (dynMsg + strlen(FuncName)) : (dynMsg), cTrapMsg0, PBlockArray);
    _Trap(DPCRTLMM_TRAP_BAD_BLOCK_ARRAY, dynMsg);
  }
  else /* malloc() failed */
  {
    if ( dpcrtlmm__EnableTraps ) abort();
  }
}
/*-------------------------------------------------------------------------*/
static void TrapOnBadBlockPtr(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr)
{
  if ( dpcrtlmm_IsBadArrayPtr(PBlockArray) ) /* The pointer to the array is invalid */
  {
    char trapMsg[MAX_TRAP_STRING_LENGTH+1]; /* For trap message */
    char blankStr[] = "";
    char* PusedFuncName; /* Points to function name to use (not dynamic) */

    if (FuncName) /* Function name sent? */
      PusedFuncName = (char*)FuncName; /* Use sent funtion name (doconst (watch out)) */
    else /* Funtion name not sent? */
      PusedFuncName = blankStr; /* Point to blank string */

    sprintf(trapMsg, "%s: The block pointer 0x%p is not valid for array 0x%p, cannot test block pointer validity.",
	    PusedFuncName,
	    BlockPtr,
	    PBlockArray
    );

    _Trap(DPCRTLMM_TRAP_BAD_BLOCK, trapMsg);
  } /* Bad block array? */

  /* The array is a valid and acceptable pointer, pass on to IsBadBlockPtr()
  and if it's bad fire a trap. */
  if ( dpcrtlmm_IsBadBlockPtr( PBlockArray, BlockPtr ) ) /* Is bad block pointer? */
  {
    char trapMsg[128]; /* Space for trap message */
    char* PusedFuncName;
    char blankStr[] = "";

    if (FuncName)
      PusedFuncName = (char*)FuncName; /* Do-consting, have a caution now! */
    else
      PusedFuncName = blankStr;

    sprintf("%s: Bad block pointer: 0x%p for array 0x%p",
	    PusedFuncName,
	    BlockPtr,
	    PBlockArray
    );
    _Trap(DPCRTLMM_TRAP_BAD_BLOCK, trapMsg); /* Fire trap */
  }
  return;
}
/*-------------------------------------------------------------------------*/
