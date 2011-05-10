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
/*******************************************************
 * Internal function for getting the index of a block  *
 * within an array, now supports NULL arrays           *
 *******************************************************/
#include <stddef.h>
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
