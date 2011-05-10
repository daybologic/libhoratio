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
