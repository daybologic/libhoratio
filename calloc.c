/*
    DPCRTLMM Memory Management Library : callocator
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
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/
#define DPCRTLMM_SOURCE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "log.h" /* Main logging support */
#include "iblkptr.h" /* dpcrtlmm_int_IndexFromBlockPtr() */
#include "dbghooks.h" /* Debug hook executive */
#include "biglock.h" /* For entire library mutual exclusion */
#include "alloc.h" /* Allows us to call AllocEx(), bipassing the big lock */
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_LOG
static void OurLog(const char* File, const unsigned int Line, const unsigned short Severity, const char* Str);
#endif /*DPCRTLMM_LOG*/

#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

#define OURLOG(f, l, sev, msg) OurLog((f), (l), ((const unsigned short)(sev)), (msg))
static void DPCRTLMM_FARDATA* dpcrtlmm_int_CallocEx(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line);
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_CallocEx(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  void DPCRTLMM_FARDATA* ret;

  LOCK
  ret = dpcrtlmm_int_CallocEx(PBlockArray, N, NewBlockSize, File, Line);
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static void DPCRTLMM_FARDATA* dpcrtlmm_int_CallocEx(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  void DPCRTLMM_FARDATA* resultantPtr;
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  #ifdef DPCRTLMM_LOG
  char logMsg[MAX_TRAP_STRING_LENGTH+1];
  #endif /*DPCRTLMM_LOG*/

  #ifdef DPCRTLMM_LOG
  sprintf(logMsg, "Calloc() called, %u blocks of %u bytes requested, passing on to Alloc()", N, NewBlockSize);
  OURLOG(File, Line, DPCRTLMM_LOG_MESSAGE, logMsg);
  #endif /*DPCRTLMM_LOG*/

  #ifdef DPCRTLMM_DEBUGHOOKS
  debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
  debugHookInfo.HookType = DPCRTLMM_HOOK_CALLOC;
  debugHookInfo.AllocReq = (N*NewBlockSize);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  resultantPtr = dpcrtlmm_int_AllocEx( PBlockArray, (N*NewBlockSize), File, Line); /* Call Alloc() */
  if (resultantPtr)
  {
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
  }
  else
  {
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
static void OurLog(const char* File, const unsigned int Line, const unsigned short Severity, const char* Str)
{
  /* Our job is to add "Calloc() to the start of the string, saves data space
  if everybody in this module calls this instead of _Log() directly.
  We can't call _Log() twice because the information will be put on different
  lines so a copy is needed. */

  if (Str && Str[0]) /* Valid string of at least on character sent to us? */
  {
    char* PcopyStr;
    const char FuncName[] = "Calloc(): "; /* Prefix */

    PcopyStr = (char*)malloc( sizeof(FuncName) + strlen(Str) ); /* Allocate space for copy.  Note that NULL termination is automatic because using sizeof() */
    if (PcopyStr)
    {
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
