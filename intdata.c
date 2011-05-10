/*
    DPCRTLMM Memory Management lib's internal data
    Copyright (C) 2000 David Duncan Ross Palmer, Daybo Logic.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


Contact me: Overlord@DayboLogic.co.uk
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/
#define DPCRTLMM_SOURCE
/* This internal data is shared between the internal and user-called
functions in the memory manager library. - Overlord David Duncan Ross
Palmer.  Oh, and init of this data is not done here, it is done within
StrtStop.C : dpcrtlmm_Startup().    Overlord@DayboLogic.co.uk

Modifications
-------------
4th May 2000 - Changed definition of _safetyList from S_GENLIST _safetyList
5th May 2000 - Disabled defintion of hook traps while they are turned off in standard builds
19th July 2000 - Added dpcrtlmm_int__defaultArray (the NULL array)
21st July 2000 - Just added DPCRTLMM_SOURCE at the top
27th July 2000 - Added dpcrtlmm_int__blockCount, it's used instead of a time consuming
		 loop through all blockarrays, for the call dpcrtlmm_GetBlockCount()
		 Added dpcrtlmm_int__blockCountPeak
		 Added dpcrtlmm_int__allocCharge & dpcrtlmm_int__allocPeak
*/
#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h"
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
