/*
    DPCRTLMM Memory Management Library
    Copyright (C) 2000-2002 David Duncan Ross Palmer, Daybo Logic.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


Contact me: Overlord@DayboLogic.co.uk
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
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

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_trap.h" /* _Trap() */
#include "dpc_log.h" /* LOG macro */
#include "dpc_safelist.h" /* Safety list support functions */
#include "dpc_isbad.h" /* Internal interface to isbad checkers */
#include "dpc_vptrap.h"

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

  if ( !dpcrtlmm_int_IsBadArrayPtr(PBlockArray) )
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
  if ( dpcrtlmm_int_IsBadArrayPtr(PBlockArray) ) /* The pointer to the array is invalid */
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
  if ( dpcrtlmm_int_IsBadBlockPtr( PBlockArray, BlockPtr ) ) /* Is bad block pointer? */
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
