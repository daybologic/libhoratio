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
