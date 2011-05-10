/*
    DPCRTLMM Memory Management Library
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
#define DPCRTLMM_SOURCE
/*
  Programmer: Overlord DDRP (Overlord@DayboLogic.co.uk)
  Library: DPCRTLMM
  Date: Feb 2000
  Last modified: 2nd August 2000
*/

#include <signal.h>
#include <string.h>
#include <stdio.h> /* For fprintf() */
#include <stdlib.h> /* For abort() */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "log.h" /* Internal logging support */
#include "trap.h"
#include "dbghooks.h" /* The debug hook executive */
/*-------------------------------------------------------------------------*/
static void DefHandler(const char* TrapMsg);
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int__Trap(const unsigned int Id, const char* Message)
{
  char* trapsCopy;
  const char preFix[] = "DPCRTLMM_UNHANDLED_TRAP: ";

  ERROR(Message); /* Pass on to the logger automatically */
  if ( !dpcrtlmm__EnableTraps ) return; /* Don't execute traps if traps have been switched off */

  /* The message is prefixed with "DPCRTLMM (Trap): " by copying it */
  trapsCopy = (char*)malloc( sizeof(preFix) + strlen(Message) ); /* No NULL terminator because sizeof() includes this */
  if (trapsCopy)
  {
    strcpy(trapsCopy, preFix);
    strcat(trapsCopy, Message);

    /* The Overlord had to work this out by means of a flow chart */
    if (_UserTrapCallback) /* Is user callback installed? */
    {
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
void dpcrtlmm_InstallTrapCallback( void(*NewTrapCallback)(const unsigned int, const char*), const unsigned int AsHook )
{
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;

  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
  debugHookInfo.HookType = DPCRTLMM_HOOK_INSTTRAPCALLBACK;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  if (NewTrapCallback)
  {
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
    sprintf(logStr, "InstallTrapCallback(): Installed the trap %s 0x%p",
	    (AsHook) ? ("hook") : ("handler"),
	    NewTrapCallback
    );
    MESSAGE(logStr);
    #endif /*DPCRTLMM_LOG*/

    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_INSTTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/
  }
  else /* Pointer to trap handler not passed */
  {
    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_INSTTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    if (_UserTrapCallback) /* Already have a handler and caller is trying to NULL it */
      _Trap(DPCRTLMM_TRAP_BAD_HANDLER_REMOVAL, "InstallTrapCallback(): Can\'t remove handler or hook in this way");
    else /* Trying to set handler when no current handler installed */
      _Trap(DPCRTLMM_TRAP_NULL_HANDLER, "InstallTrapCallback(): NULL handler or hook is not acceptable.");
  }
  return;
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_RemoveTrapCallback(void(*CurrentCallback)(const unsigned int, const char*))
{
  char logStr[MAX_TRAP_STRING_LENGTH+1];
  #ifdef DPCRTLMM_DEBUGHOOKS
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo;

  memset(&debugHookInfo, 0, sizeof(S_DPCRTLMM_DEBUGHOOKINFO));
  debugHookInfo.HookType = DPCRTLMM_HOOK_REMTRAPCALLBACK;
  debugHookInfo.Misc0 = (unsigned long)CurrentCallback;
  #endif /*DPCRTLMM_DEBUGHOOKS*/

  if (CurrentCallback == _UserTrapCallback) /* Make sure user knows */
  {
    _UserTrapCallback = NULL; /* Remove handler or hook */

    /* Log the removal */
    sprintf(logStr, "RemoveTrapCallback(): %s removed.", (_userTrapCallbackIsHook) ? ("Hook") : ("Handler"));
    MESSAGE(logStr);

    #ifdef DPCRTLMM_DEBUGHOOKS
    debugHookInfo.Success = 1U;
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_REMTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/
  }
  else /* The user does not know the address! */
  {
    #ifdef DPCRTLMM_DEBUGHOOKS
    dpcrtlmm_int_CallDebugHook(DPCRTLMM_HOOK_REMTRAPCALLBACK, &debugHookInfo);
    #endif /*DPCRTLMM_DEBUGHOOKS*/

    sprintf(logStr, "RemoveTrapCallback(): The handler is NOT 0x%p !!!", CurrentCallback);
    _Trap(DPCRTLMM_TRAP_UNAUTH_REMOVE, logStr);
  }
  return;
}
/*-------------------------------------------------------------------------*/
signed char dpcrtlmm_GetTrapCallbackInfo()
{
  if (!_UserTrapCallback) /* No user handler installed */
    return (signed char)-1;
  if (!_userTrapCallbackIsHook) /* Installed but not hook */
    return (signed char)0;

  return (signed char)1; /* Hook installed */
}
/*-------------------------------------------------------------------------*/
static void DefHandler(const char* TrapMsg)
{
  fprintf(stderr, TrapMsg); /* Output trap's message on the standard error stream */
  abort();
}
/*-------------------------------------------------------------------------*/
