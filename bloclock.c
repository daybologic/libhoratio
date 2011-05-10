/**********************************************************************
 *                                                                    *
 * "DPCRTLMM" David Palmer's C-RTL Memory Manager Copyright (c) 2000  *
 * David Duncan Ross Palmer, Daybo Logic all rights reserved.         *
 * http://daybologic.com/Dev/dpcrtlmm                                 *
 *                                                                    *
 * D.D.R. Palmer's official homepage: http://daybologic.com/overlord  *
 * See the included license file for more information.                *
 *                                                                    *
 **********************************************************************
*/
#define DPCRTLMM_SOURCE
/* Module for handling memory block lock flags,
written by Overlord David Duncan Ross Palmer
Copyright (C)2000 Daybo Logic, all rights reserved.
Creation: 22nd Feb 2000
Last modified: 21st July 2000
RESPECT! */

#include <stddef.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* The main library header */
/*-------------------------------------------------------------------------*/
void dpcrtlmm_SetBlockLockingFlag(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned int NewStatus)
{
  unsigned char flags;

  flags = dpcrtlmm_ModifyDescriptorFlags(PBlockArray, Ptr, NULL); /* Get current flags */
  if (NewStatus) /* Locking? */
    flags |= 1; /* Set lock bit */
  else /* Unlocking? */
    flags |= ~1; /* Clear lock bit */
  dpcrtlmm_ModifyDescriptorFlags(PBlockArray, Ptr, &flags); /* Set the new flags */
  return; /* That was simple enough, I can drink some water now */
}
/*-------------------------------------------------------------------------*/
unsigned int dpcrtlmm_IsBlockLocked(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr)
{
  unsigned char flags;

  flags = dpcrtlmm_ModifyDescriptorFlags(PBlockArray, Ptr, NULL); /* Get the flags for the descriptor */
  if ( ((flags & 1) == 1) ) /* The lock bit is set? */
    return 1U; /* Yes, the block is locked */
  return 0U; /* No, the block is not locked */
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_ToggleBlockLockingStatus(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr)
{
  unsigned int oldLockStat = dpcrtlmm_IsBlockLocked(PBlockArray, Ptr); /* Get current status */
  dpcrtlmm_SetBlockLockingFlag(PBlockArray, Ptr, !oldLockStat); /* Set locking state as NOT current locking state */
}
/*-------------------------------------------------------------------------*/
