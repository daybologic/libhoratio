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

/*! \file hbloclck.c
 * \brief Module for handling memory block lock flags,
 *
 * Created 22nd Feb 2000
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>
#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* The main library header */
#include "hbiglock.h" /* Mutual exclusion */
#include "hbdflags.h" /* Need this to get around the lock */
#include "hbloclck.h"

void horatio_SetBlockLockingFlag(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr,
	const unsigned int NewStatus
) {
	LOCK
	horatio_int_SetBlockLockingFlag(PBlockArray, Ptr, NewStatus);
	UNLOCK
}

unsigned int horatio_IsBlockLocked(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr
) {
	unsigned int ret;

	LOCK
	ret = horatio_int_IsBlockLocked(PBlockArray, Ptr);
	UNLOCK

	return ret;
}

void horatio_ToggleBlockLockingStatus(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr
) {
	LOCK
	horatio_int_ToggleBlockLockingStatus(PBlockArray, Ptr);
	UNLOCK
}

void horatio_int_SetBlockLockingFlag(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr,
	const unsigned int NewStatus
) {
	unsigned char flags;

	/* Get current flags */
	flags = horatio_int_ModifyDescriptorFlags(PBlockArray, Ptr, NULL);
	if (NewStatus) /* Locking? */
		flags |= 1; /* Set lock bit */
	else /* Unlocking? */
		flags |= ~1; /* Clear lock bit */

	/* Set the new flags */
	horatio_int_ModifyDescriptorFlags(PBlockArray, Ptr, &flags);
	return; /* That was simple enough, I can drink some water now */
}

unsigned int horatio_int_IsBlockLocked(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr
) {
	unsigned char flags;

	/* Get the flags for the descriptor */
	flags = horatio_int_ModifyDescriptorFlags(PBlockArray, Ptr, NULL);
	if ( ((flags & 1) == 1) ) /* The lock bit is set? */
		return 1U; /* Yes, the block is locked */

	return 0U; /* No, the block is not locked */
}

void horatio_int_ToggleBlockLockingStatus(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr
) {
	/* Get current status */
	unsigned int oldLockStat = horatio_int_IsBlockLocked(PBlockArray, Ptr);
	/* Set locking state as NOT current locking state */
	horatio_int_SetBlockLockingFlag(PBlockArray, Ptr, !oldLockStat);
}
