/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2012, David Duncan Ross Palmer, Daybo Logic
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

#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_log.h" /* Main logging support */
#include "dpc_iblkptr.h" /* dpcrtlmm_int_IndexFromBlockPtr() */
#include "dpc_dbghooks.h" /* Debug hook executive */
#include "dpc_biglock.h" /* For entire library mutual exclusion */
#include "dpc_alloc.h" /* Allows us to call AllocEx(), bipassing the big lock */
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_LOG
static void OurLog(
  const char *File,
  const unsigned int Line,
  const unsigned short Severity,
  const char *Str
);
#endif /*DPCRTLMM_LOG*/

#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

#define OURLOG(f, l, sev, msg) OurLog((f), (l), ((const unsigned short)(sev)), (msg))
static void DPCRTLMM_FARDATA* dpcrtlmm_int_CallocEx(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int N,
  const size_t NewBlockSize,
  const char* File,
  const unsigned int Line
);
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_CallocEx(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int N,
  const size_t NewBlockSize,
  const char* File,
  const unsigned int Line
) {
  void DPCRTLMM_FARDATA* ret;

  LOCK
  ret = dpcrtlmm_int_CallocEx(PBlockArray, N, NewBlockSize, File, Line);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static void DPCRTLMM_FARDATA* dpcrtlmm_int_CallocEx(
  PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray,
  const unsigned int N,
  const size_t NewBlockSize,
  const char* File,
  const unsigned int Line
) {
  void DPCRTLMM_FARDATA* resultantPtr;
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  #ifdef DPCRTLMM_LOG
  char logMsg[MAX_TRAP_STRING_LENGTH+1];
  #endif /*DPCRTLMM_LOG*/

  #ifdef DPCRTLMM_LOG
  sprintf(
    logMsg,
    "Calloc() called, %u blocks of %u bytes requested, "
    "passing on to Alloc()",
    N,
    (unsigned int)NewBlockSize
  );
  OURLOG(File, Line, DPCRTLMM_LOG_MESSAGE, logMsg);
  #endif /*DPCRTLMM_LOG*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
  debugHookInfo.HookType = DPCRTLMM_HOOK_CALLOC;
  debugHookInfo.AllocReq = (N*NewBlockSize);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  resultantPtr = dpcrtlmm_int_AllocEx( PBlockArray, (N*NewBlockSize), File, Line); /* Call Alloc() */
  if (resultantPtr) {
    #ifdef DPCRTLMM_DEBUGHOOKS
    /* Ahh damn it, I'll have to look up the descriptor for this block */
    unsigned int blkIndex = dpcrtlmm_int_IndexFromBlockPtr(PBlockArray, resultantPtr);
    debugHookInfo.PRelDesc = &_ResolveArrayPtr(PBlockArray)->Descriptors[blkIndex];
    debugHookInfo.Success = 1U;
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    #ifdef DPCRTLMM_LOG
    OURLOG(File, Line, DPCRTLMM_LOG_MESSAGE, "Allocation successful");
    #endif /*DPCRTLMM_LOG*/

    /* Bug fix: I didn't realize this but the specification for for calloc()
       requires that the new memory is zeroed. Fix DPCRTLMM Version 1.1.2 or 1.1.3 */
    memset(resultantPtr, 0, N*NewBlockSize);
  } else {
    #ifdef DPCRTLMM_DEBUGHOOKS
    /*blockDescArray.Success = 0U;   - optimized away */
    #endif /*DPCRTLMM_DEBUGHOOKS*/
    #ifdef DPCRTLMM_LOG
    OURLOG(File, Line, DPCRTLMM_LOG_MESSAGE, "Allocation failed");
    #endif /*DPCRTLMM_LOG*/
  }

  #ifdef DPCRTLMM_DEBUGHOOKS
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_CALLOC, &debugHookInfo);
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  return resultantPtr;
}
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_LOG
static void OurLog(
  const char* File,
  const unsigned int Line,
  const unsigned short Severity,
  const char* Str
) {
  /* Our job is to add "Calloc() to the start of the string, saves data space
  if everybody in this module calls this instead of _Log() directly.
  We can't call _Log() twice because the information will be put on different
  lines so a copy is needed. */

  if (Str && Str[0]) { /* Valid string of at least on character sent to us? */
    char* PcopyStr;
    const char FuncName[] = "Calloc(): "; /* Prefix */

    PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) ); /* Allocate space for copy.  Note that NULL termination is automatic because using sizeof() */
    if (PcopyStr) {
      strcpy(PcopyStr, FuncName); /* Prepend prefix */
      strcat(PcopyStr, Str); /* Add log string after the prefix */

      dpcrtlmm_int_Log(File, Line, Severity, PcopyStr); /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}
#endif /*DPCRTLMM_LOG*/
/*-------------------------------------------------------------------------*/
