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
#define HORATIO_SOURCE

/*
 * Main allocation function and block array grower
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
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
#include "hlog.h" /* Main logging support */
#include "hvptrap.h" /* _VerifyPtrs() */
#include "hdbghook.h" /* Debug hook executive and support functions */
#include "hbiglock.h" /* Mutual exclusion */
#include "halloc.h"

/*!
 * \brief Local logging function for the allocator
 *
 * \param File Source code filename
 * \param Line Source code line number
 * \param Severity The severity of the error, a higher numeric value is more serious
 * \param Message Message to be passed on to the logger
 */
static void OurLog(
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Message
);

/*!
 * \brief Grow the array by 'GrowByElems' elements
 *
 * \param PCurrentBlockArray Pointer to a block descriptor
 * \param GrowByElems number of elements by which the block should be enlarged.
 *
 * \return Boolean success value
 *
 * Returns a false value if it fails but then the original array is still valid and no
 * bigger.  Always make sure the array pointer is resolved, NULL pointers are not acceptable
 * and will be caught with assert(), with a checked build of the library.
 */

static unsigned int GrowBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PCurrentBlockArray,
	const unsigned int GrowByElems
);

#ifdef OURLOG /* Somebody else using OURLOG? */
# undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

/* Do the same paranoid check for OURLOG_POS */
#ifdef OURLOG_POS
# undef OURLOG_POS
#endif /*OURLOG_POS*/

/* Shortcut for typecast */
#define OURLOG(f, l, sev, msg) \
	OurLog((f), (l), ((const unsigned short)(sev)), (msg))

#define OURLOG_POS(sev, msg) \
	OURLOG(__FILE__, __LINE__, (sev), (msg))

/*!
 * Function which implements strdup
 *
 * \param PBlockArray Block descriptor array pointer
 * \param SrcStr Source string to be duplicated
 * \param File Source code filename information
 * \param Line Source code line number information
 *
 * \return Pointer to new string allocated by Horatio.
 *
 * This function should not be called directly, use the horatio_Strdup macro.
 */
char HORATIO_FARDATA *horatio_StrdupEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const char *SrcStr,
	const char *File,
	const unsigned int Line
) {
	char HORATIO_FARDATA *ret = horatio_int_AllocEx(
		PBlockArray,
		strlen(SrcStr)+1,
		File,
		Line
	);

	if ( ret ) strcpy(ret, SrcStr);

	return ret;
}

/*!
 * Function which wraps horatio_int_AllocEx for thread safety
 *
 * \param PBlockArray Block descriptor array pointer
 * \param NewBlockSize Size of block to be allocated by the library
 * \param File Source code filename information
 * \param Line Source code line number information
 *
 * \return Pointer to new memory allocated by Horatio.
 *
 * This function should not be called directly, use the horatio_Alloc macro.
 * This function itself does not implement allocations, but acquires the bigLock,
 * and then passes control to an internal allocator - hotatio_int_AllocEx, and
 * relinquishes the bigLock before returning to the caller.
 */
void HORATIO_FARDATA *horatio_AllocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
) {
	/* Thread safe wrapper for AllocEx() */
	void HORATIO_FARDATA *ret;

	LOCK
	ret = horatio_int_AllocEx(PBlockArray, NewBlockSize, File, Line);
	UNLOCK

	return ret;
}

void HORATIO_FARDATA *horatio_int_AllocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
) {
	/* locals */
	void HORATIO_FARDATA *genBlockPtr; /* Generated block pointer */
	char logMsg[MAX_TRAP_STRING_LENGTH + 1];
#ifdef HORATIO_DEBUGHOOKS
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
#endif /*HORATIO_DEBUGHOOKS*/
#ifdef HAVE_SNPRINTF
	size_t logMsgRemaining = MAX_TRAP_STRING_LENGTH;
#endif /*HAVE_SNPRINTF*/
	/* Resolving is done because of a possible NULL */
	PS_HORATIO_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray);

	/*
         *  Haults program if array not valid, third arg is not applicable here
	 */
	_VerifyPtrs("Alloc()", PBlockArray, NULL);

	sprintf(
		logMsg,
#ifdef HAVE_SNPRINTF
		logMsgRemaining,
#endif /*HAVE_SNPRINTF*/
		"Program Requested to allocate %u byte block for array %s%p",
		(unsigned int)NewBlockSize,
		HORATIO_FMTPTRPFX, (void*)PBlockArray
	);
#ifdef HAVE_SNPRINTF
	logMsgRemaining -= strlen(logMsg);
#endif /*HAVE_SNPRINTF*/
	OURLOG(File, Line, HORATIO_LOG_MESSAGE, logMsg);

	genBlockPtr = HORATIO_MALLOC(NewBlockSize); /* Allocate block */
	if (!genBlockPtr) { /* Out of memory? */
		/*
		 * Use buffer for log messages,
		 * it's the same size as for traps
		 */
		sprintf(
			logMsg,
#ifdef HAVE_SNPRINTF
			logMsgRemaining,
#endif /*HAVE_SNPRINTF*/
			"Attempt to allocate block of %u bytes "
			"for array at base %s%p has failed",
			(unsigned int)NewBlockSize,
			HORATIO_FMTPTRPFX, (void*)PBlockArray
		);
#ifdef HAVE_SNPRINTF
		logMsgRemaining -= strlen(logMsg);
#endif /*HAVE_SNPRINTF*/
		/*
		 * I haven't made this a warning because it can happen in a
		 * very legitimate situation where the caller may be prepared
		 * for a large allocation to handle
		 */
		OURLOG(File, Line, HORATIO_LOG_MESSAGE, logMsg);
		return NULL; /* No pointer generated */
	}

	/* Now add the block to the array, first grow array */
	if (!GrowBlockArray(PRArr, 1)) {
		/* Attempt to enlarge the array failed? */
		HORATIO_FREE(genBlockPtr); /* Release the new block of memory */

		sprintf(
			logMsg,
#ifdef HAVE_SNPRINTF
			logMsgRemaining,
#endif /*HAVE_SNPRINTF*/
			"Attempt to enlarge array at base %s%p "
			"by one element failed",
			HORATIO_FMTPTRPFX, (void*)PBlockArray
		);
#ifdef HAVE_SNPRINTF
		logMsgRemaining -= strlen(logMsg);
#endif /*HAVE_SNPRINTF*/
		/*
		 * This could be quite critical,
		 * if the memory manager is running our of space
		 */
		OURLOG_POS(HORATIO_LOG_WARNING, logMsg);
		return NULL; /* Give up */
	}

	/*
	 * Now the block's address can be added to the array,
	 * put pointer to base of block in block descriptor in the array,
	 * then, save size so caller can find it out later.
	 */
	PRArr->Descriptors[PRArr->Count-1].PBase = genBlockPtr;
	PRArr->Descriptors[PRArr->Count-1].Size = NewBlockSize;

	/* Version 1.1.4 changes, source file/line records */
	PRArr->Descriptors[PRArr->Count-1].SourceLine = Line;
	if ( File ) {
		PRArr->Descriptors[PRArr->Count-1].SourceFile
			= (char*)malloc((strlen(File)+1)*sizeof(char));

		if ( PRArr->Descriptors[PRArr->Count-1].SourceFile ) {
			strcpy(
				PRArr->Descriptors[PRArr->Count-1].SourceFile,
				File
			);
		}
	  }

	/* Update library statistics */
	horatio_int__blockCount++;
	horatio_int__allocCharge += NewBlockSize;

	/* Update peaks */
	if ( horatio_int__blockCount > horatio_int__blockCountPeak )
		horatio_int__blockCountPeak = horatio_int__blockCount;
	if ( horatio_int__allocCharge > horatio_int__allocPeak )
		horatio_int__allocPeak = horatio_int__allocCharge;

	/* Call the debug hook executive */
#ifdef HORATIO_DEBUGHOOKS
	/* Init structure */
	memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));
	/*
	 * Use resolved value, NULL means N/A, this is the only point at which
	 * the array pointer address is exposed to the caller directly
	 * (expect other debug hook calls of course)
	 */
	debugHookInfo.PRelArr = PRArr;
	debugHookInfo.PRelDesc = genBlockPtr;
	debugHookInfo.HookType = HORATIO_HOOK_ALLOC;
	debugHookInfo.AllocReq = (unsigned int)NewBlockSize;
	debugHookInfo.Success = 1U; /* TRUE */
	horatio_int_CallDebugHook(HORATIO_HOOK_ALLOC, &debugHookInfo);
#endif /*HORATIO_DEBUGHOOKS*/

	return genBlockPtr; /* Give pointer to the caller */
}

static unsigned int GrowBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PCurrentBlockArray,
	const unsigned int GrowByElems
) {
	/* Pointer to block descriptors during enlargement */
	PS_HORATIO_BLOCKDESCRIPTOR ptr;
	unsigned int oldCount; /* Count before enlargement */
	unsigned int initi; /* Initialization interator */

#ifdef NDEBUG /* Not in debug mode? */
	/* Just get out indicating error before disaster */
	if (!PCurrentBlockArray) return 0U;
#else /* Debug mode */
	assert(PCurrentBlockArray);
#endif /*NDEBUG*/

	if (!GrowByElems) { /* Want to grow by nothing? */
		OURLOG_POS(
			HORATIO_LOG_WARNING,
			"Attempt to GrowBlockArray() by no items, ignored"
		);
		return 1U;

		/*
		 * Success, already this size, it's great when there's
		 * nothing to do isn't it; Programmer's are lazy
		 */
	}

	/* Take count before we grow array */
	oldCount = PCurrentBlockArray->Count;
	ptr = HORATIO_REALLOC( /* Grow array */
		PCurrentBlockArray->Descriptors,
		(oldCount + GrowByElems) * sizeof(S_HORATIO_BLOCKDESCRIPTOR)
	);
	if (!ptr) /* Couldn't grow? */
		return 0U; /* Fail */

	/* Update array information */
	PCurrentBlockArray->Count += GrowByElems;
	/*
	 * Possible relocation might mean the pointer to the descriptors
	 * may need updating, make it so
	 */
	PCurrentBlockArray->Descriptors = ptr;

	/* Loop through All new descriptors in the array that we just created */
	for ( initi = oldCount; initi < PCurrentBlockArray->Count; initi++ ) {
		/* No block assigned to this new descriptor yet */
		PCurrentBlockArray->Descriptors[initi].PBase = NULL;

		/* Therefore no size either */
		PCurrentBlockArray->Descriptors[initi].Size = (size_t)0U;

		/* No flags set */
		PCurrentBlockArray->Descriptors[initi].Flags = 0U;

		/* No source line allocation */
		PCurrentBlockArray->Descriptors[initi].SourceLine = 0U;

		/* No known source file */
		PCurrentBlockArray->Descriptors[initi].SourceFile = NULL;
	}
	return 1U; /* Success */
}

static void OurLog(
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Str
) {
	/*
	 * Our job is to add "Alloc()" to the start of the string,
	 * which saves data space, if everybody in this module calls this
	 * instead of _Log() directly.
	 *
	 * We can't use LOG() twice because the information will be put on
	 * different lines, so a copy is needed.
	 */

	/* Valid string of at least on character sent to us? */
	if (Str && Str[0]) {
		char *PcopyStr;
		const char FuncName[] = "Alloc(): "; /* Prefix */

		/*
		 * Allocate space for copy, note that NULL termination
		 * is automagic because of using sizeof()
		 */
		PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) );
		if (PcopyStr) {
			strcpy(PcopyStr, FuncName); /* Prepend prefix */
			strcat(PcopyStr, Str); /* Add log string after prefix */

			horatio_int_Log(
				File, Line, Severity, PcopyStr
			); /* Pass on to the normal logger */

			free(PcopyStr); /* Copy can now be released */
		}
	}
	return;
}
