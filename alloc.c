/*
    DPCRTLMM Memory management library : Primary allocator
    Copyright (C) 2000 David Duncan Ross Palmer, Daybo Logic.

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
/* Main allocation function and block array grower
 - Overlord David Duncan Ross Palmer
 Copyright (C)2000 OverlordDDRP, Daybo Logic, all rights reserved.
 Overlord@DayboLogic.co.uk
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#ifdef DPCRTLMM_WANTFARDATA
#  include <alloc.h>
#endif /*DPCRTLMM_WANTFARDATA*/
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "log.h" /* Main logging support */
#include "vptrap.h" /* _VerifyPtrs() */
#include "dbghooks.h" /* Debug hook executive and support functions */
#include "biglock.h" /* Mutual exclusion */
#include "alloc.h"
/*-------------------------------------------------------------------------*/
/* Internal functions (local) */

static void OurLog(const unsigned short Severity, const char* Message);

/* Grow the array by 'GrowByElems' elements, returns FALSE if
it fails but then the original array is still valid and no
bigger.  Always make sure the array pointer is resolved, NULL
pointers are not acceptable and will be caught with assert() */

static unsigned int GrowBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PCurrentBlockArray, const unsigned int GrowByElems);

#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

/* Shortcut for typecast */
#define OURLOG(sev, msg) OurLog(((const unsigned short)(sev)), (msg))
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_AllocEx(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  /* Thread safe wrapper for AllocEx() */
  void DPCRTLMM_FARDATA* ret;

  LOCK
  ret = dpcrtlmm_int_AllocEx(PBlockArray, NewBlockSize, File, Line);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_int_AllocEx(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  /* locals */
  void DPCRTLMM_FARDATA* genBlockPtr; /* Generated block pointer */
  char logMsg[MAX_TRAP_STRING_LENGTH + 1];
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray); /* Resolving is done because of a possible NULL */

  _VerifyPtrs("Alloc()", PBlockArray, NULL); /* Haults program if array not valid, third arg is not applicable here */

  sprintf(logMsg, "Program Requested to allocate %u byte block for array 0x%p",
	  NewBlockSize,
	  PBlockArray
	  );
  OURLOG(DPCRTLMM_LOG_MESSAGE, logMsg);

  genBlockPtr = DPCRTLMM_MALLOC(NewBlockSize); /* Allocate block */
  if (!genBlockPtr) /* Out of memory? */
  {
    /* Use buffer for log messages, it's the same size as for traps */
    sprintf(logMsg, "Attempt to allocate block of %u bytes for array at base 0x%p has failed",
	    NewBlockSize,
	    PBlockArray
    );
    OURLOG(DPCRTLMM_LOG_MESSAGE, logMsg); /* I haven't made this a warning because it can happen in a very legitimate situation where the caller may be prepared for a large allocation to handle */
    return NULL; /* No pointer generated */
  }

  /* Now add the block to the array, first grow array */
  if (!GrowBlockArray(PRArr, 1))
  {
    /* Attempt to enlarge the array failed? */
    DPCRTLMM_FREE(genBlockPtr); /* Release the new block of memory */

    sprintf(logMsg, "Attempt to enlarge array at base 0x%p by one element failed",
	    PBlockArray
    );
    /* This could be quite critical, if the memory manager is running our of space */
    OURLOG(DPCRTLMM_LOG_WARNING, logMsg);
    return NULL; /* Give up */
  }

  /* Now the block's address can be added to the array */
  PRArr->Descriptors[PRArr->Count-1].PBase = genBlockPtr; /* Put pointer to base of block in block descriptor in the array */
  PRArr->Descriptors[PRArr->Count-1].Size = NewBlockSize; /* Save size so caller can find it out leter */

  /* Version 1.1.4 changes, source file/line records */
  PRArr->Descriptors[PRArr->Count-1].SourceLine = Line;
  if ( File ) {
    PRArr->Descriptors[PRArr->Count-1].SourceFile = (char*)malloc((strlen(File)+1)*sizeof(char));
    if ( PRArr->Descriptors[PRArr->Count-1].SourceFile )
      strcpy(PRArr->Descriptors[PRArr->Count-1].SourceFile, File);
  }

  /* Update library statistics */
  dpcrtlmm_int__blockCount++;
  dpcrtlmm_int__allocCharge += NewBlockSize;
  /* Update peaks */
  if ( dpcrtlmm_int__blockCount > dpcrtlmm_int__blockCountPeak )
    dpcrtlmm_int__blockCountPeak = dpcrtlmm_int__blockCount;
  if ( dpcrtlmm_int__allocCharge > dpcrtlmm_int__allocPeak )
    dpcrtlmm_int__allocPeak = dpcrtlmm_int__allocCharge;

  sprintf(logMsg, "New block of %u bytes is at 0x%p, descarray (0x%p) was extended to hold a new descriptor\nAlloc() returns 0x%p\nLeaving...",
	  NewBlockSize,
	  genBlockPtr,
	  PBlockArray,
	  genBlockPtr
  );
  OURLOG(DPCRTLMM_LOG_MESSAGE, logMsg); /* Actually this is a bit of a library debugging commend, perhaps verbose, I could remove it */

  /* Call the debug hook executive */
  #ifdef DPCRTLMM_DEBUGHOOKS
  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO)); /* Init structure */
  debugHookInfo.PRelArr = PRArr; /* Use resolved value, NULL means N/A, this is the only point at which the array pointer address is exposed to the caller directly (expect other debug hook calls of course) */
  debugHookInfo.PRelDesc = genBlockPtr;
  debugHookInfo.HookType = DPCRTLMM_HOOK_ALLOC;
  debugHookInfo.AllocReq = (unsigned int)NewBlockSize;
  debugHookInfo.Success = 1U; /* TRUE */
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_ALLOC, &debugHookInfo);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  return genBlockPtr; /* Give pointer to the caller */
}
/*-------------------------------------------------------------------------*/
static unsigned int GrowBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PCurrentBlockArray, const unsigned int GrowByElems)
{
  PS_DPCRTLMM_BLOCKDESCRIPTOR ptr; /* Pointer to block descriptors during enlargement */
  unsigned int oldCount; /* Count before enlargement */
  unsigned int initi; /* Initialization interator */

  #ifdef NDEBUG /* Not in debug mode? */
    if (!PCurrentBlockArray) return 0U; /* Just get out indicating error before disaster */
  #else /* Debug mode */
    assert(PCurrentBlockArray);
  #endif /*NDEBUG*/

  if (!GrowByElems) /* Want to grow by nothing? */
  {
    OURLOG(DPCRTLMM_LOG_WARNING, "Attempt to GrowBlockArray() by no items, ignored");
    return 1U; /* Success, already this size, it's great when there's nothing to do isn't it, programmer's are lazy */
  }

  oldCount = PCurrentBlockArray->Count; /* Take count before we grow array */
  ptr = DPCRTLMM_REALLOC( PCurrentBlockArray->Descriptors, (oldCount + GrowByElems) * sizeof(S_DPCRTLMM_BLOCKDESCRIPTOR) ); /* Grow array */
  if (!ptr) /* Couldn't grow? */
    return 0U; /* Fail */

  /* Update array information */
  PCurrentBlockArray->Count += GrowByElems;
  PCurrentBlockArray->Descriptors = ptr; /* Possible relocation might mean the pointer to the descriptors may need updating, make it so */

  for ( initi = oldCount; initi < PCurrentBlockArray->Count; initi++ ) /* All new descriptors in the array that we just created */
  {
    PCurrentBlockArray->Descriptors[initi].PBase = NULL; /* No block assigned to this new descriptor yet */
    PCurrentBlockArray->Descriptors[initi].Size = (size_t)0U; /* Therefore no size either */
    PCurrentBlockArray->Descriptors[initi].Flags = 0U; /* No flags set */
    PCurrentBlockArray->Descriptors[initi].SourceLine = 0U; /* No source line allocation */
    PCurrentBlockArray->Descriptors[initi].SourceFile = NULL; /* No known source file */
  }
  return 1U; /* Success */
}
/*-------------------------------------------------------------------------*/
static void OurLog(const unsigned short Severity, const char* Str)
{
   /* Our job is to add "Alloc() to the start of the string, saves data space
  if everybody in this module calls this instead of _Log() directly.
  We can't use LOG() twice because the information will be put on different
  lines so a copy is needed. */

  if (Str && Str[0]) /* Valid string of at least on character sent to us? */
  {
    char* PcopyStr;
    const char FuncName[] = "Alloc(): "; /* Prefix */

    PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) ); /* Allocate space for copy, note that NULL termination is automatic because using sizeof() */
    if (PcopyStr)
    {
      strcpy(PcopyStr, FuncName); /* Prepend prefix */
      strcat(PcopyStr, Str); /* Add log string after the prefix */

      dpcrtlmm_int_Log(Severity, PcopyStr); /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}
/*-------------------------------------------------------------------------*/
