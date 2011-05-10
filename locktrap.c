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
Get updates: http://daybologic.com/Dev/dpcrtlmm
My official site: http://daybologic.com/overlord
*/
#define DPCRTLMM_SOURCE
/* A trap I prepared earlier - fires trap on a locking violation, call this
at the begining of any function which invariably leads to block descriptors
being modified or deleted (except for flag changes).  This will return
FALSE normally.  If it returns TRUE it because the block is locked, the
caller should then make steps not to modify the block.  This TRUE return only
ocours when trapping is off or handled by the user who chose to ignore the
trap.  Otherwise the function does not return. */

#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library header */
#include "trap.h" /* _Trap() */
#include "locktrap.h"
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_int_LockTrap(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr)
{
  if (dpcrtlmm_IsBlockLocked(PBlockArray, BlockPtr)) /* Block locked? */
  {
    char trapMsg[MAX_TRAP_STRING_LENGTH+1]; /* For this trap message */

    sprintf(trapMsg, "%s: (locking violation) Block 0x%p in array 0x%p, flag dump: %X",
            FuncName,
            PBlockArray,
            BlockPtr,
            (unsigned short)dpcrtlmm_ModifyDescriptorFlags(PBlockArray, BlockPtr, NULL)
    );
    _Trap(DPCRTLMM_TRAP_LOCKINGVIOLATION, trapMsg); /* Execute the trap */
    return 1U; /* If the trap recovered let program go on and notify user that trap ocourred */
  }
  return 0U; /* Block not locked */
}
