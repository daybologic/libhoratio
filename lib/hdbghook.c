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

/*! \file hdbghook.c
 * \brief The debug hook executive and supporting code
 *
 * Created 24th February 2000
 */

#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library data */
#include "hlog.h" /* LOG macro */
#include "hbiglock.h" /* Mutual exclusion */
#include "hdbghook.h"

#ifdef HORATIO_DEBUGHOOKS

static unsigned int BadHookType(
	const unsigned int HookType
);

static unsigned int horatio_int_InstallDebugHook(
	const unsigned short HookType,
	unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
);

static unsigned int horatio_int_GetDebugHookChainCount(
	const unsigned int HookType
);

static unsigned int horatio_int_GetDebugHookMatrixCount(
	void
);

static unsigned int horatio_int_UninstallDebugHook(
	const unsigned short HookType,
	unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
);

#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
unsigned int horatio_InstallDebugHook(
	const unsigned short HookType,
	unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
) {
	unsigned int ret;

	LOCK
	ret = horatio_int_InstallDebugHook(HookType, NewHookProc);
	UNLOCK

	return ret;
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
unsigned int horatio_GetDebugHookChainCount(
	const unsigned int HookType
) {
	unsigned int ret;

	LOCK
	ret = horatio_int_GetDebugHookChainCount(HookType);
	UNLOCK

	return ret;
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
unsigned int horatio_GetDebugHookMatrixCount() {
	unsigned int ret;

	LOCK
	ret = horatio_int_GetDebugHookMatrixCount();
	UNLOCK

	return ret;
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
unsigned int horatio_UninstallDebugHook(
	const unsigned short HookType,
	unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
) {
	unsigned int ret;

	LOCK
	ret = horatio_int_UninstallDebugHook(HookType, HookProc2Remove);
	UNLOCK

	return ret;
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
void horatio_int_InitDebugHookMatrix() {
	/* Initialize or clear the debug hook matrix */
	unsigned int chainI; /* Used during initialization of chains loop */

	for ( chainI = 0U; chainI < HORATIO_HOOKCHAIN_SIZE; chainI++ ) {
		/* Nested loop to process chains for other hook types */
		unsigned int hookTypeI;

		for (
			hookTypeI = 0U;
			hookTypeI < HORATIO_DEBUGHOOK_LASTHOOK+1;
			hookTypeI++
		) {
			unsigned int (*NULLHookPtr)(PS_HORATIO_DEBUGHOOKINFO)
				= NULL; /* Make NULL pointer */

			horatio_int__debugHookMatrix[
				(size_t)chainI][(size_t)hookTypeI
			] = NULLHookPtr; /* Init element */
		}
	}
	return;
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
void horatio_int_CallDebugHook(
	const unsigned short HookType,
	const PS_HORATIO_DEBUGHOOKINFO PDebugHookInfo
) {
	/* locals */

	/* Local copy of caller's stuff */
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;

	/* Used to control processing of all the hooks loop */
	unsigned int allHooksLoop;

	/*
	 * We must copy the debug hook information from the caller so that hook
	 * functions may change information before it is passed on to the next
	 * hook function, if they so desire, without damanging the person who
	 * called the trap executive (incase they use the original pointers
	 * somehow), though I can't think how they can offhand, still...
	 */

	if (BadHookType(HookType)) {
		/* Bad hook type (out of range in matrix) */
		ERROR(
			HORATIO_LOG_CODE_HOOK_RANGE,
			"CallDebugHook: Internal library error, "
			"HookType out of range!"
		);
		return;
	}

	debugHookInfo = *PDebugHookInfo; /* Copy details by auto C deref */

	for (
		allHooksLoop = 0U;
		allHooksLoop < HORATIO_HOOKCHAIN_SIZE;
		allHooksLoop++
	) { /* All viable hook locations in the chain */

	/* Get hook function pointer from chain */
	unsigned int (*HookProc)(PS_HORATIO_DEBUGHOOKINFO)
		= _debugHookMatrix[allHooksLoop][HookType];

	if ( !HookProc ) /* No hook info */
		continue; /* Move onto next hook pointer */

	if ( !HookProc(&debugHookInfo) ) /* Call hook procedure */
		/*
		 * Hook requested not to pass information
		 * on to following hooks
		 */
		break;
	}
	return; /* All hook calls are done */
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
static unsigned int horatio_int_InstallDebugHook(
	const unsigned short HookType,
	unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
) {
	/* This function has added support for HORATIO_HOOK_ALL */
	unsigned int i; /* looping */
	unsigned int set = 0U; /* set = FALSE */

	if (HookType != HORATIO_HOOK_ALL) {
		/* Specific hook, not general hook; ensure type is valid */
		if (BadHookType(HookType)) return 0U;

		/* Find the first free entry in the chain */
		for ( i = 0U; i < HORATIO_HOOKCHAIN_SIZE; i++ ) {
			if ( !_debugHookMatrix[i][HookType] ) {
				/* Found free entry; Install hook proc */
				_debugHookMatrix[i][HookType] = NewHookProc;
				/*
				 * Remember at least one hook was installed:
				 * set 'set' TRUE
				 */
				set = 1U;
				break; /* Don't keep looping */
			}
		}
	} else { /* General hook that wants everything! */
		unsigned short nextHook;

		for (
			nextHook = (unsigned short)0x0000U;
			nextHook < HORATIO_DEBUGHOOK_LASTHOOK;
			nextHook++
		) { /* Go through all valid hook types */
			/* Call ourselves back to sort it out */
			if (!horatio_InstallDebugHook(nextHook, NewHookProc)) {
				/*
				 * Failed to install a hook?  Remove all of
				 * the hooks which are for this address
				 */
				horatio_UninstallDebugHook(
					HORATIO_HOOK_ALL, NewHookProc
				);
				/*
				 * Report failure for the whole lot,
				 * FALSE return
				 */
				return 0U;
			}
			set = 1U; /* Report success, in a while */
		}
	}

	return set; /* No space for handler */
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
static unsigned int horatio_int_GetDebugHookChainCount(
	const unsigned int HookType
) {
	unsigned int i;
	unsigned total = 0U;

	if (!BadHookType(HookType)) {
		/* Loop through all hook positions */
		for ( i = 0U; i < HORATIO_HOOKCHAIN_SIZE; i++ ) {
			if ( _debugHookMatrix[i][HookType] ) {
				/* Hook installed at this point in the chain? */
				total++; /* Increment count */
			}
		}
	}
	return total;
}
#endif /*HORATIO_DEBUGHOOKS*/

static unsigned int horatio_int_GetDebugHookMatrixCount(void) {
	unsigned int i;
	unsigned total = 0U;

	/* Loop through all types of hooks */
	for ( i = 0U; i <= HORATIO_DEBUGHOOK_LASTHOOK; i++ ) {
		/* Add chain contents to total for all chains */
		total += horatio_int_GetDebugHookChainCount(i);
	}

	return total; /* Give total to caller */
}

#ifdef HORATIO_DEBUGHOOKS
static unsigned int horatio_int_UninstallDebugHook(
	const unsigned short HookType,
	unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
) {
	/* This function has added support for HORATIO_HOOK_ALL */

	unsigned int i;
	unsigned int retStatus = 0U; /* Return status FALSE by default */

	if (HookType != HORATIO_HOOK_ALL) { /* Specific hook type request */
		if (BadHookType(HookType)) return 0U;

		/* Process all entries in the chain */
		for ( i = 0U; i < HORATIO_DEBUGHOOK_LASTHOOK; i++ ) {
			if (_debugHookMatrix[i][HookType] == HookProc2Remove) {
				/* Found entry */
				retStatus = 1U; /* We found it! Return TRUE */
				/* Delete address of hook proc */
				_debugHookMatrix[i][HookType] = NULL;
				/*
				 * Not breaking the loop so we can remove
				 * duplicates too, say for example the user
				 * installed the same hook proc twice for the
				 * same type
				 */
			}
		}
	} else { /* HookType is general */
		unsigned short si; /* Used for loop */
		retStatus = 1U; /* We always say success */

		for (
			si = (unsigned short)(0x0000U);
			si <= HORATIO_DEBUGHOOK_LASTHOOK;
			si++
		) /* All possible debug hook types */
			horatio_UninstallDebugHook(
				si,
				HookProc2Remove
			); /* Uninstall this hook from this type */
	  }

	  return retStatus; /* Give status to caller */
}
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_DEBUGHOOKS
static unsigned int BadHookType(const unsigned int HookType) {
	unsigned int bad = 0U; /* Not a bad hook type yet... */

# ifndef NDEBUG /* Debugging lib */
	/* Check hook type is valid */
	assert(HookType <= HORATIO_DEBUGHOOK_LASTHOOK);

# else /* Not in debug mode, must handle this same trap differenty */
	/* Check hook type is valid */
	if ( !(HookType <= HORATIO_DEBUGHOOK_LASTHOOK) )
		bad = 1U; /* bad = TRUE */

# endif /*!NDEBUG*/

	return bad;
}
#endif /*HORATIO_DEBUGHOOKS*/
