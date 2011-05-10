/*
    DPCRTLMM Memory Management Library : Internal safety list control
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
# Functions for handling the safety list                                    #
# DPCRTLMM--The DayboLogic C-Runtime Memory Manager                         #
# No functions here are for calling by the user, they are only for internal #
# library use.                                                              #
#############################################################################
Written: May 3rd 2000
Last modified 21st July 2000 by OverlordDDRP
*/

#include <stddef.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "safelist.h"
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_SafetyList_IsArrayPtrPresent(const PS_DPCRTLMM_BLOCKDESCARRAY ArrayBase)
{
  if (ArrayBase)
  {
	 unsigned int sli; /* Used for processing of the list */

	 for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ )
	 {
		if (_safetyList[sli] == ArrayBase) return 1U;
	 }
  }
  return 0U; /* Not present or bad params */
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_SafetyList_Init()
{
  unsigned int i;

  for ( i = 0U; i < DPCRTLMM_SAFETYLIST_MAXSIZE; i++ ) /* All possible entries in list */
	 _safetyList[i] = NULL; /* Zero pointer */

  return;
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_SafetyList_CountUsed()
{
  unsigned int sli, slc = 0U;

  for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ )
  {
	 if (_safetyList[sli]) /* Valid pointer in list? */
		slc++; /* Increment count */
  }

  return slc; /* Give count to caller */
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_SafetyList_AddBase(const PS_DPCRTLMM_BLOCKDESCARRAY PArrayBase)
{
  unsigned int sli;

  /* Sorry, this slows things down, must check to see if already here otherwise
  it would cause false leaks later. */
  if (SafetyList_IsArrayPtrPresent(PArrayBase)) return 0U;

  for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ )
  {
	 if (!_safetyList[sli]) /* First free entry? */
	 {
		_safetyList[sli] = PArrayBase; /* Store base in safety list */
		return 1U; /* Done, abort loop, report success */
	 }
  }

  return 0U; /* Damn, no free entries */

  /* If you do find that AddBase() fails increase the fixed list size, the
  definition can be found in BUILD.H and then rebuild the library, or cut
  down on the number of block arrays used in the program, this involves quite
  a bit of restructuring of the program but can reduce overhead, the other
  side effect is to make if more difficult to localize a leak, however. */
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_SafetyList_RemoveBase(const PS_DPCRTLMM_BLOCKDESCARRAY PArrayBase)
{
  unsigned int sli;

  /* This is faster than adding because we have no need for a separate search,
  if it is not found the function fails, it is is the entry is set to NULL,
  which is an effective removal. */

  for ( sli = 0U; sli < DPCRTLMM_SAFETYLIST_MAXSIZE; sli++ )
  {
	 if (_safetyList[sli] == PArrayBase) /* Found the base in question? */
	 {
		_safetyList[sli] = NULL; /* Mark entry in list as unused */
		return 1U; /* Successfully removed */
	 }
  }
  return 0U; /* Looks like the item was not present */
}
/*-------------------------------------------------------------------------*/
