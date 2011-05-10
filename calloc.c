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
/*
#############################################################################
# calloc() for DPCRTLMM by Overlord David Duncan Ross Palmer,               #
# Copyright 2000, OverlordDDRP, Daybo Logic, all rights reserved.           #
#############################################################################
*/
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
/*-------------------------------------------------------------------------*/
#ifdef OURLOG /* Somebody else using OURLOG? */
#  undef OURLOG /* Don't want their version */
#endif /*OURLOG*/

#ifdef DPCRTLMM_LOG /* Local logger only available in a log build */
   static void OurLog(const char* Str); /* Logging wrapper which knows our name */
#  define OURLOG(msg) OurLog(msg); /* Macro goes to local function */
#else /* Not a loggable build */
#  define OURLOG(msg) /* Dummy version so logging isn't done and log strings don't end up in the binary of the library */
#endif /*DPCRTLMM_LOG*/
/*-------------------------------------------------------------------------*/
void DPCRTLMM_FARDATA* dpcrtlmm_Calloc(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const unsigned int N, const size_t NewBlockSize)
{
  void DPCRTLMM_FARDATA* resultantPtr;
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  #ifdef DPCRTLMM_LOG
  char logMsg[MAX_TRAP_STRING_LENGTH+1];

  sprintf(logMsg, "called, %u blocks of %u bytes requested, passing on to Alloc()", N, NewBlockSize);
  #endif /*DPCRTLMM_LOG*/
  OURLOG(logMsg)

  #ifdef DPCRTLMM_DEBUGHOOKS
  debugHookInfo.PRelArr = _ResolveArrayPtr(PBlockArray);
  debugHookInfo.HookType = DPCRTLMM_HOOK_CALLOC;
  debugHookInfo.AllocReq = (N*NewBlockSize);
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  resultantPtr = dpcrtlmm_Alloc( PBlockArray, (N*NewBlockSize) ); /* Call Alloc() */
  if (resultantPtr)
  {
    #ifdef DPCRTLMM_DEBUGHOOKS
    /* Ahh damn it, I'll have to look up the descriptor for this block */
    unsigned int blkIndex = dpcrtlmm_int_IndexFromBlockPtr(PBlockArray, resultantPtr);
    debugHookInfo.PRelDesc = &_ResolveArrayPtr(PBlockArray)->Descriptors[blkIndex];
    debugHookInfo.Success = 1U;
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    OURLOG("Allocation successful")
  }
  else
  {
    #ifdef DPCRTLMM_DEBUGHOOKS
    /*blockDescArray.Success = 0U;   - optimized away */
    #endif /*DPCRTLMM_DEBUGHOOKS*/
    OURLOG("Allocation failed")
  }

  #ifdef DPCRTLMM_DEBUGHOOKS
  dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_CALLOC, &debugHookInfo);
  #endif /*DPCRTLMM_DEBUGHOOKS*/
  return resultantPtr;
}
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_LOG /* Logging must be enabled in the build (see Build.H) */
static void OurLog(const char* Str)
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

      LOG(PcopyStr) /* Pass on to the normal logger */

      free(PcopyStr); /* Copy can now be released */
    }
  }
  return;
}
#endif /*DPCRTLMM_LOG*/
/*-------------------------------------------------------------------------*/
