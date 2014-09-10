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
 * \file IsBad....() routines for testing validity of pointers
 *
 * All function can cope with resolved or unresolved array pointers
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Access to internal data */
#include "htrap.h" /* Trap support */
#include "hsafelst.h" /* List of acceptable arrays */
#include "hbiglock.h" /* Mutual exclusion */
#include "hisbad.h"

unsigned int horatio_IsBadBlockPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
) {
	/* Thread safe wrapper for IsBadBlockPtr() */
	unsigned int ret;

	LOCK
	ret = horatio_int_IsBadBlockPtr(PBlockArray, BlockPtr);
	UNLOCK

	return ret;
}

unsigned int horatio_IsBadArrayPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	/* Thread safe wrapper for IsBadArrayPtr() */
	unsigned int ret;

	LOCK
	ret = horatio_int_IsBadArrayPtr(PBlockArray);
	UNLOCK

	return ret;
}

unsigned int horatio_int_IsBadBlockPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
) {
	/* locals */
	unsigned int i; /* List/loop control */
	PS_HORATIO_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray);

	/* Test for bad block array */
	if ( horatio_int_IsBadArrayPtr(PBlockArray) ) { /* Block array bad? */
		/* Fire a trap */
		char trapmsg[MAX_TRAP_STRING_LENGTH + sizeof(char)];

		sprintf(
			trapmsg,
#ifdef HAVE_SNPRINTF
			MAX_TRAP_STRING_LENGTH,
#endif /*HAVE_SNPRINTF*/
			"The block array address %s%p is unknown, "
			"unable to search for blocks.",
			HORATIO_FMTPTRPFX, (void*)PBlockArray
		);

		Trap(HORATIO_TRAP_BAD_BLOCK_ARRAY, trapmsg);
	}

	/* For all the block descriptors in the list */
	for ( i = 0U; i < PRArr->Count; i++ ) {
		if (BlockPtr == PRArr->Descriptors[i].PBase) {
			/* Block base match */
			return 0U; /* Block is not bad */
		}
	}
	/*
	 * The block was not matched in the loop above,
	 * therefore the block is bad or not in the specified array.
	 */
	return 1U; /* Yes, block is bad */
}

unsigned int horatio_int_IsBadArrayPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	/*
	 * I've rewritten this function so that is can support NULL arrays
	 * and it does not matter whether PBlockArray is resolved or not
	 */

#ifndef HORATIO_NONULL_BLOCKDESCARRAY /* Build supports NULL arrays? */
	/* NULL array (or resolved version) */
	if ( !PBlockArray || PBlockArray == &_defaultArray )
		return 0U; /* Not a bad array */
#endif /*!HORATIO_NONULL_BLOCKDESCARRAY*/

	/* Check the normal safety list */
	return !SafetyList_IsArrayPtrPresent(PBlockArray);
}

