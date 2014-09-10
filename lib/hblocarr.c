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

/*!
 * \file Block array creation and destruction functions
 *
 * Normally each module or section of a program will make it's own array
 * using the functions here-in, if it is too much trouble (because of cleaning
 * up each module) or the program wants to hide behind a normal allocation
 * function re-routed to us via a drop-in wrapper then only one block array
 * exists per process.
 *
 * 24/11/2001 (DDRP): Attention, block arrays need to start supporting file/
 * line info soon.
*/

#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset() */

#ifdef HORATIO_WANTFARDATA
# ifdef HAVE_ALLOC_H
#  include <alloc.h>
# endif /*HAVE_ALLOC_H*/
#endif /*HORATIO_WANTFARDATA*/

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Access to internal data */
#include "hlog.h" /* LOG macro */
#include "htrap.h" /* Trap() */
#include "hsafelst.h" /* Safety list support functions */
#include "hdbghook.h" /* For the debug hook executive */
#include "hbiglock.h" /* For total library mutual exclusion */
#include "hblocarr.h"

static PS_HORATIO_BLOCKDESCARRAY horatio_int_CreateBlockArray(void);
static unsigned int horatio_int_IsDefaultBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
);

PS_HORATIO_BLOCKDESCARRAY horatio_CreateBlockArray() {
	/* Thread safe wrapper for CreateBlockArray() */
	PS_HORATIO_BLOCKDESCARRAY ret;

	LOCK
	ret = horatio_int_CreateBlockArray();
	UNLOCK

	return ret;
}

void horatio_DestroyBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	/* Thread safe wrapper for DestroyBlockArray() */

	LOCK
	horatio_int_DestroyBlockArray(PBlockArray);
	UNLOCK
}

unsigned int horatio_IsDefaultBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	/* Thread safe wrapper for IsDefaultBlockArray() */

	unsigned int ret;

	LOCK
	ret = horatio_int_IsDefaultBlockArray(PBlockArray);
	UNLOCK

	return ret;
}

static PS_HORATIO_BLOCKDESCARRAY horatio_int_CreateBlockArray() {
	PS_HORATIO_BLOCKDESCARRAY Parray; /* Pointer for caller */
#ifdef HORATIO_LOG
	char logMsg[MAX_TRAP_STRING_LENGTH+1];
#endif /*HORATIO_LOG*/
#ifdef HORATIO_DEBUGHOOKS
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
	/* Init debug hook info */
	memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));
	debugHookInfo.HookType = HORATIO_HOOK_CREATEBLOCKARRAY;
	/* Ha, this is only vaugely relevant, this will do */
	debugHookInfo.AllocReq = (unsigned int)sizeof(S_HORATIO_BLOCKDESCARRAY);
#endif /*HORATIO_DEBUGHOOKS*/

	/* Alloc the array for the caller */
	Parray = (S_HORATIO_BLOCKDESCARRAY*)malloc(
		sizeof(S_HORATIO_BLOCKDESCARRAY)
	);
	if (!Parray) { /* Failed to alloc */
		/*
		 * Memory outages while in memory manager mode
		 * must be warned about!
		 */
		WARNING(
			"CreateBlockArray(): "
			"Couldn\'t allocate the new block array!"
		);
#ifdef HORATIO_DEBUGHOOKS
		/* PRelArr is nothing, we couldn't allocate one :( */
		/* PRelDesc is nothing, there is no related descriptor */
		debugHookInfo.Success = 0U; /* Ahh, no failure! */
		/* The rest are reserved or not used */
		horatio_int_CallDebugHook(
			HORATIO_HOOK_CREATEBLOCKARRAY, &debugHookInfo
		); /* Call the debug hook executive */
#endif /*HORATIO_DEBUGHOOKS*/
		return Parray; /* Give the NULL pointer back to the caller */
	}
	Parray->Count = 0U; /* No descriptors in list */
	Parray->Descriptors = NULL; /* Nothing in block list */

	/*
	 * The array base must be added to the list of acceptable arrays,
	 * (the so called safety list)
	 */
	if ( !SafetyList_AddBase(Parray) ) { /* Add to safety list */
		/*
		 * Failed to add to the list?!
		 * Memory outages while in memory manager must be warned about
		 **/
		WARNING(
			"CreateBlockArray(): The array base address "
			"could not be added to the safety list"
		);
		HORATIO_FREE(Parray); /* Free the array again */
		Parray = NULL; /* So caller sees there's nothing allocated */
	}

#ifdef HORATIO_LOG
	/* Safe, log progress */
	sprintf(
		logMsg,
		"CreateBlockArray() returns base %s%p",
		HORATIO_FMTPTRPFX, (void*)Parray
	);
	MESSAGE(__FILE__, __LINE__, logMsg);
#endif /*HORATIO_LOG*/

#ifdef HORATIO_DEBUGHOOKS
	/*
	 * Set up more hook information to indicate success
	 * The relavant array in this case is the one allocated
	 */
	debugHookInfo.PRelArr = Parray;
	debugHookInfo.Success = 1U; /* Yay, success! */
	horatio_int_CallDebugHook(
		HORATIO_HOOK_CREATEBLOCKARRAY,
		&debugHookInfo
	); /* Call the debug hook executive */
#endif /*HORATIO_DEBUGHOOKS*/
	return Parray; /* Give new pointer to the caller */
}

void horatio_int_DestroyBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	/* locals */
	unsigned int sli; /* Safety list loop processing */
	size_t trapMsgRemaining = MAX_TRAP_STRING_LENGTH;

	/* Sometimes used for creating trap strings */
	char trapStr[MAX_TRAP_STRING_LENGTH+1];

#ifdef HORATIO_DEBUGHOOKS
	/* Used for calling the debug hook executive */
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
	/* Set up common stuff for the debug hook info */
	debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
	/* There is no relavent descriptor */
	debugHookInfo.HookType = HORATIO_HOOK_DESTROYBLOCKARRAY;
	/* There is no allocation request */
#endif /*HORATIO_DEBUGHOOKS*/

	/* For all the possible items in the safety list */
	for ( sli = 0U; sli < HORATIO_SAFETYLIST_MAXSIZE; sli++ ) {
		if (_safetyList[sli]) { /* Is this entry used? */
			if (_safetyList[sli] == PBlockArray) {
				/* Pointer match!  Any descriptors remaining? */
				if (_safetyList[sli]->Count) {
					unsigned long totBytes = 0UL;
					unsigned int li;

					for (
						li = 0;
						li < _safetyList[sli]->Count;
						li++
					) { /* All blocks */
						/* Add size of block to total */
						totBytes += _safetyList[sli]->
							Descriptors[li].Size;
					}

					sprintf(
						trapStr,
#ifdef HAVE_SNPRINTF
						trapMsgRemaining,
#endif /*HAVE_SNPRINTF*/
						"DestroyBlockArray(): "
						"%u blocks of memory not freed"
						" from array based at %s%p\n"
						"Total bytes leakage for this "
						"array: %lu",
						_safetyList[sli]->Count,
						HORATIO_FMTPTRPFX,
						(void*)_safetyList[sli],
						totBytes
					);
					Trap(
						HORATIO_TRAP_UNFREED_BLOCKS,
						trapStr
					);
				}

				if (_safetyList[sli]->Descriptors) {
					/* Descriptors not zero */

					sprintf(
						trapStr,
	#ifdef HAVE_SNPRINTF
						trapMsgRemaining,
	#endif /*HAVE_SNPRINTF*/
						"DestroyBlockArray(): "
						"Base of raw descriptor array"
						" not freed!\n%s%p->%s%p "
						"(PBlockArray->"
						"Descriptors must be NULL)",
						HORATIO_FMTPTRPFX,
						(void*)_safetyList[sli],
						HORATIO_FMTPTRPFX,
						(void*)_safetyList[sli]->
							Descriptors
					);
					Trap(HORATIO_TRAP_BASENONZERO, trapStr);
				}

				/*
				 * BUG FIX: Forgot to release the memory for
				 * the array block pointer
				 */
				HORATIO_FREE(_safetyList[sli]);
				/* Remove this array from the safety list */
				_safetyList[sli] = NULL;
#ifdef HORATIO_LOG
				sprintf(
					trapStr,
#ifdef HAVE_SNPRINTF
					trapMsgRemaining,
#endif /*HAVE_SNPRINTF*/
					"DestroyBlockArray(): The array at "
					"base %s%p was destroyed",
					HORATIO_FMTPTRPFX,
					(void*)PBlockArray
				); /* Prepare log message */
				MESSAGE(__FILE__, __LINE__, trapStr);
#endif /*HORATIO_LOG*/

#ifdef HORATIO_DEBUGHOOKS
				/* Success, call hooks */
				debugHookInfo.Success = 1U; /* Wicked! */
				horatio_int_CallDebugHook(
					HORATIO_HOOK_DESTROYBLOCKARRAY,
					&debugHookInfo
				);
#endif /*HORATIO_DEBUGHOOKS*/
				return; /* Exit to caller */
			}
		}
	}
	/* Entire list processed, array base specified not found */
#ifdef HORATIO_DEBUGHOOKS
	/* Call hooks */
	debugHookInfo.Success = 0U; /* Failed */
	horatio_int_CallDebugHook(
		HORATIO_HOOK_DESTROYBLOCKARRAY, &debugHookInfo
	);
#endif /*HORATIO_DEBUGHOOKS*/
	/* Fire trap */
	sprintf(
		trapStr,
		#ifdef HAVE_SNPRINTF
		trapMsgRemaining,
		#endif /*HAVE_SNPRINTF*/
		"DestroyBlockArray(): "
		"Attempt to destroy unknown array (%s%p)!\n",
		HORATIO_FMTPTRPFX, (void*)PBlockArray
	);
	Trap(HORATIO_TRAP_BAD_BLOCK_ARRAY, trapStr);
	return;
}

static unsigned int horatio_int_IsDefaultBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
#ifdef HORATIO_NONULL_BLOCKDESCARRAY
	return 0; /* Default (NULL) array does not exist */
#else
	if (!PBlockArray || PBlockArray == &_defaultArray)
		return 1U; /* TRUE */

	return 0U; /* FALSE */
#endif
}
