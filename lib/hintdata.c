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

/*! \file hintdata.c
 * \brief Internal data management
 *
 * This internal data is shared between the internal and user-called
 * functions in the memory manager library.  Initialisation of this data
 * is not done here, it is done within horatio.c : horatio_Startup().
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
#include "hintdata.h"

/*
 * List of pointers to arrays of block descriptors
 * (for validating array base pointers)
 */
PS_HORATIO_BLOCKDESCARRAY horatio_int__safetyList[HORATIO_SAFETYLIST_MAXSIZE];

#ifndef HORATIO_NONULL_BLOCKDESCARRAY /* NULL block array support enabled? */
/*
 * This is the built-in array, it is never created or destroyed but it's
 * contents must be what I'm setting them to now when the program ends!
 */
S_HORATIO_BLOCKDESCARRAY horatio_int__defaultArray
	= { 0U, NULL };

#endif /*!HORATIO_NONULL_BLOCKDESCARRAY*/

/* Set TRUE when library is started */
unsigned int horatio_int__libStarted = 0U;

/* The user trap handle must not be called by us unless is is a valid pointer */
void (*horatio_int__UserTrapCallback)(
	const unsigned int Id,
	const char *Message
) = NULL;

/* The trap callback is TRUE=a hook, FALSE=a handler */
unsigned int horatio_int__userTrapCallbackIsHook = 0U;

#ifdef HORATIO_DEBUGHOOKS
unsigned int (*horatio_int__debugHookMatrix[HORATIO_HOOKCHAIN_SIZE][HORATIO_DEBUGHOOK_LASTHOOK+1])(PS_HORATIO_DEBUGHOOKINFO PDebugHookInfo); /* Debug hook matrix */
#endif /*HORATIO_DEBUGHOOKS*/

/*!
 * \def S_HORATIO_OPTIONS
 * \brief Options currently in use
 *
 * TODO: Need to determine what locking is involved in accessing this.
 */
S_HORATIO_OPTIONS horatio_int__options;

/*
 * For statistics
 */
unsigned long horatio_int__blockCount = 0U;

/* The most BlockCount has been */
unsigned long horatio_int__blockCountPeak = 0U;

/* Allocation charge (amount allocated in all blocks) */
unsigned long horatio_int__allocCharge = 0U;

/* Allocation charge peak, maximum ever allocated */
unsigned long horatio_int__allocPeak = 0U;

/*! \brief horatio_int__unitTest.
 *
 *  Users must not set this flag to non-zero.
 *  Doing so will change the some documented library behavior for
 *  the convenience of the unit testing suite.
 */
int horatio_int__unitTest = 0;

/*! \brief horatio_int__aborts.
 *
 *  The number of times abort() would have been called if horatio_int__unitTest
 *  were zero.
 */
unsigned int horatio_int__aborts = 0U;

/*! \brief horatio_int__defHandlerCode.
 *
 *  The Id which was passed to the default trap handler,
 *  which aborts the program.  This is only useful during unit tests.
 */
unsigned int horatio_int__defHandlerCode = HORATIO_TRAP_UNKNOWN;
