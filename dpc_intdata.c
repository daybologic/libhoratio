/*
Horatio's Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer (M6KVM), Daybo Logic
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

/*
  This internal data is shared between the internal and user-called
  functions in the memory manager library. - Overlord David Duncan Ross
  Palmer.  Oh, and init of this data is not done here, it is done within
  dpcrtlmm.c : dpcrtlmm_Startup().
*/

#define DPCRTLMM_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "dpc_intdata.h"
/*-------------------------------------------------------------------------*/
PS_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_int__safetyList[DPCRTLMM_SAFETYLIST_MAXSIZE]; /* List of pointers to arrays of block descriptors (for validating array base pointers) */
#ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY /* New NULL block array support enabled? */
  S_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_int__defaultArray = { 0U, NULL }; /* This is the built-in array, it is never created or destroyed but it's contents must be what I'm setting them to now when the program ends! */
#endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/
unsigned int dpcrtlmm_int__libStarted = 0U; /* Set TRUE when library is started */
void (*dpcrtlmm_int__UserTrapCallback)(const unsigned int Id, const char* Message) = NULL; /* The user trap handle must not be called by us unless is is a valid pointer */
unsigned int dpcrtlmm_int__userTrapCallbackIsHook = 0U; /* The trap callback is TRUE=a hook, FALSE=a handler */
#ifdef DPCRTLMM_DEBUGHOOKS
unsigned int (*dpcrtlmm_int__debugHookMatrix[DPCRTLMM_HOOKCHAIN_SIZE][DPCRTLMM_DEBUGHOOK_LASTHOOK+1])(PS_DPCRTLMM_DEBUGHOOKINFO PDebugHookInfo); /* Debug hook matrix */
#endif /*DPCRTLMM_DEBUGHOOKS*/
/* For statistics */
unsigned long dpcrtlmm_int__blockCount = 0U;
unsigned long dpcrtlmm_int__blockCountPeak = 0U; /* The most BlockCount has been */
unsigned long dpcrtlmm_int__allocCharge = 0U; /* Allocation charge (amount allocated in all blocks) */
unsigned long dpcrtlmm_int__allocPeak = 0U; /* Allocation charge peak, maximum ever allocated */
