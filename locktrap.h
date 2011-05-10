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
/* Internal header for a trap I prepared earlier for when a block locking
violation ocours.
Date of creation: 23rd Feb 2000 - Overlord David Duncan Ross Palmer
                                  (Overlord@DayboLogic.co.uk)
Last modified: 4th May 2000 by OverlordDDRP: Stripped BOOL
*/

#ifndef __INC_DPCRTLMM_LOCKTRAP_H
#define __INC_DPCRTLMM_LOCKTRAP_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
unsigned int dpcrtlmm_int_LockTrap(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr);
#ifdef __cplusplus
}
#endif

#ifdef __INC_DPCRTLMM_INTDATA_H
#  define _LockTrap dpcrtlmm_int_LockTrap
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_LOCKTRAP_H*/
