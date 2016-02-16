/*
Horatio's Memory Manager
Copyright (c) 2000-2014, David Duncan Ross Palmer (M6KVM), Daybo Logic
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

/*! \file htrap.c
 * \brief Trap mechanism
 *
 * TODO: We should describe in more detail, the purpose of the trap subsystem
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <string.h>
#include <stdio.h> /* For fprintf() */
#include <stdlib.h> /* For abort() */

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library data */
#include "hlog.h" /* Internal logging support */
#include "htrap.h"
#include "hdbghook.h" /* The debug hook executive */
#include "hbiglock.h" /* Mutual exclusion */

static void horatio_int_InstallTrapCallback(
	void(*NewTrapCallback)(const unsigned int, const char*),
	const unsigned int AsHook
);
static void horatio_int_RemoveTrapCallback(
	void(*CurrentCallback)(const unsigned int, const char*)
);
static signed char horatio_int_GetTrapCallbackInfo(void);
static void DefHandler(
	const char *TrapMsg
);

void horatio_InstallTrapCallback(
	void(*NewTrapCallback)(const unsigned int, const char*),
	const unsigned int AsHook
) {
	LOCK
	horatio_int_InstallTrapCallback(NewTrapCallback, AsHook);
	UNLOCK
}

void horatio_RemoveTrapCallback(
	void(*CurrentCallback)(const unsigned int, const char*)
) {
	LOCK
	horatio_int_RemoveTrapCallback(CurrentCallback);
	UNLOCK
}

/*!
 * \brief Returns the state of trap handler (user configuration)
 *
 * Sometimes the user of the library has installed a handler for traps instead of
 * the default library handler, this function returns very simple information about it.
 * There are three return possible values, the other values won’t be used. The three possible values are
 *
 * -1 for no user trap handler
 *  0 for a handler which is installed as a handler
 *  1 for a handler which acts as a hook.
 *
 * The important thing about a hook is that it gets preview of the trap but the default one is called
 * afterwards and therefore the hook is expected to return.  A handler should terminate the program,
 * if it returns the default handler will not be called, the user handler has replaced it.
 *
 * So if a handler returns the program may continue after a trap which could lead to unpredictable results.
 *
 * \return Mode of trap operation
 */
signed char horatio_GetTrapCallbackInfo() {
	signed char ret;

	LOCK
	ret = horatio_int_GetTrapCallbackInfo();
	UNLOCK

	return ret;
}

void horatio_int_Trap(
	const unsigned int Id,
	const char *Message
) {
	char *trapsCopy;
	const char preFix[] = "HORATIO_UNHANDLED_TRAP: ";

	/* FIXME: Trap codes and log error codes overlap; this should be fixed */
	ERROR(Id, Message); /* Pass on to the logger automatically */
	/* Don't execute traps if traps have been switched off */
	if ( !horatio__EnableTraps ) return;

	/*
	 * The message is prefixed with "HORATIO (Trap): " by copying it
	 * No NULL terminator because sizeof() includes this
	 */
	trapsCopy = (char*)malloc( sizeof(preFix) + strlen(Message) );
	if (trapsCopy) {
		strcpy(trapsCopy, preFix);
		strcat(trapsCopy, Message);

		/* Palmer had to work this out by means of a flow chart */
		if (_UserTrapCallback) { /* Is user callback installed? */
			/* Call user's callback */
			_UserTrapCallback(Id, Message);
			/* Is handler, not hook? */
			if (!_userTrapCallbackIsHook)
				goto trapRecover; /* Recover from trap */
		}
		/* No user proc installed or user hook called */
		DefHandler(Message);

trapRecover:
		/*
		 * If handled by user handler and allowed to return free
		 * memory for copy of trap description string
		 */
		free(trapsCopy);
	}
	return;
}

static void horatio_int_InstallTrapCallback(
	void(*NewTrapCallback)(const unsigned int, const char*),
	const unsigned int AsHook
) {
#ifdef HORATIO_DEBUGHOOKS
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;

	memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));
	debugHookInfo.HookType = HORATIO_HOOK_INSTTRAPCALLBACK;
#endif /*HORATIO_DEBUGHOOKS*/

	if (NewTrapCallback) {
#ifdef HORATIO_LOG
		char logStr[MAX_TRAP_STRING_LENGTH+sizeof(char)];
#endif /*HORATIO_LOG*/

		/* Install the handler/hook */
		_UserTrapCallback = NewTrapCallback; /* Replace the PFunc */
		_userTrapCallbackIsHook = AsHook; /* Set hook mode (or not) */

#ifdef HORATIO_DEBUGHOOKS
		/* Update debug hook info */
		debugHookInfo.Success = 1U;
		debugHookInfo.Misc0 = (unsigned long)NewTrapCallback;
		/* Set bit 0 in accordance with the documentation */
		if (AsHook) debugHookInfo.Misc1 |= 1;
#endif /*HORATIO_DEBUGHOOKS*/

#ifdef HORATIO_LOG
		/* Log that we did that */
		sprintf(
			logStr,
			"InstallTrapCallback(): Installed the trap %s %s%lX",
			(AsHook) ? ("hook") : ("handler"),
			HORATIO_FMTPTRPFX, (unsigned long int)NewTrapCallback
		);
    MESSAGE(HORATIO_LOG_CODE_INSTALL_TRAP, NULL, 0, logStr);
#endif /*HORATIO_LOG*/

#ifdef HORATIO_DEBUGHOOKS
		horatio_int_CallDebugHook(
			HORATIO_HOOK_INSTTRAPCALLBACK, &debugHookInfo
		);
#endif /*HORATIO_DEBUGHOOKS*/
	} else { /* Pointer to trap handler not passed */
#ifdef HORATIO_DEBUGHOOKS
		horatio_int_CallDebugHook(
			HORATIO_HOOK_INSTTRAPCALLBACK, &debugHookInfo
		);
#endif /*HORATIO_DEBUGHOOKS*/

		if (_UserTrapCallback) {
			/*
			 * Already have a handler and caller is trying to
			 * NULL it
			 */
			Trap(
				HORATIO_TRAP_BAD_HANDLER_REMOVAL,
				"InstallTrapCallback(): Can\'t remove handler "
				"or hook in this way"
			);
		} else {
			/*
			 * Trying to set handler when no current handler
			 * installed
			 */
			Trap(
				HORATIO_TRAP_NULL_HANDLER,
				"InstallTrapCallback(): NULL handler "
				"or hook is not acceptable."
			);
		}
	}
	return;
}

static void horatio_int_RemoveTrapCallback(
	void(*CurrentCallback)(const unsigned int, const char*)
) {
	char logStr[MAX_TRAP_STRING_LENGTH+1];
#ifdef HORATIO_DEBUGHOOKS
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;

	memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));
	debugHookInfo.HookType = HORATIO_HOOK_REMTRAPCALLBACK;
	debugHookInfo.Misc0 = (unsigned long)CurrentCallback;
#endif /*HORATIO_DEBUGHOOKS*/

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
    MESSAGE(HORATIO_LOG_CODE_REMOVE_TRAP, NULL, 0, logStr);

#ifdef HORATIO_DEBUGHOOKS
		debugHookInfo.Success = 1U;
		horatio_int_CallDebugHook(
			HORATIO_HOOK_REMTRAPCALLBACK, &debugHookInfo
		);
#endif /*HORATIO_DEBUGHOOKS*/
	} else { /* The user does not know the address! */
#ifdef HORATIO_DEBUGHOOKS
		horatio_int_CallDebugHook(
			HORATIO_HOOK_REMTRAPCALLBACK, &debugHookInfo
		);
#endif /*HORATIO_DEBUGHOOKS*/

		sprintf(
			logStr,
	#ifdef HAVE_SNPRINTF
			MAX_TRAP_STRING_LENGTH,
	#endif /*HAVE_SNPRINTF*/
			"RemoveTrapCallback(): The handler is NOT %s%lX !!!",
			HORATIO_FMTPTRPFX, (unsigned long int)CurrentCallback
		);

		Trap(HORATIO_TRAP_UNAUTH_REMOVE, logStr);
	}
	return;
}

static signed char horatio_int_GetTrapCallbackInfo() {
	if (!_UserTrapCallback) /* No user handler installed */
		return (signed char)-1;
	if (!_userTrapCallbackIsHook) /* Installed but not hook */
		return (signed char)0;

	return (signed char)1; /* Hook installed */
}

void horatio_EnableTraps() {
	LOCK
	horatio__EnableTraps = '\x1';
	UNLOCK
}

void horatio_DisableTraps() {
	LOCK
	horatio__EnableTraps = '\x0';
	UNLOCK
}

unsigned char horatio_AreTrapsEnabled() {
	unsigned char ret;

	LOCK
	ret = horatio__EnableTraps;
	UNLOCK

	return ret;
}

static void DefHandler(
	const char *TrapMsg
) {
	/* Output trap's message on the standard error stream */
	fprintf(HORATIO_DEV_ERROR, "%s", TrapMsg);
	abort();
}
