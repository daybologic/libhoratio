/*
    DPCRTLMM Memory Management library
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
Get updates: http://daybologic.com/Dev/dpcrtlmm
My official site: http://daybologic.com/overlord
*/
/* Incase you're wondering why DPCRTLMM_SOURCE appears at the top of all my
source, it's to do with build.h.  That header is for the compilation fo the
library only, not to be included in user programs.  If users include build.h
the definition won't exist and build.h will tell them off! */
#define DPCRTLMM_SOURCE
/* Allocations on behalf of callers are done with DPCRTLMM_MALLOC, internal
allocations are done with malloc() and failure should be ignored by checking
for NULL */

#include <stdlib.h> /* For normal C memory functions */
#include <string.h>
#include <stdio.h>
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
/*
Minimal stuff here please, if possible promote granularity by using other C files
*/
static void TrapUnFreedArrays(void); /* Traps all unfreed arrays */
/* Called by TrapUnFreedArrays(), not by other functions,
TrapUnFreedBlocks() does processing for a block array, I had
to put it in a separate function so it could be called in
two different locations, the safety list loop and then for the
built-in array, returns number of bytes wasted, the called wil
add this to the total leakage */
static unsigned long TrapUnFreedBlocks(const PS_DPCRTLMM_BLOCKDESCARRAY PArr);

unsigned char dpcrtlmm__EnableTraps = 1U;
/*-------------------------------------------------------------------------*/
PS_DPCRTLMM_VERSION dpcrtlmm_Ver(PS_DPCRTLMM_VERSION PVerStruct)
{
  if (PVerStruct)
  {
    /* Load version information into the caller's structure */
    PVerStruct->Major = DPCRTLMM_VERSION_MAJOR;
    PVerStruct->Minor = DPCRTLMM_VERSION_MINOR;
    PVerStruct->Flags = (unsigned char)0U;
    #ifdef DPCRTLMM_VERSION_TEST /* Test build? */
    PVerStruct->Flags |= 1;
    #endif /*DPCRTLMM_VERSION_TEST*/
  }
  return NULL;
}
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
  MESSAGE("Library started");
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
  MESSAGE("Library shutdown");
  return;
}
/*-------------------------------------------------------------------------*/
static void TrapUnFreedArrays()
{
  char trapMsg[MAX_TRAP_STRING_LENGTH+sizeof(char)]; /* Reserved for trap/log messages */
  long unsigned int numArraysUnfreed = 0U; /* Count of number of arrays the programmer failed to free (excludes built-in one which doesn't get destroyed) */
  unsigned long totalBytesLeaked = 0UL; /* The total bytes leaked for the whole program */
  unsigned int sli;

  for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ ) /* All entries in the safety list */
  {
    unsigned int oldTrapEnablement; /* Temporary used while traps are supressed */

    if (_safetyList[sli]) /* Discovered non-freed array? */
    {
      /* numArraysUnfreed is used for a log message */
      numArraysUnfreed++; /* Increment count of failures to free an array */

      /* Make a log message that an array was not destroyed */
      sprintf(trapMsg, "Shutdown(): The array 0x%p was not freed, any blocks unfreed in the array will be listed",
              _safetyList[sli]
      );
      WARNING(trapMsg);

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

  if (numArraysUnfreed)
  {
    sprintf(trapMsg, "%lu arrays were not freed", numArraysUnfreed);
    WARNING(trapMsg);
  }

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
  char trapMsg[MAX_TRAP_STRING_LENGTH+sizeof(char)];
  unsigned int unfreedBlockCount;
  unsigned long totalLeakage = 0U; /* Total byte leakage for this array (blocks summed) */

  if (PArr)
  {
    if (PArr->Count)
    {
      if (!PArr->Descriptors) /* The descriptor base is NULL when the count is nonzero */
        abort(); /* This is not a normal trap event, internal error or user tamper of array information */

      unfreedBlockCount = PArr->Count; /* Save count of blocks which weren't freed in this array */

      /* I discovered I could not transend the list normally, after all dpcrtlmm_Free() reduces the count
      the best way of dealing with this is to remove the top element, therefore, while the block array
      contains descriptors I shall remove the top one (0) */
      while ( PArr->Count )
      {
        sprintf(trapMsg, "Block 0x%p in descriptor array 0x%p was not freed, size: %u bytes",
                PArr->Descriptors[0].PBase,
                PArr,
                PArr->Descriptors[0].Size
        );
        MESSAGE(trapMsg);
        totalLeakage += PArr->Descriptors[0].Size; /* Add size of this block to the total leakage value */
        dpcrtlmm_Free(PArr, PArr->Descriptors[0].PBase); /* Automatically collect the garbage */
      }

      /* Array leakage summary */
      sprintf(trapMsg, "Array leakage summary: array 0x%p contained %u unfreed blocks, a total of %lu bytes",
              PArr,
              unfreedBlockCount,
              totalLeakage
             );
      WARNING(trapMsg);
    }
  } /*(PArr)*/

  return totalLeakage; /* Caller gets to know this so they can add it to a total */
}
/*-------------------------------------------------------------------------*/
