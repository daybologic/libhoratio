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
#include <stdlib.h> /* for Abort() */
#include <stdio.h>
#include <string.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal data */
#include "trap.h" /* _Trap() - Trap support */
#include "log.h" /* LOG macro - Logging support */
#include "safelist.h" /* Safety list support functions */
#include "dbghooks.h" /* The debug hook support/executive */
/*-------------------------------------------------------------------------*/
static void TrapUnFreedArrays(void); /* Traps all unfreed arrays */
/* Called by TrapUnFreedArrays(), not by other functions,
TrapUnFreedBlocks() does processing for a block array, I had
to put it in a separate function so it could be called in
two different locations, the safety list loop and then for the
built-in array, returns number of bytes wasted, the called wil
add this to the total leakage */
static unsigned long TrapUnFreedBlocks(const PS_DPCRTLMM_BLOCKDESCARRAY PArr);
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD)
static void Banner(void);
#endif
/*-------------------------------------------------------------------------*/
void dpcrtlmm_Startup()
{
  static unsigned int doneOnce;

  if (!doneOnce) /* Not done this before */
  {
    doneOnce = 1U; /* Remember we did this so we don't do it again */

    /* Initialization of internal library data */
    _libStarted = 1U; /* The library is started now */
    _UserTrapCallback = NULL; /* No user trap handler installed */
    SafetyList_Init(); /* Init the safety list */
    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_InitDebugHookMatrix(); /* Init the debug hook matrix */
    #endif /*DPCRTLMM_DEBUGHOOKS*/
  }
  else /* This has been done before! */
  {
    #ifdef DPCRTLMM_DEBUGHOOKS
    S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;

    memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_STARTUP, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    _Trap(DPCRTLMM_TRAP_MUL_STARTUP, "Multiple calls of Startup()!");
  }
  LOG("Library started")
  #if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD)
  Banner();
  #endif
  return;
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_Shutdown()
{
  /* Don't moan about my double use of the define, I like it this
  way, it feels cleaner, declarations separated! */
  static unsigned int doneOnce;
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  if (!doneOnce) /* Not done this before */
  {
    doneOnce = 1U; /* Remember we did this so we don't do it again */

    /* Cleanup of internal library data */
    _libStarted = 0U; /* The library has been shut down */

    #ifdef DPCRTLMM_DEBUGHOOKS
    debugHookInfo.Success = 1U; /* Normal call to shutdown even if leaks are caught */
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_SHUTDOWN, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    TrapUnFreedArrays(); /* Output log information if memory has not been released */
  }
  else /* This has been done before! */
  {
    /* Call hooks and fire trap */
    #ifdef DPCRTLMM_DEBUGHOOKS
    debugHookInfo.Success = 0U; /* Failed */
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_SHUTDOWN, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/
    _Trap(DPCRTLMM_TRAP_MUL_SHUTDOWN, "Multiple calls of Shutdown()");
  }
  LOG("Library shutdown");
  return;
}
/*-------------------------------------------------------------------------*/
static void TrapUnFreedArrays()
{
  char trapMsg[MAX_TRAP_STRING_LENGTH+sizeof(char)]; /* Reserved for trap/log messages */
  #ifdef DPCRTLMM_LOG
  unsigned int numArraysUnfreed = 0U; /* Count of number of arrays the programmer failed to free (excludes built-in one which doesn't get destroyed) */
  #endif /*DPCRTLMM_LOG*/
  unsigned long totalBytesLeaked = 0UL; /* The total bytes leaked for the whole program */
  unsigned int sli;

  for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ ) /* All entries in the safety list */
  {
    unsigned int oldTrapEnablement; /* Temporary used while traps are supressed */

    if (_safetyList[sli]) /* Discovered non-freed array? */
    {
      #ifdef DPCRTLMM_LOG
      /* numArraysUnfreed is used for a log message */
      numArraysUnfreed++; /* Increment count of failures to free an array */
      #endif /*DPCRTLMM_LOG*/

      #ifdef DPCRTLMM_LOG
      /* Make a log message that an array was not destroyed */
      sprintf(trapMsg, "Shutdown(): The array 0x%p was not freed, any blocks unfreed in the array will be listed",
              _safetyList[sli]
      );
      #endif /*DPCRTLMM_LOG*/
      LOG(trapMsg)

      if (_safetyList[sli]->Count) /* Are there any unfreed blocks in the array? */
      {
        totalBytesLeaked += TrapUnFreedBlocks(_safetyList[sli]);
      }
      /* The specified array must be automatically released */
      oldTrapEnablement = dpcrtlmm__EnableTraps; /* Save old enablement */
      dpcrtlmm__EnableTraps = 0U; /* Traps are supressed */
      dpcrtlmm_DestroyBlockArray(_safetyList[sli]); /* Automatic garbage collection */
      if (oldTrapEnablement) /* Did traps used to be enabled? */
        dpcrtlmm__EnableTraps = 1U; /* Traps are re-enabled */
    }
  }

  if (_defaultArray.Count) /* Any unfreed blocks in the default ("NULL") array? */
  {
    totalBytesLeaked += TrapUnFreedBlocks(&_defaultArray);
  }

  #ifdef DPCRTLMM_LOG
  if (numArraysUnfreed)
  {
    sprintf(trapMsg, "%lu arrays were not freed", numArraysUnfreed);
    LOG(trapMsg)
  }
  #endif /*DPCRTLMM_LOG*/

  /* Entire list was processed, if there were any leaks report general message */
  if (totalBytesLeaked) /* So, were there any unfreed arrays or blocks? */
  {
    sprintf(trapMsg, "%lu bytes of memory leaked in total,\n", totalBytesLeaked);
    if (totalBytesLeaked >= 524288UL) /* >= 500K!? */
      strcat(trapMsg, "that\'s terrible!!");
    else if (totalBytesLeaked < 1024UL)
      strcat(trapMsg, "only solve this problem if it will not take too much development time and does not increase with time.");
    else
      strcat(trapMsg, "You should use the log as an aid to resolving this.");

    _Trap(DPCRTLMM_TRAP_UNFREED_DATA, trapMsg);
  }

  return;
}
/*-------------------------------------------------------------------------*/
static unsigned long TrapUnFreedBlocks(const PS_DPCRTLMM_BLOCKDESCARRAY PArr)
{
  #ifdef DPCRTLMM_LOG
  char trapMsg[MAX_TRAP_STRING_LENGTH+sizeof(char)];
  unsigned int unfreedBlockCount;
  #endif /*DPCRTLMM_LOG*/
  unsigned long totalLeakage = 0U; /* Total byte leakage for this array (blocks summed) */

  if (PArr)
  {
    if (PArr->Count)
    {
      if (!PArr->Descriptors) /* The descriptor base is NULL when the count is nonzero */
        abort(); /* This is not a normal trap event, internal error or user tamper of array information */

      #ifdef DPCRTLMM_LOG
      unfreedBlockCount = PArr->Count; /* Save count of blocks which weren't freed in this array */
      #endif /*DPCRTLMM_LOG*/

      /* I discovered I could not transend the list normally, after all dpcrtlmm_Free() reduces the count
      the best way of dealing with this is to remove the top element, therefore, while the block array
      contains descriptors I shall remove the top one (0) */
      while ( PArr->Count )
      {
        #ifdef DPCRTLMM_LOG
        sprintf(trapMsg, "Block 0x%p in descriptor array 0x%p was not freed, size: %u bytes",
                PArr->Descriptors[0].PBase,
                PArr,
                PArr->Descriptors[0].Size
        );
        #endif /*DPCRTLMM_LOG*/
        LOG(trapMsg);
        totalLeakage += PArr->Descriptors[0].Size; /* Add size of this block to the total leakage value */
        dpcrtlmm_Free(PArr, PArr->Descriptors[0].PBase); /* Automatically collect the garbage */
      }
      #ifdef DPCRTLMM_LOG
      /* Array leakage summary */
      sprintf(trapMsg, "Array leakage summary: array 0x%p contained %u unfreed blocks, a total of %lu bytes",
              PArr,
              unfreedBlockCount,
              totalLeakage
      );
      #endif /*DPCRTLMM_LOG*/
      LOG(trapMsg)
    }
  } /*(PArr)*/

  return totalLeakage; /* Caller gets to know this so they can add it to a total */
}
/*-------------------------------------------------------------------------*/
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD)
static void Banner()
{
  S_DPCRTLMM_VERSION ver;

  dpcrtlmm_Ver(&ver);
  printf("DPCRTLMM Memory Manager freeware  Version %u.%u\nCopyright 2000 David D R Palmer, Daybo Logic, all rights reserved.\nhttp://daybologic.com/Dev/dpcrtlmm\n\n",
         ver.Major,
         ver.Minor
  );
}
#endif
/*-------------------------------------------------------------------------*/
