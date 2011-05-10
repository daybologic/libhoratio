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

#ifdef DPCRTLMM_LOG /* Local logger only available in a log build */
   static void OurLog(const char* Str); /* Logging wrapper which knows our name */
#  define OURLOG(msg) OurLog(msg); /* Macro goes to local function */
#else /* Not a loggable build */
#  define OURLOG(msg) /* Dummy version so logging isn't done and log strings don't end up in the binary of the library */
#endif /*DPCRTLMM_LOG*/

/* Always make sure to pass resolved arrays to these functions */
static void Moveup(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int StartPos); /* Moveup following blocks descriptors in array to remove blank space.  StartPos is the item just deleted when moveup will be started from */
static void ShrinkBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int Amount); /* Shrink array, trap is fired on an attempt to shrink more than the current size */
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
  OURLOG(trapMsg);
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

  OURLOG("/Moveup: Executing Moveup!")
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
  OURLOG("/Moveup: Finished Moveup operation")
  return;
}
/*-------------------------------------------------------------------------*/
static void ShrinkBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int Amount)
{
  _VerifyPtrs("ShrinkBlockArray()", PBlockArray, NULL); /* Ensure array is valid */
  if (!Amount)
  {
    #ifdef DPCRTLMM_LOG
    char logMsg[MAX_TRAP_STRING_LENGTH+1];

    sprintf(logMsg, "WARNING! Attempt to ShrinkBlockArray(0x%p) by nothing, ignored", PBlockArray);
    OURLOG(logMsg)
    #endif /*DPCRTLMM_LOG*/
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
    OURLOG("Attempting to free array interior but leave usable array base (shrinking to zero)...")
    DPCRTLMM_FREE(PBlockArray->Descriptors); /* Release entire descriptor array */
    PBlockArray->Descriptors = NULL; /* Mark as no allocation in entire array */
  }
  else /* Reducing somewhat but not completely */
  {
    OURLOG("Attempting to shrink an array leaving usable entries...")
    /* Shrink array */
    PBlockArray->Descriptors = DPCRTLMM_REALLOC( PBlockArray->Descriptors, (PBlockArray->Count - Amount)*sizeof(S_DPCRTLMM_BLOCKDESCRIPTOR) );
  }
  OURLOG("The resize appears to have completed successfully, adjusting count...")
  PBlockArray->Count -= Amount; /* Adjust count for descriptor array */
  OURLOG("The block descriptor array count is adjusted")
  return;
}
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_LOG /* Loggable build? */
static void OurLog(const char* Str)
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

      LOG(PcopyStr); /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}
#endif /*DPCRTLMM_LOG*/
/*-------------------------------------------------------------------------*/
