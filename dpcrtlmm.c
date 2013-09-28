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

unsigned int horatio_GetDebugHookChainCount(
  const unsigned int HookType
);

unsigned int horatio_GetDebugHookMatrixCount(
  void
);
unsigned int horatio_UninstallDebugHook(
  const unsigned short HookType,
  unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
);
void HORATIO_FARDATA* horatio_AllocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
);
void horatio_Free(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *Ptr
);
PS_HORATIO_BLOCKDESCARRAY horatio_CreateBlockArray(
  void
);
void horatio_DestroyBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray
);
unsigned int horatio_IsDefaultBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray
);
void horatio_Startup(void);
void horatio_Shutdown(void);
unsigned int horatio_IsStarted(void);
size_t horatio_GetBlockSize(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *BlockPtr
);
unsigned int horatio_IsBadBlockPtr(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *BlockPtr
);
unsigned int horatio_IsBadArrayPtr(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray
);
void HORATIO_FARDATA *horatio_Realloc(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *OldBlockPtr, const size_t NewSize
);
void HORATIO_FARDATA *horatio_CallocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const unsigned int N,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
);
void horatio_InstallTrapCallback(
  void(*UserCallbackProc)(
    const unsigned int TrapID,
    const char *TrapMessage
  ),
  const unsigned int AsHook
);
void horatio_RemoveTrapCallback(
  void(*CurrentCallbackProc)(
    const unsigned int TrapID,
    const char *TrapDesc
  )
);
signed char horatio_GetTrapCallbackInfo(void);
unsigned char horatio_ModifyDescriptorFlags(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *Ptr,
  const unsigned char *PNewFlags
);
void horatio_SetBlockLockingFlag(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA* Ptr,
  const unsigned int NewStatus
);
unsigned int horatio_IsBlockLocked(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA* Ptr
);
void horatio_ToggleBlockLockingStatus(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA* Ptr
); /* If locked, unlocks, if unlocked, locks */
unsigned char horatio_AreTrapsEnabled(void);
void horatio_DisableTraps(void);
void horatio_EnableTraps(void);
extern unsigned char horatio__EnableTraps; /* Obsolete */
unsigned long horatio_GetBlockCount(void); /* Returns number of allocated blocks */
void horatio_GetStats(PS_HORATIO_STATS PReadStats);
void horatio_Dump(FILE* Target); /* Dumps a table of all active allocations with lots of detail */
PS_HORATIO_VERSION horatio_Ver(PS_HORATIO_VERSION PVerStruct);
