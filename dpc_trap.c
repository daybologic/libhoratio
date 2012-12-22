/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer, Daybo Logic
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
  Library: DPCRTLMM
  Created: Feb 2000
*/

#define DPCRTLMM_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <string.h>
#include <stdio.h> /* For fprintf() */
#include <stdlib.h> /* For abort() */

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_log.h" /* Internal logging support */
#include "dpc_trap.h"
#include "dpc_dbghooks.h" /* The debug hook executive */
#include "dpc_biglock.h" /* Mutual exclusion */
/*-------------------------------------------------------------------------*/
static void dpcrtlmm_int_InstallTrapCallback(
  void(*NewTrapCallback)(const unsigned int, const char*),
  const unsigned int AsHook
);
static void dpcrtlmm_int_RemoveTrapCallback(
  void(*CurrentCallback)(const unsigned int, const char*)
);
static signed char dpcrtlmm_int_GetTrapCallbackInfo(void);
static void DefHandler(
  const char *TrapMsg
);
/*-------------------------------------------------------------------------*/
void dpcrtlmm_InstallTrapCallback(
  void(*NewTrapCallback)(const unsigned int, const char*),
  const unsigned int AsHook
) {
  LOCK
  dpcrtlmm_int_InstallTrapCallback(NewTrapCallback, AsHook);
  UNLOCK
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_RemoveTrapCallback(
  void(*CurrentCallback)(const unsigned int, const char*)
) {
  LOCK
  dpcrtlmm_int_RemoveTrapCallback(CurrentCallback);
  UNLOCK
}
/*-------------------------------------------------------------------------*/
signed char dpcrtlmm_GetTrapCallbackInfo() {
  signed char ret;

  LOCK
  ret = dpcrtlmm_int_GetTrapCallbackInfo();
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_Trap(
  const unsigned int Id,
  const char *Message
) {
  char* trapsCopy;
  const char preFix[] = "DPCRTLMM_UNHANDLED_TRAP: ";

  /* FIXME: Trap codes and log error codes overlap; this should be fixed */
  ERROR(Id, Message); /* Pass on to the logger automatically */
  if ( !dpcrtlmm__EnableTraps ) return; /* Don't execute traps if traps have been switched off */

  /* The message is prefixed with "DPCRTLMM (Trap): " by copying it */
  trapsCopy = (char*)malloc( sizeof(preFix) + strlen(Message) ); /* No NULL terminator because sizeof() includes this */
  if (trapsCopy) {
    strcpy(trapsCopy, preFix);
    strcat(trapsCopy, Message);

    /* The Overlord had to work this out by means of a flow chart */
    if (_UserTrapCallback) { /* Is user callback installed? */
      _UserTrapCallback(Id, Message); /* Call user's callback */
      if (!_userTrapCallbackIsHook) /* Is handler, not hook? */
        goto trapRecover; /* Recover from trap */
    }
    /* No user proc installed or user hook called */
    DefHandler(Message);

trapRecover:
    /* If handled by user handler and allowed to return free memory for copy of
    trap description string */
    free(trapsCopy);
  }
  return;
}
/*-------------------------------------------------------------------------*/
static void dpcrtlmm_int_InstallTrapCallback(
  void(*NewTrapCallback)(const unsigned int, const char*),
  const unsigned int AsHook
) {
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;

  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
  debugHookInfo.HookType = DPCRTLMM_HOOK_INSTTRAPCALLBACK;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  if (NewTrapCallback) {
    #ifdef DPCRTLMM_LOG
    char logStr[MAX_TRAP_STRING_LENGTH+sizeof(char)];
    #endif /*DPCRTLMM_LOG*/

    /* Install the handler/hook */
    _UserTrapCallback = NewTrapCallback; /* Replace the PFunc */
    _userTrapCallbackIsHook = AsHook; /* Set hook mode (or not) */

    #ifdef DPCRTLMM_DEBUGHOOKS
    /* Update debug hook info */
    debugHookInfo.Success = 1U;
    debugHookInfo.Misc0 = (unsigned long)NewTrapCallback;
    if (AsHook) debugHookInfo.Misc1 |= 1; /* Set bit 0 in accordance with the documentation */
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    #ifdef DPCRTLMM_LOG
    /* Log that we did that */
    sprintf(
      logStr,
      "InstallTrapCallback(): Installed the trap %s %s%lX",
      (AsHook) ? ("hook") : ("handler"),
      DPCRTLMM_FMTPTRPFX, (unsigned long int)NewTrapCallback
    );
    MESSAGE(DPCRTLMM_LOG_CODE_INSTALL_TRAP, NULL, 0, logStr);
    #endif /*DPCRTLMM_LOG*/

    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_INSTTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/
  } else { /* Pointer to trap handler not passed */
    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_INSTTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    if (_UserTrapCallback) /* Already have a handler and caller is trying to NULL it */
      Trap(DPCRTLMM_TRAP_BAD_HANDLER_REMOVAL, "InstallTrapCallback(): Can\'t remove handler or hook in this way");
    else /* Trying to set handler when no current handler installed */
      Trap(DPCRTLMM_TRAP_NULL_HANDLER, "InstallTrapCallback(): NULL handler or hook is not acceptable.");
  }
  return;
}
/*-------------------------------------------------------------------------*/
static void dpcrtlmm_int_RemoveTrapCallback(
  void(*CurrentCallback)(const unsigned int, const char*)
) {
  char logStr[MAX_TRAP_STRING_LENGTH+1];
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;

  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
  debugHookInfo.HookType = DPCRTLMM_HOOK_REMTRAPCALLBACK;
  debugHookInfo.Misc0 = (unsigned long)CurrentCallback;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  if (CurrentCallback == _UserTrapCallback) { /* Make sure user knows */
    _UserTrapCallback = NULL; /* Remove handler or hook */

    /* Log the removal */
    sprintf(
      logStr,
      #ifdef HAVE_SNPRINTF
      MAX_TRAP_STRING_LENGTH,
      #endif /*HAVE_SNPRINTF*/
      "RemoveTrapCallback(): %s removed.",
      (_userTrapCallbackIsHook) ? ("Hook") : ("Handler")
    );
    MESSAGE(DPCRTLMM_LOG_CODE_REMOVE_TRAP, NULL, 0, logStr);

    #ifdef DPCRTLMM_DEBUGHOOKS
    debugHookInfo.Success = 1U;
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_REMTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/
  } else { /* The user does not know the address! */
    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_REMTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    sprintf(
      logStr,
      #ifdef HAVE_SNPRINTF
      MAX_TRAP_STRING_LENGTH,
      #endif /*HAVE_SNPRINTF*/
      "RemoveTrapCallback(): The handler is NOT %s%lX !!!",
      DPCRTLMM_FMTPTRPFX, (unsigned long int)CurrentCallback
    );

    Trap(DPCRTLMM_TRAP_UNAUTH_REMOVE, logStr);
  }
  return;
}
/*-------------------------------------------------------------------------*/
static signed char dpcrtlmm_int_GetTrapCallbackInfo() {
  if (!_UserTrapCallback) /* No user handler installed */
    return (signed char)-1;
  if (!_userTrapCallbackIsHook) /* Installed but not hook */
    return (signed char)0;

  return (signed char)1; /* Hook installed */
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_EnableTraps() {
  LOCK
  dpcrtlmm__EnableTraps = '\x1';
  UNLOCK
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_DisableTraps() {
  LOCK
  dpcrtlmm__EnableTraps = '\x0';
  UNLOCK
}
/*-------------------------------------------------------------------------*/
unsigned char dpcrtlmm_AreTrapsEnabled() {
  unsigned char ret;

  LOCK
  ret = dpcrtlmm__EnableTraps;
  UNLOCK

  return ret;
}
/*-------------------------------------------------------------------------*/
static void DefHandler(
  const char *TrapMsg
) {
  /*
    Output trap's message on the standard error stream
  */
  fprintf(DPCRTLMM_DEV_ERROR, TrapMsg);
  abort();
}
/*-------------------------------------------------------------------------*/
