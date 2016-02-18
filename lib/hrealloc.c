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

/*! \file hrealloc.c
 * \brief Implementation of realloc()
 *
 * HORATIO's memory user-memory reallocation
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdlib.h>
#include <stdio.h>

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
#include "hintdata.h" /* Internal library data */
#include "hvptrap.h" /* _VerifyPtrs() */
#include "hlcktrap.h" /* _LockTrap() */
#include "hiblkptr.h"
#include "hdbghook.h" /* Debug hooking routines, we use the executive */
#include "hlog.h"
#include "hbiglock.h" /* Mutual exclusion */

static void HORATIO_FARDATA *horatio_int_Realloc(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	void HORATIO_FARDATA *OldBlockPtr,
	const size_t NewSize
);

void HORATIO_FARDATA *horatio_Realloc(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	void HORATIO_FARDATA *OldBlockPtr,
	const size_t NewSize
) {
	void HORATIO_FARDATA *ret;

	LOCK
	ret = horatio_int_Realloc(PBlockArray, OldBlockPtr, NewSize);
	UNLOCK

	return ret;
}

static void HORATIO_FARDATA *horatio_int_Realloc(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	void HORATIO_FARDATA *OldBlockPtr,
	const size_t NewSize
) {
	/* ptr is returned to the caller (modified later) */
	void HORATIO_FARDATA *ptr = OldBlockPtr;
	const char funcName[] = "Realloc()"; /* Name of our function */
	unsigned int blockIndex; /* Index of block in the array */
	char HORATIO_FARDATA* sizePtr; /* Pointer used during resizing */
	PS_HORATIO_BLOCKDESCARRAY PRArr;
#ifdef HORATIO_DEBUGHOOKS
	/* Used for advanced debug hooks! */
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
#endif /*HORATIO_DEBUGHOOKS*/

	PRArr = _ResolveArrayPtr(PBlockArray);
	/* Do trap if passed pointers are invalid */
	_VerifyPtrs(funcName, PBlockArray, OldBlockPtr);
	/* Do trap if block is locked */
	if (_LockTrap(funcName, PBlockArray, OldBlockPtr))
		return NULL;

	if (!NewSize) { /* No new size, hmm, must be wanting free() really */
		WARNING(
			"Dynamic possibly non-portable use of realloc() "
			"as a free-er"
		);
		/* Give the caller what they want */
		horatio_Free(PBlockArray, OldBlockPtr);
		return NULL;
	}

	if ( !OldBlockPtr ) {
		/*
		 * This is a non-portable attempt to use realloc
		 * as an initial allocator
		 */
		WARNING(
			"Dynamic possibly non-portable use of realloc() "
			"as an initial allocator"
		);
		return horatio_Alloc(PBlockArray, NewSize);
	}

	blockIndex = horatio_int_IndexFromBlockPtr(PRArr, OldBlockPtr);
	if ( PRArr->Descriptors[blockIndex].Size == NewSize ) {
		/*
		 * The block is already the requested size!
		 * Give current pointer back to caller wihout touching it
		 */
		return ptr;
	}
	/* The resize is valid */

	/* Attempt to resize the block */
	sizePtr = HORATIO_REALLOC( OldBlockPtr, NewSize );
	/*
	 * If the block cannot be enlarged return NULL to the caller
	 * to indicate the failure
	 */
	if (!sizePtr) return NULL;

#ifdef HORATIO_DEBUGHOOKS
	/* Call debug hooks */
	debugHookInfo.PRelArr = PRArr;

	/* Do NOT dereference, base may have changed */
	debugHookInfo.PRelDesc = OldBlockPtr;

	debugHookInfo.HookType = HORATIO_HOOK_REALLOC;

	/* Set AllocReq to size difference */
	debugHookInfo.AllocReq = HORATIO_MAX(
		PRArr->Descriptors[blockIndex].Size, NewSize
	) - HORATIO_MIN(
		PRArr->Descriptors[blockIndex].Size, NewSize
	);
	if ( NewSize < PRArr->Descriptors[blockIndex].Size ) {
		/* Negate number */
		debugHookInfo.Misc0 |= 1; /* Set bit 0 */
	} else { /* Positive number */
		debugHookInfo.Misc0 &= ~1; /* Clear bit 0 */
	}
	/*
	 * Misc1 points to the new block, the hook routine can
	 * dereference it if it wants to
	 */

	/* Hook routine should cast it back to void* */
	debugHookInfo.Misc1 = (unsigned long)sizePtr;
	horatio_int_CallDebugHook(HORATIO_HOOK_REALLOC, &debugHookInfo);
#endif /*HORATIO_DEBUGHOOKS*/

	ptr = sizePtr; /* Set new pointer as the return value */
	/* Update block descriptor */
	PRArr->Descriptors[blockIndex].PBase = ptr;
	PRArr->Descriptors[blockIndex].Size = NewSize;

	return ptr; /* Give new pointer to caller */
}
