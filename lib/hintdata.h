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
#ifndef INC_HORATIO_INTDATA_H
#define INC_HORATIO_INTDATA_H

/*! \file hintdata.h
 * \brief Internal data header
 *
 * WARNING! This header is for internal library use only, it contains
 * internal data for the library.
 */

#include "hbuild.h" /* Build parameters */

/* Internal library data */

/*
 * List of pointers to arrays of block descriptors
 * (for validating array base pointers)
 */
extern PS_HORATIO_BLOCKDESCARRAY horatio_int__safetyList[HORATIO_SAFETYLIST_MAXSIZE];
#ifndef HORATIO_NONULL_BLOCKDESCARRAY /* New NULL block array support enabled? */
/*
 * Always here, never created, never destroyed accessable by specifying
 * NULL for an array pointer
 */
  extern S_HORATIO_BLOCKDESCARRAY horatio_int__defaultArray;
#endif /*!HORATIO_NONULL_BLOCKDESCARRAY*/

/* Set horatio_int__libStarted to TRUE when library is started */
extern unsigned int horatio_int__libStarted;

/* The user trap handle must not be called by us unless is is a valid pointer */
extern void (*horatio_int__UserTrapCallback)(
	const unsigned int Id,
	const char *Message
);

/*
 * Indication as to the manner in which the user trap callback expects
 * to be treated, as a hook or as a handler
 */
extern unsigned int horatio_int__userTrapCallbackIsHook;
#ifdef HORATIO_DEBUGHOOKS
/*
 * Debug hook matrix (sizeof(a hook) * hook types * maximum hook chain length)
 */
extern unsigned int (*horatio_int__debugHookMatrix[HORATIO_HOOKCHAIN_SIZE][HORATIO_DEBUGHOOK_LASTHOOK+1])(PS_HORATIO_DEBUGHOOKINFO PDebugHookInfo);

extern S_HORATIO_OPTIONS horatio_int__options;

#endif /*HORATIO_DEBUGHOOKS*/
#ifdef HORATIO_NONULL_BLOCKDESCARRAY
# define horatio_int__ResolveArrayPtr(p) (p)  /* Resolving not neccersary, it's for the NULL array */
#else
# define horatio_int__ResolveArrayPtr(p) ((p) ? (p) : (&horatio_int__defaultArray))
#endif /*HORATIO_NONULL_BLOCKDESCARRAY*/
/* Statistics */
extern unsigned long horatio_int__blockCount;
extern unsigned long horatio_int__blockCountPeak;
extern unsigned long horatio_int__allocCharge;
extern unsigned long horatio_int__allocPeak;

/* Magic lazy references for library's own use */
#define _safetyList horatio_int__safetyList
#ifndef HORATIO_NONULL_BLOCKDESCARRAY
# define _defaultArray horatio_int__defaultArray
#endif /*!HORATIO_NONULL_BLOCKDESCARRAY*/
#define _libStarted horatio_int__libStarted
#define _UserTrapCallback horatio_int__UserTrapCallback
#define _userTrapCallbackIsHook horatio_int__userTrapCallbackIsHook
#define _debugHookMatrix horatio_int__debugHookMatrix
#define _ResolveArrayPtr horatio_int__ResolveArrayPtr
#define _blockCount horatio_int__blockCount
#define _blockCountPeak horatio_int__blockCountPeak
#define _allocCharge horatio_int__allocCharge
#define _allocPeak horatio_int__allocPeak
#define _options horatio_int__options

#endif /*!INC_HORATIO_INTDATA_H*/
