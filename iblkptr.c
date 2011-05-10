/*
    DPCRTLMM Memory Management Library
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
/*******************************************************
 * Internal function for getting the index of a block  *
 * within an array, now supports NULL arrays           *
 *******************************************************/
#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal data */
#include "trap.h" /* Trap support */
#include "vptrap.h" /* For _VerifyPtrs() */
#include "iblkptr.h"
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_int_IndexFromBlockPtr(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr)
{
  unsigned int i;
  PS_DPCRTLMM_BLOCKDESCARRAY PRArr; /* Resolved array pointer */

  _VerifyPtrs("IndexFromBlockPtr()", PBlockArray, NULL);

  PRArr = _ResolveArrayPtr(PBlockArray);
  for ( i = 0U; i < PRArr->Count; i++ )
  {
    if ( PRArr->Descriptors[i].PBase == BlockPtr ) /* Found it */
      return i; /* Give index to caller */
  }

  _Trap(DPCRTLMM_TRAP_INDEX_GEN_FAILED, "IndexFromBlockPtr(): Block not found, index not generated");
  return ~0; /* Buggered */
}
