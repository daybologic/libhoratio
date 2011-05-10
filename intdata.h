/*
    DPCRTLMM Memory management lib - internal data header
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
#ifndef __INC_DPCRTLMM_INTDATA_H
#define __INC_DPCRTLMM_INTDATA_H
/*-------------------------------------------------------------------------*/
/* WARNING! This header is for internal library use only, it contains
internal data for the library. */

#include "build.h" /* Build parameters */

/* Internal library data */
extern PS_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_int__safetyList[DPCRTLMM_SAFETYLIST_MAXSIZE]; /* List of pointers to arrays of block descriptors (for validating array base pointers) */
#ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY /* New NULL block array support enabled? */
  extern S_DPCRTLMM_BLOCKDESCARRAY dpcrtlmm_int__defaultArray; /* Always here, never created, never destroyed accessable by specifying NULL for an array pointer */
#endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/
extern unsigned int dpcrtlmm_int__libStarted; /* Set TRUE when library is started */
extern void (*dpcrtlmm_int__UserTrapCallback)(const unsigned int Id, const char* Message); /* The user trap handle must not be called by us unless is is a valid pointer */
extern unsigned int dpcrtlmm_int__userTrapCallbackIsHook; /* Indication as to the manner in which the user trap callback expects to be treated, as a hook or as a handler */
#ifdef DPCRTLMM_DEBUGHOOKS
extern unsigned int (*dpcrtlmm_int__debugHookMatrix[DPCRTLMM_HOOKCHAIN_SIZE][DPCRTLMM_DEBUGHOOK_LASTHOOK+1])(PS_DPCRTLMM_DEBUGHOOKINFO PDebugHookInfo); /* Debug hook matrix (sizeof(a hook) * hook types * maximum hook chain length) */
#endif /*DPCRTLMM_DEBUGHOOKS*/
#ifdef DPCRTLMM_NONULL_BLOCKDESCARRAY
# define dpcrtlmm_int__ResolveArrayPtr(p) (p)  /* Resolving not neccersary, it's for the NULL array */
#else
# define dpcrtlmm_int__ResolveArrayPtr(p) ((p) ? (p) : (&dpcrtlmm_int__defaultArray))
#endif /*DPCRTLMM_NONULL_BLOCKDESCARRAY*/
/* Statistics */
extern unsigned long dpcrtlmm_int__blockCount;
extern unsigned long dpcrtlmm_int__blockCountPeak;
extern unsigned long dpcrtlmm_int__allocCharge;
extern unsigned long dpcrtlmm_int__allocPeak;

/* Magic lazy references for library's own use */
#define _safetyList dpcrtlmm_int__safetyList
#ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY
#  define _defaultArray dpcrtlmm_int__defaultArray
#endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/
#define _libStarted dpcrtlmm_int__libStarted
#define _UserTrapCallback dpcrtlmm_int__UserTrapCallback
#define _userTrapCallbackIsHook dpcrtlmm_int__userTrapCallbackIsHook
#define _debugHookMatrix dpcrtlmm_int__debugHookMatrix
#define _ResolveArrayPtr dpcrtlmm_int__ResolveArrayPtr
#define _blockCount dpcrtlmm_int__blockCount
#define _blockCountPeak dpcrtlmm_int__blockCountPeak
#define _allocCharge dpcrtlmm_int__allocCharge
#define _allocPeak dpcrtlmm_int__allocPeak
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_H*/
