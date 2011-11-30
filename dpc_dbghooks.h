/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2006, David Duncan Ross Palmer, Daybo Logic
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
      
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
      
    * Neither the name of the Daybo Logic nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Header for the debug hook executive and support code
*/

#ifndef INC_DPCRTLMM_DBGHOOKS_H
#define INC_DPCRTLMM_DBGHOOKS_H
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
void dpcrtlmm_int_CallDebugHook(
  const unsigned short HookType,
  const PS_DPCRTLMM_DEBUGHOOKINFO PDebugHookInfo
);

/*
  For user mode functions declrs:
  dpcrtlmm_InstallDebugHook(),
  dpcrtlmm_GetDebugHookChainCount(),
  dpcrtlmm_GetDebugHookMatrixCount() &
  dpcrtlmm_UninstallDebugHook()
                                   
  look in the public header dpcrtlmm.h.
*/

#endif /*DPCRTLMM_DEBUGHOOKS*/

#ifdef INC_DPCRTLMM_INTDATA_H
#  define _InitDebugHookMatrix dpcrtlmm_int_InitDebugHookMatrix
#  define _CallHook dpcrtlmm_int_CallHook
#endif /*INC_DPCRTLMM_INTDATA_H*/

#ifdef __cplusplus
} /*extern "C"*/
#endif /*__cplusplus*/
/*-------------------------------------------------------------------------*/
#endif /*!INC_DPCRTLMM_DBGHOOKS_H*/
