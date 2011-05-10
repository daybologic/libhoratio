/*
    DPCRTLMM Memory Management library : Primary block freeer
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
#include "intdata.h" /* Internal data */
#include "vptrap.h" /* _VerifyPtrs() */
#include "locktrap.h" /* _LockTrap() */
#include "log.h" /* LOG macro */
#include "trap.h" /* _Trap() */
#include "dbghooks.h" /* Debug hook executive */
/*-------------------------------------------------------------------------*/
#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

#define OURLOG(sev, msg) OurLog(((const unsigned short)(sev)), (msg))

/* Always make sure to pass resolved arrays to these functions */
static void Moveup(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int StartPos); /* Moveup following blocks descriptors in array to remove blank space.  StartPos is the item just deleted when moveup will be started from */
static void ShrinkBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int Amount); /* Shrink array, trap is fired on an attempt to shrink more than the current size */
static void OurLog(const unsigned short Severity, const char* Msg);
/*-------------------------------------------------------------------------*/
void dpcrtlmm_Free(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA* Ptr)
{
  /* locals */
  unsigned int i; /* For the finder loop */
  char trapMsg[MAX_TRAP_STRING_LENGTH + sizeof(char)];
  const char funcName[] = "Free()"; /* Our function name */
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr; /* Resolved array pointer */
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugTrapInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  #ifdef DPCRTLMM_LOG
  sprintf(trapMsg, "Attempting release of block 0x%p from array 0x%p . . .", Ptr, PBlockArray);
  OURLOG(DPCRTLMM_LOG_MESSAGE, trapMsg);
  #endif /*DPCRTLMM_LOG*/

  PRArr = _ResolveArrayPtr(PBlockArray); /* Resolve incase block array is NULL */
  _VerifyPtrs(funcName, PBlockArray, NULL); /* Don't check if bad block in this trap, use own trap... */
  if ( dpcrtlmm_IsBadBlockPtr(PBlockArray, Ptr) ) /* Block pointer not valid? */
  {
    sprintf(trapMsg, "Free(): Attempt to release memory we don\'t own or memory which has already been released, array: 0x%p, block 0x%p",
	    PBlockArray,
	    Ptr
    );
    _Trap(DPCRTLMM_TRAP_UNOWNED_FREE, trapMsg);
  }

  if (_LockTrap(funcName, PBlockArray, Ptr)) /* Do trap if block is locked */
    return;

  /* Find the block's descriptor using the block base address, have a caution to use the resolved array pointer */
  for ( i = 0U; i < PRArr->Count; i++ )
  {
    if ( PRArr->Descriptors[i].PBase == Ptr ) /* This is the one */
    {
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
/*-------------------------------------------------------------------------*/
static void Moveup(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int StartPos)
{
  /* locals */
  unsigned int i; /* Loop control */

  if ( PBlockArray->Count < 2) /* Only one or no items, can't do a moveup */
  {
    /* Do trap */
    _Trap(DPCRTLMM_TRAP_BAD_RANGE_MOVEUP, "Free()/Moveup: Can\'t move up one item or no items.\n");
    return;
  }
  if ( StartPos >= PBlockArray->Count ) /* StartPos out of range? */
  {
    /* Do trap */
    char trapMsg[MAX_TRAP_STRING_LENGTH+1]; /* Space for trap message */

    sprintf(trapMsg, "Free()/Moveup: StartPos is not valid. StartPos=%u, 0x%p->Count=%u",
                     StartPos,
                     PBlockArray,
                     PBlockArray->Count
    );
    _Trap(DPCRTLMM_TRAP_BAD_RANGE_MOVEUP, trapMsg);
    return;
  }

  /* Moving elements left to fill a gap */
  for ( i = StartPos+1; i < PBlockArray->Count; i++ )
  {
    S_DPCRTLMM_BLOCKDESCRIPTOR blockDesc;

    blockDesc = PBlockArray->Descriptors[i];
    PBlockArray->Descriptors[i-1] = blockDesc;
  }
  return;
}
/*-------------------------------------------------------------------------*/
static void ShrinkBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int Amount)
{
  _VerifyPtrs("ShrinkBlockArray()", PBlockArray, NULL); /* Ensure array is valid */
  if (!Amount)
  {
    char logMsg[MAX_TRAP_STRING_LENGTH+1];

    sprintf(logMsg, "Attempt to ShrinkBlockArray(0x%p) by nothing, ignored (internal DPCRTLMM error)", PBlockArray);
    OURLOG(DPCRTLMM_LOG_WARNING, logMsg);
    return;
  }
  if (!PBlockArray->Count)
  {
    char trapMsg[MAX_TRAP_STRING_LENGTH+1];

    sprintf(trapMsg, "ShrinkBlockArray(): 0x%p->Count=0U, can\'t shrink the array any more!", PBlockArray);
    _Trap(DPCRTLMM_TRAP_SHRINKARR_WHILE_NOWT, trapMsg);
    return;
  }
  if (Amount > PBlockArray->Count) /* Shrink further than size?! */
  {
    char trapMsg[MAX_TRAP_STRING_LENGTH+1];
    sprintf(trapMsg, "ShrinkBlockArray(): Amount=%u, greater than original size in elements (0x%p->Count=%u)",
            Amount,
            PBlockArray,
            PBlockArray->Count
    );
    _Trap(DPCRTLMM_TRAP_SHRINKARR_TOOMUCH, trapMsg);
    return;
  }

  if (!(PBlockArray->Count - Amount)) /* Reducing to zero? */
  {
    DPCRTLMM_FREE(PBlockArray->Descriptors); /* Release entire descriptor array */
    PBlockArray->Descriptors = NULL; /* Mark as no allocation in entire array */
  }
  else /* Reducing somewhat but not completely */
  {
    /* Shrink array */
    PBlockArray->Descriptors = DPCRTLMM_REALLOC( PBlockArray->Descriptors, (PBlockArray->Count - Amount)*sizeof(S_DPCRTLMM_BLOCKDESCRIPTOR) );
  }
  PBlockArray->Count -= Amount; /* Adjust count for descriptor array */
  return;
}
/*-------------------------------------------------------------------------*/
static void OurLog(const unsigned short Severity, const char* Str)
{
  /* Our job is to add "Free() to the start of the string, saves data space
  if everybody in this module calls this instead of _Log() directly.
  We can't call _Log() twice because the information will be put on different
  lines so a copy is needed. */

  if (Str && Str[0]) /* Valid string of at least on character sent to us? */
  {
    char* PcopyStr;
    const char FuncName[] = "Free(): "; /* Prefix */

    PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) ); /* Allocate space for copy using xmalloc(), pointer allocation can't fail because if it does xmalloc() ends the program.  Note that NULL termination is automatic because using sizeof() */
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
