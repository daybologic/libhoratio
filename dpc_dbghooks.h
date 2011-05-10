/*
    DPCRTLMM internal debug hook working header
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
Get updates: http://daybologic.com/Dev/dpcrtlmm
My official site: http://daybologic.com/overlord
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
