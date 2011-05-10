/*
    DPCRTLMM Memory Management Library : Block validity tests
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
Get updates: http://daybologic.com/Dev/dpcrtlmm
My official site: http://daybologic.com/overlord
*/
#define DPCRTLMM_SOURCE
/*
#############################################################################
# IsBad....() routines for testing validity of pointers, just a small part  #
# of one of my favourite libraries DPCRTLMM, heh heh.                       #
#############################################################################

  NOTE: 28th July 2000
  Both function can now cope with resolved or unresolved array pointers
*/
#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Access to internal data */
#include "trap.h" /* Trap support */
#include "safelist.h" /* List of acceptable arrays */
#include "biglock.h" /* Mutual exclusion */
/*-------------------------------------------------------------------------*/
static unsigned int dpcrtlmm_int_IsBadBlockPtr(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr);
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsBadBlockPtr(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr)
{
  /* Thread safe wrapper for IsBadBlockPtr() */
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_IsBadBlockPtr(PBlockArray, BlockPtr);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static unsigned int dpcrtlmm_int_IsBadBlockPtr(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr)
{
  /* locals */
  unsigned int i; /* List/loop control */
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr = _ResolveArrayPtr(PBlockArray);

  /* Test for bad block array */
  if ( dpcrtlmm_IsBadArrayPtr(PBlockArray) ) /* Block array bad? */
  {
    /* Fire a trap */
    char trapmsg[MAX_TRAP_STRING_LENGTH + sizeof(char)];

    sprintf(trapmsg, "The block array address 0x%p is unknown, unable to search for blocks.",
	    PBlockArray
    );

    _Trap(DPCRTLMM_TRAP_BAD_BLOCK_ARRAY, trapmsg);
  }

  for ( i = 0U; i < PRArr->Count; i++ ) /* For all the block descriptors in the list */
  {
    if (BlockPtr == PRArr->Descriptors[i].PBase) /* Block base match */
      return 0U; /* Block is not bad */
  }
  /* The block was not matched in the loop above, therefore the block
  is bad or not in the specified array. */
  return 1U; /* Yes, block is bad */
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsBadArrayPtr( const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray )
{
  /* I've rewritten this function so that is can support NULL arrays
  and it does not matter whether PBlockArray is resolved or not */

  #ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY /* Build supports NULL arrays? */
  if ( !PBlockArray || PBlockArray == &_defaultArray ) /* NULL array (or resolved version) */
    return 0U; /* Not a bad array */
  #endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/

  /* Check the normal safety list */
  return !SafetyList_IsArrayPtrPresent(PBlockArray);
}
/*-------------------------------------------------------------------------*/
