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
#define HORATIO_SOURCE

/*
  Main allocation function and block array grower
*/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef HORATIO_WANTFARDATA
# ifdef HAVE_ALLOC_H
#  include <alloc.h>
# endif /*HAVE_ALLOC_H*/
#endif /*HORATIO_WANTFARDATA*/

#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "restricted_horatio.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_log.h" /* Main logging support */
#include "dpc_vptrap.h" /* _VerifyPtrs() */
#include "dpc_dbghooks.h" /* Debug hook executive and support functions */
#include "dpc_biglock.h" /* Mutual exclusion */
#include "dpc_alloc.h"

/* Internal functions (local) */

static void OurLog(
  const char * File,
  const unsigned int Line,
  const unsigned short Severity,
  const char *Message
);

/*
  Grow the array by 'GrowByElems' elements, returns FALSE if
  it fails but then the original array is still valid and no
  bigger.  Always make sure the array pointer is resolved, NULL
  pointers are not acceptable and will be caught with assert()
*/

static unsigned int GrowBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PCurrentBlockArray,
  const unsigned int GrowByElems
);

#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

/* Do the same paranoid check for OURLOG_POS */
#ifdef OURLOG_POS
#  undef OURLOG_POS
#endif /*OURLOG_POS*/

/* Shortcut for typecast */
#define OURLOG(f, l, sev, msg) \
  OurLog((f), (l), ((const unsigned short)(sev)), (msg))

#define OURLOG_POS(sev, msg) \
  OURLOG(__FILE__, __LINE__, (sev), (msg))

void HORATIO_FARDATA* horatio_AllocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
) {
  /* Thread safe wrapper for AllocEx() */
  void HORATIO_FARDATA* ret;

  LOCK
  ret = horatio_int_AllocEx(PBlockArray, NewBlockSize, File, Line);
  UNLOCK

  return ret;
}

void HORATIO_FARDATA* horatio_int_AllocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
) {
  /* locals */
  void HORATIO_FARDATA* genBlockPtr; /* Generated block pointer */
  char logMsg[MAX_TRAP_STRING_LENGTH + 1];
  #ifdef HORATIO_DEBUGHOOKS
  S_HORATIO_DEBUGHOOKINFO debugHookInfo;
  #endif /*HORATIO_DEBUGHOOKS*/
  #ifdef HAVE_SNPRINTF
  size_t logMsgRemaining = MAX_TRAP_STRING_LENGTH;
  #endif /*HAVE_SNPRINTF*/
  PS_HORATIO_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray); /* Resolving is done because of a possible NULL */

  _VerifyPtrs("Alloc()", PBlockArray, NULL); /* Haults program if array not valid, third arg is not applicable here */

  sprintf(
    logMsg,
    #ifdef HAVE_SNPRINTF
    logMsgRemaining,
    #endif /*HAVE_SNPRINTF*/
    "Program Requested to allocate %u byte block for array %s%p",
    (unsigned int)NewBlockSize,
    HORATIO_FMTPTRPFX, (void*)PBlockArray
  );
  #ifdef HAVE_SNPRINTF
  logMsgRemaining -= strlen(logMsg);
  #endif /*HAVE_SNPRINTF*/
  OURLOG(File, Line, HORATIO_LOG_MESSAGE, logMsg);

  genBlockPtr = HORATIO_MALLOC(NewBlockSize); /* Allocate block */
  if (!genBlockPtr) { /* Out of memory? */
    /* Use buffer for log messages, it's the same size as for traps */
    sprintf(
      logMsg,
      #ifdef HAVE_SNPRINTF
      logMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "Attempt to allocate block of %u bytes for array at base %s%p has failed",
      (unsigned int)NewBlockSize,
      HORATIO_FMTPTRPFX, (void*)PBlockArray
    );
    #ifdef HAVE_SNPRINTF
    logMsgRemaining -= strlen(logMsg);
    #endif /*HAVE_SNPRINTF*/
    OURLOG(File, Line, HORATIO_LOG_MESSAGE, logMsg); /* I haven't made this a warning because it can happen in a very legitimate situation where the caller may be prepared for a large allocation to handle */
    return NULL; /* No pointer generated */
  }

  /* Now add the block to the array, first grow array */
  if (!GrowBlockArray(PRArr, 1)) {
    /* Attempt to enlarge the array failed? */
    HORATIO_FREE(genBlockPtr); /* Release the new block of memory */

    sprintf(
      logMsg,
      #ifdef HAVE_SNPRINTF
      logMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "Attempt to enlarge array at base %s%p by one element failed",
      HORATIO_FMTPTRPFX, (void*)PBlockArray
    );
    #ifdef HAVE_SNPRINTF
    logMsgRemaining -= strlen(logMsg);
    #endif /*HAVE_SNPRINTF*/
    /* This could be quite critical, if the memory manager is running our of space */
    OURLOG_POS(HORATIO_LOG_WARNING, logMsg);
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
  horatio_int__blockCount++;
  horatio_int__allocCharge += NewBlockSize;
  /* Update peaks */
  if ( horatio_int__blockCount > horatio_int__blockCountPeak )
    horatio_int__blockCountPeak = horatio_int__blockCount;
  if ( horatio_int__allocCharge > horatio_int__allocPeak )
    horatio_int__allocPeak = horatio_int__allocCharge;

  /* Call the debug hook executive */
  #ifdef HORATIO_DEBUGHOOKS
  memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO)); /* Init structure */
  debugHookInfo.PRelArr = PRArr; /* Use resolved value, NULL means N/A, this is the only point at which the array pointer address is exposed to the caller directly (expect other debug hook calls of course) */
  debugHookInfo.PRelDesc = genBlockPtr;
  debugHookInfo.HookType = HORATIO_HOOK_ALLOC;
  debugHookInfo.AllocReq = (unsigned int)NewBlockSize;
  debugHookInfo.Success = 1U; /* TRUE */
  horatio_int_CallDebugHook(HORATIO_HOOK_ALLOC, &debugHookInfo);
  #endif /*HORATIO_DEBUGHOOKS*/

  return genBlockPtr; /* Give pointer to the caller */
}

static unsigned int GrowBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PCurrentBlockArray,
  const unsigned int GrowByElems
) {
  PS_HORATIO_BLOCKDESCRIPTOR ptr; /* Pointer to block descriptors during enlargement */
  unsigned int oldCount; /* Count before enlargement */
  unsigned int initi; /* Initialization interator */

  #ifdef NDEBUG /* Not in debug mode? */
    if (!PCurrentBlockArray) return 0U; /* Just get out indicating error before disaster */
  #else /* Debug mode */
    assert(PCurrentBlockArray);
  #endif /*NDEBUG*/

  if (!GrowByElems) { /* Want to grow by nothing? */
    OURLOG_POS(HORATIO_LOG_WARNING, "Attempt to GrowBlockArray() by no items, ignored");
    return 1U; /* Success, already this size, it's great when there's nothing to do isn't it, programmer's are lazy */
  }

  oldCount = PCurrentBlockArray->Count; /* Take count before we grow array */
  ptr = HORATIO_REALLOC( PCurrentBlockArray->Descriptors, (oldCount + GrowByElems) * sizeof(S_HORATIO_BLOCKDESCRIPTOR) ); /* Grow array */
  if (!ptr) /* Couldn't grow? */
    return 0U; /* Fail */

  /* Update array information */
  PCurrentBlockArray->Count += GrowByElems;
  PCurrentBlockArray->Descriptors = ptr; /* Possible relocation might mean the pointer to the descriptors may need updating, make it so */

  for ( initi = oldCount; initi < PCurrentBlockArray->Count; initi++ ) { /* All new descriptors in the array that we just created */
    PCurrentBlockArray->Descriptors[initi].PBase = NULL; /* No block assigned to this new descriptor yet */
    PCurrentBlockArray->Descriptors[initi].Size = (size_t)0U; /* Therefore no size either */
    PCurrentBlockArray->Descriptors[initi].Flags = 0U; /* No flags set */
    PCurrentBlockArray->Descriptors[initi].SourceLine = 0U; /* No source line allocation */
    PCurrentBlockArray->Descriptors[initi].SourceFile = NULL; /* No known source file */
  }
  return 1U; /* Success */
}

static void OurLog(
  const char *File,
  const unsigned int Line,
  const unsigned short Severity,
  const char *Str
) {
   /* Our job is to add "Alloc() to the start of the string, saves data space
  if everybody in this module calls this instead of _Log() directly.
  We can't use LOG() twice because the information will be put on different
  lines so a copy is needed. */

  if (Str && Str[0]) { /* Valid string of at least on character sent to us? */
    char* PcopyStr;
    const char FuncName[] = "Alloc(): "; /* Prefix */

    PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) ); /* Allocate space for copy, note that NULL termination is automatic because using sizeof() */
    if (PcopyStr) {
      strcpy(PcopyStr, FuncName); /* Prepend prefix */
      strcat(PcopyStr, Str); /* Add log string after the prefix */

      horatio_int_Log(File, Line, Severity, PcopyStr); /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}

