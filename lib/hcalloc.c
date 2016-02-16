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

/*! \file hcalloc.c
 * \brief Functions related to the implementation of calloc()
 */

#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library data */
#include "hlog.h" /* Main logging support */
#include "hiblkptr.h" /* horatio_int_IndexFromBlockPtr() */
#include "hdbghook.h" /* Debug hook executive */
#include "hbiglock.h" /* For entire library mutual exclusion */
#include "halloc.h" /* Allows us to call AllocEx(), bipassing the big lock */

#ifdef HORATIO_LOG
static void OurLog(
  const unsigned short Code,
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Str
);
#endif /*HORATIO_LOG*/

#ifdef OURLOG /* Somebody else using OURLOG? */
# undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

#define OURLOG(lcode, f, l, sev, msg)                                              \
    OurLog((lcode), (f), (l), ((const unsigned short)(sev)), (msg))

static void HORATIO_FARDATA* horatio_int_CallocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const unsigned int N,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
);

void HORATIO_FARDATA *horatio_CallocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const unsigned int N,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
) {
	void HORATIO_FARDATA *ret;

	LOCK
	ret = horatio_int_CallocEx(PBlockArray, N, NewBlockSize, File, Line);
	UNLOCK

	return ret;
}

static void HORATIO_FARDATA *horatio_int_CallocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const unsigned int N,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
) {
	void HORATIO_FARDATA *resultantPtr;
#ifdef HORATIO_DEBUGHOOKS
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
#endif /*HORATIO_DEBUGHOOKS*/
#ifdef HORATIO_LOG
	char logMsg[MAX_TRAP_STRING_LENGTH+1];
#endif /*HORATIO_LOG*/

#ifdef HORATIO_LOG
	sprintf(
		logMsg,
		"Calloc() called, %u blocks of %u bytes requested, "
		"passing on to Alloc()",
		N,
		(unsigned int)NewBlockSize
	);
	OURLOG(HORATIO_LOG_CODE_CALLOC_REQ, File, Line, HORATIO_LOG_MESSAGE, logMsg);
#endif /*HORATIO_LOG*/

#ifdef HORATIO_DEBUGHOOKS
	debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
	debugHookInfo.HookType = HORATIO_HOOK_CALLOC;
	debugHookInfo.AllocReq = (N*NewBlockSize);
#endif /*HORATIO_DEBUGHOOKS*/

	resultantPtr = horatio_int_AllocEx(
		PBlockArray, (N*NewBlockSize), File, Line /* Call Alloc() */
	);
	if (resultantPtr) {
#ifdef HORATIO_DEBUGHOOKS
		/* I'll have to look up the descriptor for this block */
		unsigned int blkIndex = horatio_int_IndexFromBlockPtr(
			PBlockArray, resultantPtr
		);
		debugHookInfo.PRelDesc = &_ResolveArrayPtr(PBlockArray)
			->Descriptors[blkIndex];
		debugHookInfo.Success = 1U;
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_LOG
		OURLOG(
			HORATIO_LOG_CODE_CALLOC_DONE,
			File, Line, HORATIO_LOG_MESSAGE,
			"Allocation successful"
		);
#endif /*HORATIO_LOG*/

	/*
	 * Bug fix: I didn't realize this but the specification for for calloc()
	 * requires that the new memory is zeroed.
	 * Fix HORATIO Version 1.1.2 or 1.1.3
	 */
	memset(resultantPtr, 0, N*NewBlockSize);
	} else {
#ifdef HORATIO_DEBUGHOOKS
		/*blockDescArray.Success = 0U;   - optimized away */
#endif /*HORATIO_DEBUGHOOKS*/
#ifdef HORATIO_LOG
		OURLOG(HORATIO_LOG_CODE_CALLOC_FAIL, ile, Line, HORATIO_LOG_MESSAGE, "Allocation failed");
#endif /*HORATIO_LOG*/
	}

#ifdef HORATIO_DEBUGHOOKS
	horatio_int_CallDebugHook(HORATIO_HOOK_CALLOC, &debugHookInfo);
#endif /*HORATIO_DEBUGHOOKS*/
	return resultantPtr;
}

#ifdef HORATIO_LOG
static void OurLog(
  const unsigned short Code,
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Str
) {
	/*
	 * Our job is to add "Calloc()" to the start of the string,
	 * saves data space if everybody in this module calls this
	 * instead of _Log() directly.
	 *
	 * We can't call _Log() twice because the information will
	 * be put on different lines so a copy is needed.
	 */

	if (Str && Str[0]) {
		/* Valid string of at least on character sent to us? */
		char *PcopyStr;
		const char FuncName[] = "Calloc(): "; /* Prefix */
		/*
		 * Allocate space for copy.
		 * Note that NULL termination is automatic because of
		 * using sizeof()
		 */
		PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) );
		if (PcopyStr) {
			strcpy(PcopyStr, FuncName); /* Prepend prefix */
			strcat(PcopyStr, Str); /* Add log string after prefix */

			/* Pass on to the normal logger */
			horatio_int_Log(Code, File, Line, Severity, PcopyStr);

			free(PcopyStr); /* Copy can now be released */
		}
	}
	return;
}
#endif /*HORATIO_LOG*/
