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
/* Header for the debug hook executive and support code */

#ifndef __INC_DPCRTLMM_DBGHOOKS_H
#define __INC_DPCRTLMM_DBGHOOKS_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#ifndef DPCRTLMM_SOURCE
#  error ("Internal library users only!")  // <-- See Overlord DDRP
#endif /*!DPCRTLMM_SOURCE*/

#ifdef DPCRTLMM_DEBUGHOOKS

void dpcrtlmm_int_InitDebugHookMatrix(void); /* Call to clear all pointers in the matrix */

/* Debug hook executive, caller must prepare information according to the
rules first. */
void dpcrtlmm_int_CallDebugHook(const unsigned short HookType, const PS_DPCRTLMM_DEBUGHOOKINFO PDebugHookInfo);

/* For user mode functions declrs: dpcrtlmm_InstallDebugHook(),
				   dpcrtlmm_GetDebugHookChainCount(),
				   dpcrtlmm_GetDebugHookMatrixCount() &
				   dpcrtlmm_UninstallDebugHook()
look in the public header DPCRTLMM.H. */

#endif /*DPCRTLMM_DEBUGHOOKS*/

#ifdef __INC_DPCRTLMM_INTDATA_H
#  define _InitDebugHookMatrix dpcrtlmm_int_InitDebugHookMatrix
#  define _CallHook dpcrtlmm_int_CallHook
#endif /*__INC_DPCRTLMM_INTDATA_H*/

#ifdef __cplusplus
} /*extern "C"*/
#endif /*__cplusplus*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_DBGHOOKS_H*/
