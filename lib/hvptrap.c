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

/*! \file hvptrap.c
 * \brief Pre-defined default traps, and internal pointer verification
 *
 * 19th July 2000: This module replaces bbatrap.c and bbptrap.c, which no
 * longer exist.  It's an internal library issue, prebuilt traps for invalid
 * pointers.  In version 1.0 through all the betas these two functions which
 * were always called at the same time where in two separate modules.
 * I decided it would be cleaner to move them into the same module. If there
 * is no block pointer in the array to check, pass NULL as arg three.
 *
 * Creation: 19th July 2000
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library data */
#include "htrap.h" /* Trap() */
#include "hlog.h" /* LOG macro */
#include "hsafelst.h" /* Safety list support functions */
#include "hisbad.h" /* Internal interface to isbad checkers */
#include "hvptrap.h"

/*
 * Functions only we can see
 */
static void TrapOnBadBlockArray(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray
);
static void TrapOnBadBlockPtr(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
);

void horatio_int_VerifyPtrs(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
) {
	/* CAUTION: Do not pass a resolved array pointer to this function */
	/*
	 * First trap invalid array pointers
	 */
	TrapOnBadBlockArray(FuncName, PBlockArray);
	if (BlockPtr) {
		/*
		 * Then trap invalid block pointers within the array
		 */
		TrapOnBadBlockPtr(FuncName, PBlockArray, BlockPtr);
	}
}

static void TrapOnBadBlockArray(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	/* locals */
	const char *const cTrapMsg0 =
		"%s: The array base ptr %s%p was not found in "
		"the internal safety list";

	char *dynMsg; /* Dynamically allocated message string */
	/* Calculate enough space for address and NULL (and more) */
	size_t dynMsgSize = ((FuncName) ? (strlen(FuncName)) : (0))
	    + strlen(cTrapMsg0) + 32;

	if ( !horatio_int_IsBadArrayPtr(PBlockArray) )
		return;

	/* The array base is bad */
	dynMsg = (char*)malloc(dynMsgSize);
	if (dynMsg) {
		sprintf(
			dynMsg,
#ifdef HAVE_SNPRINTF
			dynMsgSize-1,
#endif /*HAVE_SNPRINTF*/
			/*cTrapMsg0,*/
			/*
			 * TODO: This is a temporary workaround for a
			 * warning that we need to statically use this
			 * string, so that the compiler may validate
			 * it
			 */
			"%s: The array base ptr %s%p was not found in "
			"the internal safety list",
			(FuncName) ? (FuncName) : ("UNKNOWN"),
			HORATIO_FMTPTRPFX, PBlockArray
		);

		Trap(HORATIO_TRAP_BAD_BLOCK_ARRAY, dynMsg);
	} else { /* malloc() failed */
		if ( horatio__EnableTraps ) abort();
	}
}

static void TrapOnBadBlockPtr(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
) {
	char trapMsg[MAX_TRAP_STRING_LENGTH +1]; /* For trap message */
#ifdef HAVE_SNPRINTF
	size_t trapMsgRemaining = MAX_TRAP_STRING_LENGTH;
#endif /*HAVE_SNPRINTF*/

	if ( horatio_int_IsBadArrayPtr(PBlockArray) ) {
		/* The pointer to the array is invalid */
		char blankStr[] = "";
		/* PusedFuncName points to function name to use (not dynamic) */
		const char *PusedFuncName;

		if (FuncName) /* Function name sent? */
			PusedFuncName = FuncName; /* Use sent funtion name */
		else /* Funtion name not sent? */
			PusedFuncName = blankStr; /* Point to blank string */

		sprintf(
			trapMsg,
	#ifdef HAVE_SNPRINTF
			trapMsgRemaining,
	#endif /*HAVE_SNPRINTF*/
			"%s: The block pointer %s%p "
			"is not valid for array %s%p, "
			"cannot test block pointer validity.",
			PusedFuncName,
			HORATIO_FMTPTRPFX, BlockPtr,
			HORATIO_FMTPTRPFX, (void*)PBlockArray
		);

		Trap(HORATIO_TRAP_BAD_BLOCK, trapMsg);
	} /* Bad block array? */

	/*
	 * The array is a valid and acceptable pointer,
	 * pass on to IsBadBlockPtr() and if it's bad fire a trap.
	 */
	if ( horatio_int_IsBadBlockPtr( PBlockArray, BlockPtr ) ) {
		char *PusedFuncName;
		int pusedDynamic= 0;
		char blankStr[] = "";

		if (FuncName) {
			unsigned int lenUsedFuncName = strlen(FuncName);
			PusedFuncName = malloc( lenUsedFuncName +1 );
			if ( PusedFuncName ) {
				pusedDynamic = 1;
				PusedFuncName[0] = '\0';
				strncat(
					PusedFuncName,
					FuncName,
					lenUsedFuncName
				);
			}
		} else {
			PusedFuncName = blankStr;
		}

		sprintf(
			trapMsg,
			#ifdef HAVE_SNPRINTF
			trapMsgRemaining,
			#endif /*HAVE_SNPRINTF*/
			"%s: Bad block pointer: %s%p for array %s%p",
			PusedFuncName,
			HORATIO_FMTPTRPFX, BlockPtr,
			HORATIO_FMTPTRPFX, (void *)PBlockArray
		);

		Trap(HORATIO_TRAP_BAD_BLOCK, trapMsg); /* Fire trap */
		if ( pusedDynamic )
			free(PusedFuncName);
	}
	return;
}
