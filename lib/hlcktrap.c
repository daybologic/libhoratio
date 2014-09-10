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
 * \file Block locking trap mechanism
 *
 * A trap I prepared earlier - fires trap on a locking violation, call this
 * at the begining of any function which invariably leads to block descriptors
 * being modified or deleted (except for flag changes).  This will return
 * FALSE normally.  If it returns TRUE it because the block is locked, the
 * caller should then make steps not to modify the block.  This TRUE return only
 * occurs when trapping is off or handled by the user who chose to ignore the
 * trap.  Otherwise the function does not return.
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
#include "hintdata.h" /* Internal library header */
#include "htrap.h" /* Trap() */
#include "hbloclck.h" /* For internal interfaces */
#include "hbdflags.h" /* For internal interface */
#include "hlcktrap.h"

unsigned int horatio_int_LockTrap(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
) {
	/* Block locked? */
	if (horatio_int_IsBlockLocked(PBlockArray, BlockPtr)) {
		char trapMsg[MAX_TRAP_STRING_LENGTH+1]; /* For this message */

		sprintf(
			trapMsg,
	#ifdef HAVE_SNPRINTF
			MAX_TRAP_STRING_LENGTH,
	#endif /*HAVE_SNPRINTF*/
			"%s: (locking violation) Block %s%p in "
			"array %s%p, flag dump: %X",
			FuncName,
			HORATIO_FMTPTRPFX, (void*)PBlockArray,
			HORATIO_FMTPTRPFX, BlockPtr,
			(unsigned short)horatio_int_ModifyDescriptorFlags(
				PBlockArray,
				BlockPtr,
				NULL
			)
		);
		/* Execute the trap */
		Trap(HORATIO_TRAP_LOCKINGVIOLATION, trapMsg);
		/*
		 * If the trap recovered let program go on and notify
		 * user that trap ocourred
		 */
		return 1U;
	}
	return 0U; /* Block not locked */
}
