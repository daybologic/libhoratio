/*
Horatio's Memory Manager
Copyright (c) 2000-2014, David Duncan Ross Palmer (M6KVM), Daybo Logic
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

/*! \file horatio.c
 * \brief Main library function
 *
 * Minimal stuff here please,
 * if possible promote granularity by using other C files
 *
 * Incase you're wondering why HORATIO_SOURCE appears at the top of all
 * sources, it's to do with build.h.  That header is for the compilation of the
 * library only, not to be included in user programs.  If users include hbuild.h
 * the definition won't exist and build.h will tell them off!
 *
 * Allocations on behalf of callers are done with HORATIO_MALLOC,
 * internal allocations are done with malloc() and failure should be
 * ignored by checking for NULL.
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdlib.h> /* For normal C memory functions */
#include <string.h>
#include <stdio.h>

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal data */
#include "htrap.h" /* Trap() - Trap support */
#include "hlog.h" /* LOG macro - Logging support */
#include "hsafelst.h" /* Safety list support functions */
#include "hdbghook.h" /* The debug hook support/executive */
#include "hbiglock.h" /* To init / uninit the big lib lock */
#include "hblocarr.h" /* Internal interface to block arrays */
#include "hlic.h" /* horatio_license text */
#include "hversion.h" /* HORATIO_VERSION_ constants */

static void TrapUnFreedArrays(void); /* Traps all unfreed arrays */
static unsigned long TrapUnFreedBlocks(const PS_HORATIO_BLOCKDESCARRAY PArr);
unsigned char horatio__EnableTraps = 1U;

/*!
 * \brief Request version number from library at run-time.
 *
 * \param PVerStruct Pointer to version information structure to fill
 * \return PVerStruct, as passed.
 *
 * Although you can check, with the macros HORATIO_VERSION_MAJOR,
 * HORATIO_VERSION_MINOR and HORATIO_VERSION_PATCH, the version, using
 * macros at compile time, or print the version number with the same,
 * sometimes you may want to verify that the version number you are
 * running with, is acceptable for the version you were compiled for.
 * This function returns information about the binary library version
 * at compile time.
 *
 * Pass a pointer to a structure wide enough to hold the version number
 * and flags, and it will be populated by the library call.
 * Passing NULL will have no effect, but is safe.  The pointer passed to
 * the library will be returned for your convenience.  This function does
 * not fail.
 */
PS_HORATIO_VERSION horatio_Ver(PS_HORATIO_VERSION PVerStruct) {
	/*
	 * No need to lock the big global lock for this,
	 * only reading readonly data.
	 */
	if (PVerStruct) {
		/* Load version information into the caller's structure */
		PVerStruct->Major = HORATIO_VERSION_MAJOR;
		PVerStruct->Minor = HORATIO_VERSION_MINOR;
		PVerStruct->Patch = HORATIO_VERSION_PATCH;
		PVerStruct->Flags = (unsigned char)0U;
#ifdef DEBUG
		PVerStruct->Flags |= HORATIO_VERSION_DEBUG;
#endif /*DEBUG*/
#ifdef PRIVATE
		PVerStruct->Flags |= HORATIO_VERSION_PRIVATE;
#endif /*PRIVATE*/
#ifdef SPECIAL
		PVerStruct->Flags |= HORATIO_VERSION_SPECIAL;
#endif /*SPECIAL*/
#ifdef HORATIO_THREADS
		PVerStruct->Flags |= HORATIO_VERSION_MT;
#endif /*HORATIO_THREADS*/
	}
	return PVerStruct;
}

/*!
 * \brief Initialise the library
 *
 * Initialises the library's internal data structures.
 * This function must be called before any macro invocations,
 * function calls or static data flags are accessed or set.
 * This should be done as close to the start of main() as possible.
 * The atexit() handler for horatio_Shutdown() should not be registered
 * until this function returns.
 *
 * Failure in this function will cause an exit or abort of the process.
 */
void horatio_Startup() {
	if (!_libStarted) {
		/* Initialization of internal library data */
		_libStarted = 1U; /* The library is started now */
		_UserTrapCallback = NULL; /* No user trap handler installed */
		SafetyList_Init(); /* Init the safety list */
#ifdef HORATIO_DEBUGHOOKS
		/* Init the debug hook matrix */
		horatio_int_InitDebugHookMatrix();
#endif /*HORATIO_DEBUGHOOKS*/
#ifdef HORATIO_THREADS
		horatio_int_BigLockInit();
#endif /*HORATIO_THREADS*/
	} else { /* This has been done before! */
#ifdef HORATIO_DEBUGHOOKS
		S_HORATIO_DEBUGHOOKINFO debugHookInfo;
		memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));
		horatio_int_CallDebugHook(HORATIO_HOOK_STARTUP, &debugHookInfo);
#endif /*HORATIO_DEBUGHOOKS*/
		Trap(HORATIO_TRAP_MUL_STARTUP, "Multiple calls of Startup()!");
	}
	MESSAGE(HORATIO_LOG_CODE_STARTUP, NULL, 0, "Library started");
	return;
}

/*!
 * \brief Shut down the Horatio library.
 *
 * This function should be called just before the process terminates,
 * either in the last statement of the main() function, or within the last
 * atexit() handler before process termination.
 *
 * This function will return, but returns no value.
 * It should only be called once.
 * The library must hve been started before the call.
 * Do not call the function from a signal handler, or outside the main thread.
 */
void horatio_Shutdown() {

#ifdef HORATIO_DEBUGHOOKS

	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
	memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));

#endif /*HORATIO_DEBUGHOOKS*/

	if (_libStarted) {
		/* Cleanup of internal library data */
		_libStarted = 0U; /* The library has been shut down */
#ifdef HORATIO_THREADS
		horatio_int_BigLockUninit();
#endif /*HORATIO_THREADS*/
#ifdef HORATIO_DEBUGHOOKS
		/* Normal call to shutdown even if leaks are caught */
		debugHookInfo.Success = 1U;
		horatio_int_CallDebugHook(
			HORATIO_HOOK_SHUTDOWN,
			&debugHookInfo
		);
#endif /*HORATIO_DEBUGHOOKS*/
		/* Output log information if memory has not been released */
		TrapUnFreedArrays(); 
		MESSAGE(HORATIO_LOG_CODE_SHUTDOWN, NULL, 0, "Library shutdown");
	} else { /* This has been done before! */
		/* Call hooks and fire trap */
#ifdef HORATIO_DEBUGHOOKS
		debugHookInfo.Success = 0U; /* Failed */
		horatio_int_CallDebugHook(
			HORATIO_HOOK_SHUTDOWN,
			&debugHookInfo
		);
#endif /*HORATIO_DEBUGHOOKS*/
		Trap(
			HORATIO_TRAP_MUL_SHUTDOWN,
			"Multiple calls of Shutdown()"
		);
	}
	return;
}

/*!
 * \brief Obtain the license text
 */
const char *horatio_license() {

	return horatio_license_text;
}

/*!
 * \brief Determine whether the library has been initialised.
 *
 * \return Boolean truth or false value
 */
unsigned int horatio_IsStarted() {
  return _libStarted;
}

/*!
 * \brief Traps all unfreed arrays
 *
 * This function loops through all knon block descriptor arrays and
 * calls TrapUnFreedBlocks on each one in turn.  This function returns
 * nothing, and triggers the HORATIO_TRAP_UNFREED_DATA trap, should anything
 * have neglected to be released.
 */
static void TrapUnFreedArrays() {
  char trapMsg[MAX_TRAP_STRING_LENGTH+sizeof(char)]; /* Reserved for trap/log messages */
  size_t trapMsgRemaining = MAX_TRAP_STRING_LENGTH;
  long unsigned int numArraysUnfreed = 0U; /* Count of number of arrays the programmer failed to free (excludes built-in one which doesn't get destroyed) */
  unsigned long totalBytesLeaked = 0UL; /* The total bytes leaked for the whole program */
  unsigned int sli;
  for ( sli = 0U; sli < HORATIO_SAFETYLIST_MAXSIZE; sli++ ) { /* All entries in the safety list */
    unsigned int oldTrapEnablement; /* Temporary used while traps are supressed */
    if (_safetyList[sli]) { /* Discovered non-freed array? */
      /* numArraysUnfreed is used for a log message */
      numArraysUnfreed++; /* Increment count of failures to free an array */
      /* Make a log message that an array was not destroyed */
      sprintf(
        trapMsg,
        #ifdef HAVE_SNPRINTF
        trapMsgRemaining,
        #endif /*HAVE_SNPRINTF*/
        "Shutdown(): The array %s%p was not freed, any blocks unfreed in the array will be listed",
        HORATIO_FMTPTRPFX, (void*)(_safetyList[sli])
      );
      WARNING(HORATIO_LOG_CODE_UNFREED_ARRAY, trapMsg);
      if (_safetyList[sli]->Count) { /* Are there any unfreed blocks in the array? */
        totalBytesLeaked += TrapUnFreedBlocks(_safetyList[sli]);
      }
      /* The specified array must be automatically released */
      oldTrapEnablement = horatio__EnableTraps; /* Save old enablement */
      horatio__EnableTraps = 0U; /* Traps are supressed */
      horatio_int_DestroyBlockArray(_safetyList[sli]); /* Automatic garbage collection */
      if (oldTrapEnablement) /* Did traps used to be enabled? */
        horatio__EnableTraps = 1U; /* Traps are re-enabled */
    }
  }
  #ifndef HORATIO_NONULL_BLOCKDESCARRAY
  if (_defaultArray.Count) { /* Any unfreed blocks in the default ("NULL") array? */
    totalBytesLeaked += TrapUnFreedBlocks(&_defaultArray);
  }
  #endif /*!HORATIO_NONULL_BLOCKDESCARRAY*/
  if (numArraysUnfreed) {
    sprintf(
      trapMsg,
      #ifdef HAVE_SNPRINTF
      trapMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "%lu arrays were not freed",
      numArraysUnfreed
    );
    WARNING(HORATIO_LOG_CODE_UNFREED_ARRAY, trapMsg);
  }
  /* Entire list was processed, if there were any leaks report general message */
  if (totalBytesLeaked) { /* So, were there any unfreed arrays or blocks? */
    sprintf(
      trapMsg,
      #ifdef HAVE_SNPRINTF
      trapMsgRemaining,
      #endif /*HAVE_SNPRINTF*/
      "%lu bytes of memory leaked in total.\n",
      totalBytesLeaked
    );
    Trap(HORATIO_TRAP_UNFREED_DATA, trapMsg);
  }
  return;
}

/*!
 * \brief Trap unfreed blocks within an array
 *
 * \param PArr Pointer to block descriptor array
 *
 * \return Total bytes leaked
 *
 * Called by TrapUnFreedArrays(), not by other functions,
 * TrapUnFreedBlocks() does processing for a block array, I had
 * to put it in a separate function so it could be called in
 * two different locations, the safety list loop and then for the
 * built-in array, returns number of bytes wasted, the caller shall
 * this to the total leakage.
 */
static unsigned long TrapUnFreedBlocks(const PS_HORATIO_BLOCKDESCARRAY PArr) {
  char trapMsg[MAX_TRAP_STRING_LENGTH+sizeof(char)];
  size_t trapMsgRemaining = MAX_TRAP_STRING_LENGTH;
  unsigned int unfreedBlockCount;
  unsigned long totalLeakage = 0U; /* Total byte leakage for this array (blocks summed) */
  if (PArr) {
    if (PArr->Count) {
      if (!PArr->Descriptors) /* The descriptor base is NULL when the count is nonzero */
        abort(); /* This is not a normal trap event, internal error or user tamper of array information */
      unfreedBlockCount = PArr->Count; /* Save count of blocks which weren't freed in this array */
      /* I discovered I could not transend the list normally, after all horatio_Free() reduces the count
      the best way of dealing with this is to remove the top element, therefore, while the block array
      contains descriptors I shall remove the top one (0) */
      while ( PArr->Count ) {
        sprintf(
          trapMsg,
          #ifdef HAVE_SNPRINTF
          trapMsgRemaining,
          #endif /*HAVE_SNPRINTF*/
          "Block %s%p in descriptor array %s%p was not freed, size: %u bytes",
          HORATIO_FMTPTRPFX, PArr->Descriptors[0].PBase,
          HORATIO_FMTPTRPFX, (void*)PArr,
          (unsigned int)PArr->Descriptors[0].Size
        );
        MESSAGE(HORATIO_LOG_CODE_UNFREED_BLOCK, PArr->Descriptors[0].SourceFile, PArr->Descriptors[0].SourceLine, trapMsg);
        totalLeakage += PArr->Descriptors[0].Size; /* Add size of this block to the total leakage value */
        horatio_Free(PArr, PArr->Descriptors[0].PBase); /* Automatically collect the garbage */
      }
      /* Array leakage summary */
      sprintf(
        trapMsg,
        #ifdef HAVE_SNPRINTF
        trapMsgRemaining,
        #endif /*HAVE_SNPRINTF*/
        "Array leakage summary: array %s%p contained %u unfreed blocks, a total of %lu bytes",
        HORATIO_FMTPTRPFX, (void*)PArr,
        unfreedBlockCount,
        totalLeakage
      );
      WARNING(HORATIO_LOG_CODE_UNFREED_BLOCK, trapMsg);
    }
  } /*(PArr)*/
  return totalLeakage; /* Caller gets to know this so they can add it to a total */
}

