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
#ifndef __INC_DPCRTLMM_VPTRAP_H
#define __INC_DPCRTLMM_VPTRAP_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
void dpcrtlmm_int_VerifyPtrs(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr); /* BlockPtr can be NULL */
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#ifdef __INC_DPCRTLMM_INTDATA_H
#  define _VerifyPtrs dpcrtlmm_int_VerifyPtrs
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_VPTRAP_H*/
