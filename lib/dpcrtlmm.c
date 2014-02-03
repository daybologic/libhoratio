/*
Horatio's Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer (M6KVM), Daybo Logic
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
#############################################################################
# Legacy wrapper functions (ABI compatibility).                             #
# All functions are designed to retain compatibility with existing apps     #
#############################################################################
*/
#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#undef DPCRTLMM_LEGACY /* Don't map our wrappers back to the horatio functions */
#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */

unsigned int dpcrtlmm_InstallDebugHook(
  const unsigned short HookType,
  unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
) {
	return horatio_InstallDebugHook(
		HookType,
		NewHookProc
	);
}

unsigned int dpcrtlmm_GetDebugHookChainCount(
  const unsigned int HookType
) {
	return horatio_GetDebugHookChainCount(
		HookType
	);
}

unsigned int dpcrtlmm_GetDebugHookMatrixCount() {
	return horatio_GetDebugHookMatrixCount();
}

unsigned int dpcrtlmm_UninstallDebugHook(
  const unsigned short HookType,
  unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
) {
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
	horatio_AllocEx(
		PBlockArray,
		NewBlockSize,
		File,
		Line
	);
	return;
}

void dpcrtlmm_Free(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *Ptr
) {
	horatio_Free(
		PBlockArray,
		Ptr
	);
	return;
}

PS_HORATIO_BLOCKDESCARRAY dpcrtlmm_CreateBlockArray() {
	return horatio_CreateBlockArray();
}

void dpcrtlmm_DestroyBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	horatio_DestroyBlockArray(
		PBlockArray
	);
	return;
}

unsigned int dpcrtlmm_IsDefaultBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	return horatio_IsDefaultBlockArray(
		PBlockArray
	);
}

void dpcrtlmm_Startup() {
	horatio_Startup();
	return;
}

void dpcrtlmm_Shutdown() {
	horatio_Shutdown();
	return;
}

unsigned int dpcrtlmm_IsStarted() {
	return horatio_IsStarted();
}

size_t dpcrtlmm_GetBlockSize(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *BlockPtr
) {
	return horatio_GetBlockSize(
		PBlockArray,
		BlockPtr
	);
}

unsigned int dpcrtlmm_IsBadBlockPtr(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *BlockPtr
) {
	return horatio_IsBadBlockPtr(
		PBlockArray,
		BlockPtr
	);
}

unsigned int dpcrtlmm_IsBadArrayPtr(
	return horatio_IsBadArrayPtr(
		PBlockArray
	);
);

void HORATIO_FARDATA *dpcrtlmm_Realloc(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *OldBlockPtr, const size_t NewSize
) {
	horatio_Realloc(
		PBlockArray,
		OldBlockPtr,
		NewSize
	);
	return;
}

void HORATIO_FARDATA *dpcrtlmm_CallocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const unsigned int N,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
) {
	horatio_CallocEx(
		PBlockArray,
		N,
		NewBlockSize,
		File,
		Line
	);
	return;
}

void dpcrtlmm_InstallTrapCallback(
  void(*UserCallbackProc)(
    const unsigned int TrapID,
    const char *TrapMessage
  ),
  const unsigned int AsHook
) {
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
	horatio_RemoveTrapCallback(
		CurrentCallbackProc
	);
	return;
}

signed char dpcrtlmm_GetTrapCallbackInfo() {
	return horatio_GetTrapCallbackInfo();
}

unsigned char dpcrtlmm_ModifyDescriptorFlags(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *Ptr,
  const unsigned char *PNewFlags
) {
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
	horatio_SetBlockLockingFlag(
		PBlockArray,
		Ptr,
		NewStatus
	);
	return;
}

unsigned int dpcrtlmm_IsBlockLocked(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA* Ptr
) {
	return horatio_IsBlockLocked(
		PBlockArray,
		Ptr
	);
}

void dpcrtlmm_ToggleBlockLockingStatus(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA* Ptr
) {
	horatio_ToggleBlockLockingStatus(
		PBlockArray,
		Ptr
	);
	return;
}

unsigned char dpcrtlmm_AreTrapsEnabled() {
	return horatio_AreTrapsEnabled();
}

void dpcrtlmm_DisableTraps() {
	return dpcrtlmm_DisableTraps();
}

void dpcrtlmm_EnableTraps() {
	return horatio_EnableTraps();
}

unsigned long dpcrtlmm_GetBlockCount() {
	return horatio_GetBlockCount();
}

void dpcrtlmm_GetStats(PS_HORATIO_STATS PReadStats) {
	horatio_GetStats(
		PReadStats
	);
	return;
}

void dpcrtlmm_Dump(FILE* Target) {
	dpcrtlmm_Dump(
		Target
	);
	return;
}

PS_HORATIO_VERSION dpcrtlmm_Ver(
	PS_HORATIO_VERSION PVerStruct
) {
	return horatio_Ver(
		PVerStruct
	);
}
