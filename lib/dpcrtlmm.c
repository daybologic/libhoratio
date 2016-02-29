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

/*! \file dpcrtlmm.c
 * \brief Legacy wrapper functions (ABI compatibility)
 *
 * All functions are designed to retain compatibility with existing apps
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* for abort() */

#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#ifndef DPCRTLMM_LEGACY
# define DPCRTLMM_LEGACY
#endif /*DPCRTLMM_LEGACY*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hdbghook.h" /* Debug hook executive and support functions */

static struct {
	const char *funcName;
	unsigned long int count;
} callCounters[] = {
	{ "InstallDebugHook", 0 },
	{ "GetDebugHookChainCount", 0 },
	{ "GetDebugHookMatrixCount", 0 },
	{ "UninstallDebugHook", 0 },
	{ "AllocEx", 0 },
	{ "StrdupEx", 0 },
	{ "Free", 0 },
	{ "CreateBlockArray", 0 },
	{ "DestroyBlockArray", 0 },
	{ "IsDefaultBlockArray", 0 },
	{ "Startup", 0 },
	{ "Shutdown", 0 },
	{ "IsStarted", 0 },
	{ "GetBlockSize", 0 },
	{ "IsBadBlockPtr", 0 },
	{ "IsBadBlockPtr", 0 },
	{ "Realloc", 0 },
	{ "CallocEx", 0 },
	{ "InstallTrapCallback", 0 },
	{ "RemoveTrapCallback", 0 },
	{ "GetTrapCallbackInfo", 0 },
	{ "ModifyDescriptorFlags", 0 },
	{ "SetBlockLockingFlag", 0 },
	{ "IsBlockLocked", 0 },
	{ "ToggleBlockLockingStatus", 0 },
	{ "AreTrapsEnabled", 0 },
	{ "DisableTraps", 0 },
	{ "EnableTraps", 0 },
	{ "GetBlockCount", 0 },
	{ "GetStats", 0 },
	{ "Dump", 0 },
	{ "Ver", 0 }
};
static unsigned long int incrementCallCounter(const char *const funcName) {
	unsigned int i;
	for (i = 0U; i < sizeof(callCounters)/sizeof(callCounters[0]); i++) {
		if (0 != strcmp(funcName, callCounters[i].funcName))
			continue;

		return callCounters[i].count++;
	}

	fprintf(stderr, "Deprecated call %s, missing from counters\n", funcName);
	abort();
}

static void deprecated(const char *const funcName) {
#ifdef HORATIO_DEBUGHOOKS
	unsigned short int hookType = HORATIO_HOOK_LEGACY;
	S_HORATIO_DEBUGHOOKINFO debugHookInfo;
#endif /*HORATIO_DEBUGHOOKS*/

	if (0 == incrementCallCounter(funcName))
		fprintf(stderr, "Deprecated function call: dpcrtlmm_%s\n", funcName);

	/* Call the debug hook executive */
#ifdef HORATIO_DEBUGHOOKS
	/* Init structure */
	memset(&debugHookInfo, 0, sizeof(S_HORATIO_DEBUGHOOKINFO));
	/*debugHookInfo.PRelArr = PRArr;
	debugHookInfo.PRelDesc = genBlockPtr;*/
	debugHookInfo.HookType = hookType;
	debugHookInfo.AllocReq = (unsigned int)0;
	debugHookInfo.Success = 1U; /* TRUE */
	debugHookInfo.Misc0 = (unsigned long)funcName;
	debugHookInfo.Misc1 = 0; /* FIXME */
	horatio_int_CallDebugHook(hookType, &debugHookInfo);
#endif /*HORATIO_DEBUGHOOKS*/

	return;
}

unsigned int dpcrtlmm_InstallDebugHook(
	const unsigned short HookType,
	unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
) {
	deprecated("InstallDebugHook");
	return horatio_InstallDebugHook(
		HookType,
		NewHookProc
	);
}

unsigned int dpcrtlmm_GetDebugHookChainCount(
	const unsigned int HookType
) {
	deprecated("GetDebugHookChainCount");
	return horatio_GetDebugHookChainCount(
		HookType
	);
}

unsigned int dpcrtlmm_GetDebugHookMatrixCount() {
	deprecated("GetDebugHookMatrixCount");
	return horatio_GetDebugHookMatrixCount();
}

unsigned int dpcrtlmm_UninstallDebugHook(
	const unsigned short HookType,
	unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
) {
	deprecated("UninstallDebugHook");
	return horatio_UninstallDebugHook(
		HookType,
		HookProc2Remove
	);
}

void HORATIO_FARDATA* dpcrtlmm_AllocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
) {
	deprecated("AllocEx");
	return horatio_AllocEx(
		PBlockArray,
		NewBlockSize,
		File,
		Line
	);
}

char HORATIO_FARDATA *dpcrtlmm_StrdupEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const char *SrcStr,
	const char *File,
	const unsigned int Line
) {
	deprecated("StrdupEx");
	return horatio_StrdupEx(
		PBlockArray,
		SrcStr,
		File,
		Line
	);
}

void dpcrtlmm_Free(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	void HORATIO_FARDATA *Ptr
) {
	deprecated("Free");
	horatio_Free(
		PBlockArray,
		Ptr
	);
	return;
}

PS_HORATIO_BLOCKDESCARRAY dpcrtlmm_CreateBlockArray() {
	deprecated("CreateBlockArray");
	return horatio_CreateBlockArray();
}

void dpcrtlmm_DestroyBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	deprecated("DestroyBlockArray");
	horatio_DestroyBlockArray(
		PBlockArray
	);
	return;
}

unsigned int dpcrtlmm_IsDefaultBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	deprecated("IsDefaultBlockArray");
	return horatio_IsDefaultBlockArray(
		PBlockArray
	);
}

void dpcrtlmm_Startup() {
	deprecated("Startup");
	horatio_Startup();
	return;
}

void dpcrtlmm_Shutdown() {
	deprecated("Shutdown");
	horatio_Shutdown();
	return;
}

unsigned int dpcrtlmm_IsStarted() {
	deprecated("IsStarted");
	return horatio_IsStarted();
}

size_t dpcrtlmm_GetBlockSize(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *const BlockPtr
) {
	deprecated("GetBlockSize");
	return horatio_GetBlockSize(
		PBlockArray,
		BlockPtr
	);
}

unsigned int dpcrtlmm_IsBadBlockPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
) {
	deprecated("IsBadBlockPtr");
	return horatio_IsBadBlockPtr(
		PBlockArray,
		BlockPtr
	);
}

unsigned int dpcrtlmm_IsBadArrayPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	deprecated("IsBadBlockPtr");
	return horatio_IsBadArrayPtr(
		PBlockArray
	);
}

void HORATIO_FARDATA *dpcrtlmm_Realloc(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	void HORATIO_FARDATA *OldBlockPtr,
	const size_t NewSize
) {
	deprecated("Realloc");
	return horatio_Realloc(
		PBlockArray,
		OldBlockPtr,
		NewSize
	);
}

void HORATIO_FARDATA *dpcrtlmm_CallocEx(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const unsigned int N,
	const size_t NewBlockSize,
	const char *File,
	const unsigned int Line
) {
	deprecated("CallocEx");
	return horatio_CallocEx(
		PBlockArray,
		N,
		NewBlockSize,
		File,
		Line
	);
}

void dpcrtlmm_InstallTrapCallback(
	void(*UserCallbackProc)(
		const unsigned int TrapID,
		const char *TrapMessage
	),
	const unsigned int AsHook
) {
	deprecated("InstallTrapCallback");
	horatio_InstallTrapCallback(
		UserCallbackProc,
		AsHook
	);
	return;
}

void dpcrtlmm_RemoveTrapCallback(
	void(*CurrentCallbackProc)(
		const unsigned int TrapID,
		const char *TrapDesc
	)
) {
	deprecated("RemoveTrapCallback");
	horatio_RemoveTrapCallback(
		CurrentCallbackProc
	);
	return;
}

signed char dpcrtlmm_GetTrapCallbackInfo() {
	deprecated("GetTrapCallbackInfo");
	return horatio_GetTrapCallbackInfo();
}

unsigned char dpcrtlmm_ModifyDescriptorFlags(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr,
	const unsigned char *PNewFlags
) {
	deprecated("ModifyDescriptorFlags");
	return horatio_ModifyDescriptorFlags(
		PBlockArray,
		Ptr,
		PNewFlags
	);
}

void dpcrtlmm_SetBlockLockingFlag(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA* Ptr,
	const unsigned int NewStatus
) {
	deprecated("SetBlockLockingFlag");
	horatio_SetBlockLockingFlag(
		PBlockArray,
		Ptr,
		NewStatus
	);
	return;
}

unsigned int dpcrtlmm_IsBlockLocked(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr
) {
	deprecated("IsBlockLocked");
	return horatio_IsBlockLocked(
		PBlockArray,
		Ptr
	);
}

void dpcrtlmm_ToggleBlockLockingStatus(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *Ptr
) {
	deprecated("ToggleBlockLockingStatus");
	horatio_ToggleBlockLockingStatus(
		PBlockArray,
		Ptr
	);
	return;
}

unsigned char dpcrtlmm_AreTrapsEnabled() {
	deprecated("AreTrapsEnabled");
	return horatio_AreTrapsEnabled();
}

void dpcrtlmm_DisableTraps() {
	deprecated("DisableTraps");
	return dpcrtlmm_DisableTraps();
}

void dpcrtlmm_EnableTraps() {
	deprecated("EnableTraps");
	return horatio_EnableTraps();
}

unsigned long dpcrtlmm_GetBlockCount() {
	deprecated("GetBlockCount");
	return horatio_GetBlockCount();
}

void dpcrtlmm_GetStats(PS_HORATIO_STATS PReadStats) {
	deprecated("GetStats");
	horatio_GetStats(
		PReadStats
	);
	return;
}

void dpcrtlmm_Dump(FILE *Target) {
	deprecated("Dump");
	dpcrtlmm_Dump(
		Target
	);
	return;
}

PS_HORATIO_VERSION dpcrtlmm_Ver(
	PS_HORATIO_VERSION PVerStruct
) {
	deprecated("Ver");
	return horatio_Ver(
		PVerStruct
	);
}
