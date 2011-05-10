/*
    DPCRTLMM Memory management library : Block locking interface
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
/* Module for handling memory block lock flags,
written by Overlord David Duncan Ross Palmer
Copyright (C)2000 Daybo Logic, all rights reserved.
Creation: 22nd Feb 2000
Last modified: 31st July 2001
RESPECT! */

#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* The main library header */
#include "biglock.h" /* Mutual exclusion */
#include "bdflags.h" /* Need this to get around the lock */
#include "bloclock.h"
/*-------------------------------------------------------------------------*/
void dpcrtlmm_SetBlockLockingFlag(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned int NewStatus)
{
  LOCK
  dpcrtlmm_int_SetBlockLockingFlag(PBlockArray, Ptr, NewStatus);
  UNLOCK
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsBlockLocked(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr)
{
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_IsBlockLocked(PBlockArray, Ptr);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_ToggleBlockLockingStatus(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr)
{
  LOCK
  dpcrtlmm_int_ToggleBlockLockingStatus(PBlockArray, Ptr);
  UNLOCK
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_SetBlockLockingFlag(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned int NewStatus)
{
  unsigned char flags;

  flags = dpcrtlmm_int_ModifyDescriptorFlags(PBlockArray, Ptr, NULL); /* Get current flags */
  if (NewStatus) /* Locking? */
    flags |= 1; /* Set lock bit */
  else /* Unlocking? */
    flags |= ~1; /* Clear lock bit */
  dpcrtlmm_int_ModifyDescriptorFlags(PBlockArray, Ptr, &flags); /* Set the new flags */
  return; /* That was simple enough, I can drink some water now */
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_int_IsBlockLocked(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr)
{
  unsigned char flags;

  flags = dpcrtlmm_int_ModifyDescriptorFlags(PBlockArray, Ptr, NULL); /* Get the flags for the descriptor */
  if ( ((flags & 1) == 1) ) /* The lock bit is set? */
    return 1U; /* Yes, the block is locked */
  return 0U; /* No, the block is not locked */
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_ToggleBlockLockingStatus(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr)
{
  unsigned int oldLockStat = dpcrtlmm_int_IsBlockLocked(PBlockArray, Ptr); /* Get current status */
  dpcrtlmm_int_SetBlockLockingFlag(PBlockArray, Ptr, !oldLockStat); /* Set locking state as NOT current locking state */
}
/*-------------------------------------------------------------------------*/
