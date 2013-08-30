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

#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef DPCRTLMM_WANTFARDATA
# ifdef HAVE_ALLOC_H
#  include <alloc.c>
# endif /*HAVE_ALLOC_H*/
#endif /*DPCRTLMM_WANTFARDATA*/

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal data */
#include "dpc_vptrap.h" /* _VerifyPtrs() */
#include "dpc_locktrap.h" /* _LockTrap() */
#include "dpc_log.h" /* LOG macro */
#include "dpc_trap.h" /* Trap() */
#include "dpc_dbghooks.h" /* Debug hook executive */
#include "dpc_biglock.h" /* For mutual exclusion */
#include "dpc_isbad.h" /* Internal interface to block testers */

#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

#define OURLOG(f, l, sev, msg) \
  OurLog((f), (l), ((const unsigned short)(sev)), (msg))
#define OURLOG_POS(sev, msg) \
  OURLOG(__FILE__, __LINE__, (sev), (msg))

/* Function under the locked version */
static void dpcrtlmm_int_Free(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  void DPCRTLMM_FARDATA *Ptr
);

/*
  Always make sure to pass resolved arrays to these functions:-
  Moveup following blocks descriptors in array to remove blank
  space.  StartPos is the item just deleted when moveup will be
  started from
*/
static void Moveup(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int StartPos
);
/*
  Shrink array, trap is fired on an attempt to shrink more
  than the current size.
*/
static void ShrinkBlockArray(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int Amount
);
static void OurLog(
  const char *File,
  const unsigned int Line,
  const unsigned short Severity,
  const char *Msg
);

void dpcrtlmm_Free(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  void DPCRTLMM_FARDATA *Ptr
) {
  /* Thread safe wrapper around Free() */

  LOCK
  dpcrtlmm_int_Free(PBlockArray, Ptr);
  UNLOCK
}

static void dpcrtlmm_int_Free(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  void DPCRTLMM_FARDATA *Ptr
) {
  /* locals */
  unsigned int i; /* For the finder loop */
  char trapMsg[MAX_TRAP_STRING_LENGTH + sizeof(char)];
  #ifdef HAVE_SNPRINTF
  size_t trapMsgRemaining = MAX_TRAP_STRING_LENGTH;
  #endif /*HAVE_SNPRINTF*/
  const char funcName[] = "Free()"; /* Our function name */
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr; /* Resolved array pointer */
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugTrapInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  PRArr = _ResolveArrayPtr(PBlockArray); /* Resolve incase block array is NULL */
  _VerifyPtrs(funcName, PBlockArray, NULL); /* Don't check if bad block in this trap, use own trap... */
  if ( dpcrtlmm_int_IsBadBlockPtr(PBlockArray, Ptr) ) { /* Block pointer not valid? */
    sprintf(
      trapMsg,
      #ifdef HAVE_SNPRINTF
      trapMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "Free(): Attempt to release memory we don\'t own or memory which has already been released, array: %s%p, block %s%p",
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray,
      DPCRTLMM_FMTPTRPFX, Ptr
    );
    #ifdef HAVE_SNPRINTF
    trapMsgRemaining -= strlen(trapMsg);
    #endif /*HAVE_SNPRINTF*/
    Trap(DPCRTLMM_TRAP_UNOWNED_FREE, trapMsg);
  }

  if (_LockTrap(funcName, PBlockArray, Ptr)) /* Do trap if block is locked */
    return;

  /* Find the block's descriptor using the block base address, have a caution to use the resolved array pointer */
  for ( i = 0U; i < PRArr->Count; i++ ) {
    if ( PRArr->Descriptors[i].PBase == Ptr ) { /* This is the one */
      #ifdef DPCRTLMM_LOG
      sprintf(
        trapMsg,
        #ifdef HAVE_SNPRINTF
        trapMsgRemaining,
        #endif /*HAVE_SNPRINTF*/
        "Freeing block %s%p from array %s%p",
        DPCRTLMM_FMTPTRPFX, PRArr->Descriptors[i].PBase,
        DPCRTLMM_FMTPTRPFX, (void*)PRArr
      );
      #ifdef HAVE_SNPRINTF
      trapMsgRemaining -= strlen(trapMsg);
      #endif /*HAVE_SNPRINTF*/
      OURLOG(PRArr->Descriptors[i].SourceFile, PRArr->Descriptors[i].SourceLine, DPCRTLMM_LOG_MESSAGE, trapMsg);
      #endif /*DPCRTLMM_LOG*/

      DPCRTLMM_FREE( PRArr->Descriptors[i].PBase ); /* Free the block */
      if ( PRArr->Descriptors[i].SourceFile ) /* We know the file which allocated this */
        free(PRArr->Descriptors[i].SourceFile); /* Now we don't! */

      /* Update library stats */
      dpcrtlmm_int__blockCount--;
      dpcrtlmm_int__allocCharge -= PRArr->Descriptors[i].Size;

      if (PRArr->Count >= 2) /* These are other items in the array? */
        Moveup(PRArr, i); /* Moveup following blocks descriptors in array to remove blank space */
      ShrinkBlockArray(PRArr, 1); /* Shrink size of array of descriptors (deleting redundant end item (which has been shifted up))*/

      /* Call the debug hoop executive */
      #ifdef DPCRTLMM_DEBUGHOOKS
      memset(&debugTrapInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
      debugTrapInfo.PRelArr = PRArr;
      debugTrapInfo.PRelDesc = &PRArr->Descriptors[i];
      debugTrapInfo.HookType = DPCRTLMM_HOOK_FREE;
      /* AlloqReq is not applicable */
      debugTrapInfo.Success = 1U; /* Yup, success! */
      /* The rest are currently reserved. */
      dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_FREE, &debugTrapInfo);
      #endif /*DPCRTLMM_DEBUGHOOKS*/

      break; /* Don't look at anymore blocks */
    }
  }
  return;
}

static void Moveup(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int StartPos
) {
  /* locals */
  unsigned int i; /* Loop control */

  if ( PBlockArray->Count < 2) { /* Only one or no items, can't do a moveup */
    /* Do trap */
    Trap(DPCRTLMM_TRAP_BAD_RANGE_MOVEUP, "Free()/Moveup: Can\'t move up one item or no items.\n");
    return;
  }
  if ( StartPos >= PBlockArray->Count ) { /* StartPos out of range? */
    /* Do trap */
    char trapMsg[MAX_TRAP_STRING_LENGTH+1]; /* Space for trap message */
    #ifdef HAVE_SNPRINTF
    size_t trapMsgRemaining = MAX_TRAP_STRING_LENGTH;
    #endif /*HAVE_SNPRINTF*/

    sprintf(
      trapMsg,
      #ifdef HAVE_SNPRINTF
      trapMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "Free()/Moveup: StartPos is not valid. StartPos=%u, %s%p->Count=%u",
      StartPos,
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray,
      PBlockArray->Count
    );
    #ifdef HAVE_SNPRINTF
    trapMsgRemaining -= strlen(trapMsg);
    #endif /*HAVE_SNPRINTF*/
    Trap(DPCRTLMM_TRAP_BAD_RANGE_MOVEUP, trapMsg);
    return;
  }

  /* Moving elements left to fill a gap */
  for ( i = StartPos+1; i < PBlockArray->Count; i++ ) {
    S_DPCRTLMM_BLOCKDESCRIPTOR blockDesc;

    blockDesc = PBlockArray->Descriptors[i];
    PBlockArray->Descriptors[i-1] = blockDesc;
  }
  return;
}

static void ShrinkBlockArray(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int Amount
) {
  char logMsg[MAX_TRAP_STRING_LENGTH +1];
  #ifdef HAVE_SNPRINTF
  size_t logMsgRemaining = MAX_TRAP_STRING_LENGTH;
  #endif /*HAVE_SNPRINTF*/

  _VerifyPtrs("ShrinkBlockArray()", PBlockArray, NULL); /* Ensure array is valid */
  if (!Amount) {
    sprintf(
      logMsg,
      #ifdef HAVE_SNPRINTF
      logMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "Attempt to ShrinkBlockArray(%s%p) by nothing, ignored (internal DPCRTLMM error)",
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray
    );
    #ifdef HAVE_SNPRINTF
    logMsgRemaining -= strlen(logMsg);
    #endif /*HAVE_SNPRINTF*/
    OURLOG_POS(DPCRTLMM_LOG_WARNING, logMsg);
    return;
  }
  if (!PBlockArray->Count) {
    sprintf(
      logMsg,
      #ifdef HAVE_SNPRINTF
      logMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "ShrinkBlockArray(): %s%p->Count=0U, can\'t shrink the array any more!",
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray
    );
    #ifdef HAVE_SNPRINTF
    logMsgRemaining -= strlen(logMsg);
    #endif /*HAVE_SNPRINTF*/
    Trap(DPCRTLMM_TRAP_SHRINKARR_WHILE_NOWT, logMsg);
    return;
  }
  if (Amount > PBlockArray->Count) { /* Shrink further than size?! */
    sprintf(
      logMsg,
      #ifdef HAVE_SNPRINTF
      logMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "ShrinkBlockArray(): Amount=%u, greater than original size in elements (%s%p->Count=%u)",
      Amount,
      DPCRTLMM_FMTPTRPFX, (void*)PBlockArray,
      PBlockArray->Count
    );
    #ifdef HAVE_SNPRINTF
    logMsgRemaining -= strlen(logMsg);
    #endif /*HAVE_SNPRINTF*/
    Trap(DPCRTLMM_TRAP_SHRINKARR_TOOMUCH, logMsg);
    return;
  }

  /* Reducing to zero? */
  if ( !(PBlockArray->Count - Amount) ) {
    DPCRTLMM_FREE(PBlockArray->Descriptors); /* Release entire descriptor array */
    PBlockArray->Descriptors = NULL; /* Mark as no allocation in entire array */
  } else { /* Reducing somewhat but not completely */
    /* Shrink array */
    PBlockArray->Descriptors = DPCRTLMM_REALLOC( PBlockArray->Descriptors, (PBlockArray->Count - Amount)*sizeof(S_DPCRTLMM_BLOCKDESCRIPTOR) );
  }
  PBlockArray->Count -= Amount; /* Adjust count for descriptor array */
  return;
}

static void OurLog(
  const char* File,
  const unsigned int Line,
  const unsigned short Severity,
  const char* Str
) {
  /* Our job is to add "Free() to the start of the string, saves data space
  if everybody in this module calls this instead of _Log() directly.
  We can't call _Log() twice because the information will be put on different
  lines so a copy is needed. */

  if (Str && Str[0]) { /* Valid string of at least on character sent to us? */
    char* PcopyStr;
    const char FuncName[] = "Free(): "; /* Prefix */

    PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) ); /* Allocate space for copy using xmalloc(), pointer allocation can't fail because if it does xmalloc() ends the program.  Note that NULL termination is automatic because using sizeof() */
    if (PcopyStr) {
      strcpy(PcopyStr, FuncName); /* Prepend prefix */
      strcat(PcopyStr, Str); /* Add log string after the prefix */

      dpcrtlmm_int_Log(File, Line, Severity, PcopyStr); /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}

