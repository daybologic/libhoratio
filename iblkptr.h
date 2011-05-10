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
#ifndef __INC_DPCRTLMM_IBLKPTR
#define __INC_DPCRTLMM_IBLKPTR
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
unsigned int dpcrtlmm_int_IndexFromBlockPtr(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr);
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#ifdef __INC_DPCRTLMM_INTDATA_H /* Build from inside? */
#  define _IndexFromBlockPtr dpcrtlmm_int_IndexFromBlockPtr /* Lazy shortcut */
#else /* Build from outside? */
#  error ("Clear off! This header is for internal library use")
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*__INC_DPCRTLMM_IBLKPTR*/
