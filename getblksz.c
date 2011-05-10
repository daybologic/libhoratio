/*
    DPCRTLMM Memory Management Library: Block size function(s)
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
/* Gets size of a block

Last mod: 21st July 2000
*/
#include <stdio.h>
#include <stddef.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "trap.h" /* _Trap() - Trap support */
#include "vptrap.h" /* _VerifyPtrs() */
#include "iblkptr.h" /* IndexFromBlockPtr() */
/*-------------------------------------------------------------------------*/
size_t dpcrtlmm_GetBlockSize( PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, void DPCRTLMM_FARDATA* BlockPtr)
{
  /* First test pointers */
  _VerifyPtrs("GetBlockSize()", PBlockArray, NULL);

  if (dpcrtlmm_IsBadBlockPtr(PBlockArray, BlockPtr)) /* Block pointer is invalid? */
  {
    char trapMsg[MAX_TRAP_STRING_LENGTH+1];

    sprintf(trapMsg, "Attempt to GetBlockSize() unknown block at base 0x%p, in array base: 0x%p",
            BlockPtr,
            PBlockArray
    );
    _Trap(DPCRTLMM_TRAP_BAD_BLOCK, trapMsg);
  }
  return _ResolveArrayPtr(PBlockArray)->Descriptors[ _IndexFromBlockPtr(PBlockArray, BlockPtr) ].Size;
}
/*-------------------------------------------------------------------------*/
