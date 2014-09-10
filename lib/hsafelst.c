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
 * \file Functions for handling the safety list
 *
 * No functions here are for calling by the user, they are only for internal
 * library use.
 *
 * Created May 3rd 2000
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
#include "hintdata.h" /* Internal library data */
#include "hsafelst.h"

unsigned int horatio_SafetyList_IsArrayPtrPresent(
	const PS_HORATIO_BLOCKDESCARRAY ArrayBase
) {
	if (ArrayBase) {
		unsigned int sli; /* Used for processing of the list */
		for ( sli = 0U; sli < HORATIO_SAFETYLIST_MAXSIZE; sli++ )
			if (_safetyList[sli] == ArrayBase) return 1U;
	}
	return 0U; /* Not present or bad params */
}

void horatio_SafetyList_Init() {
	unsigned int i;

	/* All possible entries in list */
	for ( i = 0U; i < HORATIO_SAFETYLIST_MAXSIZE; i++ )
		_safetyList[i] = NULL; /* Zero pointer */

	return;
}

unsigned int horatio_SafetyList_CountUsed() {
	unsigned int sli, slc = 0U;

	for ( sli = 0U; sli < HORATIO_SAFETYLIST_MAXSIZE; sli++ ) {
		if (_safetyList[sli]) /* Valid pointer in list? */
			slc++; /* Increment count */
	}

	return slc; /* Give count to caller */
}

unsigned int horatio_SafetyList_AddBase(
	const PS_HORATIO_BLOCKDESCARRAY PArrayBase
) {
	unsigned int sli;

	/*
	 * Sorry, this slows things down, must check to see if already here
	 * otherwise it would cause false leaks later.
	 */
	if (SafetyList_IsArrayPtrPresent(PArrayBase)) return 0U;

	for ( sli = 0U; sli < HORATIO_SAFETYLIST_MAXSIZE; sli++ ) {
		if (!_safetyList[sli]) { /* First free entry? */
			/* Store base in safety list */
			_safetyList[sli] = PArrayBase;
			return 1U; /* Done, abort loop, report success */
		}
	}

	return 0U; /* Damn, no free entries */

	/*
	 * If you do find that AddBase() fails increase the fixed list size,
	 * the definition can be found in build.h and then rebuild the library,
	 * or cut down on the number of block arrays used in the program,
	 * this involves quite a bit of restructuring of the program but can
	 * reduce overhead, the other side effect is to make if more difficult
	 * to localize a leak, however.
	 */
}

unsigned int horatio_SafetyList_RemoveBase(
	const PS_HORATIO_BLOCKDESCARRAY PArrayBase
) {
	unsigned int sli;

	/*
	 * This is faster than adding because we have no need for a separate
	 * search, if it is not found the function fails, it is is the entry
	 * is set to NULL, which is an effective removal.
	 */

	for ( sli = 0U; sli < HORATIO_SAFETYLIST_MAXSIZE; sli++ ) {
		if (_safetyList[sli] == PArrayBase) {
			/*
			 * Found the base in question.  Mark entry in the list
			 * as unused.
			 */
			_safetyList[sli] = NULL;
			return 1U; /* Successfully removed */
		}
	}
	return 0U; /* Looks like the item was not present */
}
