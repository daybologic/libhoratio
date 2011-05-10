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
#ifndef __INC_DPCRTLMM_TRAP_H
#define __INC_DPCRTLMM_TRAP_H
/*-------------------------------------------------------------------------*/
#ifndef __INC_DPCRTLMM_INTDATA_H
#  error ("Internal library users only!")  // <-- See Overlord DDRP Overlord@DayboLogic.co.uk
#endif /*__INC_DPCRTLMM_INTDATA_H*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
void dpcrtlmm_int__Trap(const unsigned int Id, const char* Message); /* Fire the specified trap */
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

/* NOTE: Prototype for InstallTrapHandler() is in DPCRTLMM.H */

#ifdef __INC_DPCRTLMM_INTDATA_H
#  define _Trap dpcrtlmm_int__Trap
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_TRAP_H*/