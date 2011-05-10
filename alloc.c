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

/* Grow the array by 'GrowByElems' elements, returns FALSE if
it fails but then the original array is still valid and no
bigger.  Always make sure the array pointer is resolved, NULL
pointers are not acceptable and will be caught with assert() */
static unsigned int GrowBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PCurrentBlockArray, const unsigned int GrowByElems);
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD)
  static void Nag(void);
#endif
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_Alloc(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const size_t NewBlockSize)
{
  /* locals */
  void DPCRTLMM_FARDATA* genBlockPtr; /* Generated block pointer */
  #ifdef DPCRTLMM_LOG
  char logMsg[MAX_TRAP_STRING_LENGTH + sizeof(char)];
  #endif /*DPCRTLMM_LOG*/
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray); /* Resolving is done because of a possible NULL */

  _VerifyPtrs("Alloc()", PBlockArray, NULL); /* Haults program if array not valid, third arg is not applicable here */

  #ifdef DPCRTLMM_LOG
  sprintf(logMsg, "Program Requested to allocate %u byte block for array 0x%p",
	  NewBlockSize,
	  PBlockArray
  );
  #endif /*DPCRTLMM_LOG*/
  OURLOG(logMsg)

  #if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD) /* Does this actually work? - DEBUG */
  if ( dpcrtlmm_GetBlockCount() >= 50 ) /* Already 50 blocks used (limit for free version) */
  {
    printf("Your program has already reached the limit of 50 blocks\n");
    Nag();
    dpcrtlmm__EnableTraps = 0U; /* Disable traps, so we don't see a leak report... */
    dpcrtlmm_Shutdown(); /* Clean all current memory used by the program and the library */
    exit(0); /* Terminate the program as cleanly as we can, we don't always know what the caller might have left open though */
  }
  #endif

  genBlockPtr = DPCRTLMM_MALLOC(NewBlockSize); /* Allocate block */
  if (!genBlockPtr) /* Out of memory? */
  {
    #ifdef DPCRTLMM_LOG
    /* Use buffer for log messages, it's the same size as for traps */
    sprintf(logMsg, "Attempt to allocate block of %u bytes for array at base 0x%p has failed",
	    NewBlockSize,
	    PBlockArray
    );
    #endif /*DPCRTLMM_LOG*/
    OURLOG(logMsg)
    return NULL; /* No pointer generated */
  }

  /* Now add the block to the array, first grow array */
  if (!GrowBlockArray(PRArr, 1))
  {
    /* Attempt to enlarge the array failed? */
    DPCRTLMM_FREE(genBlockPtr); /* Release the new block of memory */

    #ifdef DPCRTLMM_LOG
    sprintf(logMsg, "Attempt to enlarge array at base 0x%p by one element failed",
	    PBlockArray
    );
    #endif /*DPCRTLMM_LOG*/
    OURLOG(logMsg)
    return NULL; /* Give up */
  }

  /* Now the block's address can be added to the array */
  PRArr->Descriptors[PRArr->Count-1].PBase = genBlockPtr; /* Put pointer to base of block in block descriptor in the array */
  PRArr->Descriptors[PRArr->Count-1].Size = NewBlockSize; /* Save size so caller can find it out leter */

  /* Update library statistics */
  dpcrtlmm_int__blockCount++;
  dpcrtlmm_int__allocCharge += NewBlockSize;
  /* Update peaks */
  if ( dpcrtlmm_int__blockCount > dpcrtlmm_int__blockCountPeak )
    dpcrtlmm_int__blockCountPeak = dpcrtlmm_int__blockCount;
  if ( dpcrtlmm_int__allocCharge > dpcrtlmm_int__allocPeak )
    dpcrtlmm_int__allocPeak = dpcrtlmm_int__allocCharge;

  #ifdef DPCRTLMM_LOG
  sprintf(logMsg, "New block of %u bytes is at 0x%p, descarray (0x%p) was extended to hold a new descriptor\nAlloc() returns 0x%p\nLeaving...",
	  NewBlockSize,
	  genBlockPtr,
	  PBlockArray,
	  genBlockPtr
  );
  #endif /*DPCRTLMM_LOG*/
  OURLOG(logMsg)

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
    OURLOG("WARNING! Attempt to GrowBlockArray() by no items, ignored")
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
  }
  return 1U; /* Success */
}
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_LOG /* OurLog() only in loggable builds */
static void OurLog(const char* Str)
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

      LOG(PcopyStr) /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}
#endif /*DPCRTLMM_LOG*/
/*-------------------------------------------------------------------------*/
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD)
static void Nag()
{
  printf("Commercial use of DPCRTLMM must be licensed, read your license agreement for\n");
  printf("further information.  Current version of the library and information on how\n");
  printf("to register can be found at http://daybologic.com/Dev/dpcrtlmm\n");
  printf("Please register soon.\n\n");
}
#endif
/*-------------------------------------------------------------------------*/