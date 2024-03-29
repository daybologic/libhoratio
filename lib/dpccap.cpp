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

// HORATIO 1.2 compatible encapsulation object for C++
// Add this file to your C++ project
// You can have this particular module it's free
// I'm not going to use new style C++ casts, I want this module to port
// to compilers pre-ANSI/ISO C++
// If you have any warnings, please raise an issue via the bitbucket.org
// web-site.
// but please make sure it's the full compiler output with compiler name
// and version.

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <stddef.h> /* For NULL */
#include <stdio.h>
#ifdef HORATIO_HDRSTOP // Same as in library source
# pragma hdrstop
#endif //HORATIO_HDRSTOP

#include "horatio.h" // Library header
#include "dpccap.h" // Class definitions

#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager MemManager;

// daybo::THORATIO_MemManager

// Constructor, starts HORATIO
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::THORATIO_MemManager()
  : firstAccess(1)
{
}

// Destructor, shuts down HORATIO
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::~THORATIO_MemManager() {
	if ( this->firstAccess == 0 ) { /* Has been accessed/started? */
		horatio_Shutdown();
		this->firstAccess = 1;
	}
}

// Explicit startup function
void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Startup() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::InstallDebugHook(
	const unsigned short HookType,
	unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_InstallDebugHook(HookType, NewHookProc);
}

unsigned
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::GetDebugHookChainCount(
  const unsigned int HookType
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_GetDebugHookChainCount(HookType);
}

unsigned
#ifndef __NO_NAMESPACES__
daybo::
#endif /*__NO_NAMESPACES__*/
THORATIO_MemManager::GetDebugHookMatrixCount() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_GetDebugHookMatrixCount();
}

unsigned int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::UninstallDebugHook(
	const unsigned short HookType,
	unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_UninstallDebugHook(HookType, HookProc2Remove);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Alloc(
	const size_t NewBlockSize
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_Alloc(NULL, NewBlockSize);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Alloc(
	const size_t NewBlockSize,
	const char* File,
	const unsigned int Line
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_AllocEx(NULL, NewBlockSize, File, Line);
}

// Don't bother shielding the library from NULLs, the library
// is perfectly capable of finding and reporting all the programmer's
// mistakes without this little fancy layer playing at being HORATIO
void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Free(
	void* Ptr
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_Free(NULL, Ptr);
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::IsDefaultBlockArray(
	PS_HORATIO_BLOCKDESCARRAY PBlockArray
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_IsDefaultBlockArray(PBlockArray);
}

size_t
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::GetBlockSize(
	const void* BlockPtr
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_GetBlockSize(NULL, (const void*)BlockPtr);
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::IsBadBlockPtr(
	const void* BlockPtr
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_IsBadBlockPtr(NULL, BlockPtr);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Realloc(
	void* OldBlockPtr,
	const size_t NewSize
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_Realloc(NULL, OldBlockPtr, NewSize);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Calloc(
	const unsigned int N,
	const size_t NewBlockSize
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_Calloc(NULL, N, NewBlockSize);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Calloc(
	const unsigned int N,
	const size_t NewBlockSize,
	const char* File,
	const unsigned int Line
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_CallocEx(NULL, N, NewBlockSize, File, Line);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::InstallTrapCallback(
	void(*UserCallbackProc)(
		const unsigned int TrapID,
		const char* TrapMessage
	),
	const unsigned int AsHook
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_InstallTrapCallback(UserCallbackProc, AsHook);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::RemoveTrapCallback(
	void(*CurrentCallbackProc)(
		const unsigned int TrapID,
		const char* TrapDesc
	)
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_RemoveTrapCallback(CurrentCallbackProc);
}

signed char
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::GetTrapCallbackInfo() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_GetTrapCallbackInfo();
}

unsigned char
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::ModifyDescriptorFlags(
	const void* Ptr,
	const unsigned char* PNewFlags
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_ModifyDescriptorFlags(NULL, Ptr, PNewFlags);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::SetBlockLockingFlag(
	const void* Ptr,
	const int NewStatus
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_SetBlockLockingFlag(NULL, Ptr, (const unsigned int)NewStatus);
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::IsBlockLocked(
	const void* Ptr
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_IsBlockLocked(NULL, Ptr);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::LockBlock(
	const void* pBlock
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_SetBlockLockingFlag(NULL, pBlock, (1U));
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::UnlockBlock(
	const void* pBlock
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_SetBlockLockingFlag(NULL, pBlock, (0U));
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::ToggleBlockLockingStatus(
	const void* Ptr
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_ToggleBlockLockingStatus(NULL, Ptr);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::EnableTraps() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	// Change HORATIO externed variable, quicker than a library call
	horatio__EnableTraps = 1U;
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::DisableTraps() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio__EnableTraps = 0U;
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::AreTrapsEnabled() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio__EnableTraps;
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::GetStats(
	PS_HORATIO_STATS PReadStats
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_GetStats(PReadStats);
}

unsigned long
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::GetBlockCount() {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_GetBlockCount();
}

PS_HORATIO_VERSION
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Ver(
	PS_HORATIO_VERSION PVerStruct
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	return horatio_Ver(PVerStruct);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_MemManager::Dump(
	FILE* Target
) {
	if ( this->firstAccess ) {
		horatio_Startup();
		this->firstAccess = 0;
	}
	horatio_Dump(Target);
}



// daybo::THORATIO_BlockArray

#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::THORATIO_BlockArray(
	bool doInit
):
	_PblockArray(NULL)
{
	if ( doInit ) this->Init();
}

#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::THORATIO_BlockArray():
  _PblockArray(NULL)
{
	this->Init();
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Init() {
	int ret = 0;

	if ( !_PblockArray ) {
		_PblockArray = horatio_CreateBlockArray();
		if ( _PblockArray ) ret = 1;
	}
	return ret;
}

#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::~THORATIO_BlockArray() {
	horatio_DestroyBlockArray(_PblockArray);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Alloc(
	const size_t NewBlockSize
) {
	return horatio_Alloc(_PblockArray, NewBlockSize);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Alloc(
	const size_t NewBlockSize,
	const char* File,
	const unsigned int Line
) {
	return horatio_AllocEx(_PblockArray, NewBlockSize, File, Line);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Free(
	void* Ptr
) {
	horatio_Free(_PblockArray, Ptr);
}

size_t
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::GetBlockSize(
	const void* BlockPtr
) const {
	return horatio_GetBlockSize(_PblockArray, (const void*)BlockPtr);
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::IsBadBlockPtr(
	const void* BlockPtr
) const {
	return horatio_IsBadBlockPtr(_PblockArray, BlockPtr);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Realloc(
	void* OldBlockPtr,
	const size_t NewSize
) {
	return horatio_Realloc(_PblockArray, OldBlockPtr, NewSize);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Calloc(
	const unsigned int N,
	const size_t NewBlockSize
) {
	return horatio_Calloc(_PblockArray, N, NewBlockSize);
}

void*
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::Calloc(
	const unsigned int N,
	const size_t NewBlockSize,
	const char* File,
	const unsigned int Line
) {
	return horatio_CallocEx(_PblockArray, N, NewBlockSize, File, Line);
}

unsigned char
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::ModifyDescriptorFlags(
	const void* Ptr,
	const unsigned char* PNewFlags
) {
	return horatio_ModifyDescriptorFlags(_PblockArray, Ptr, PNewFlags);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::SetBlockLockingFlag(
	const void* Ptr,
	int NewStatus
) {
	horatio_SetBlockLockingFlag(
		_PblockArray,
		Ptr,
		(const unsigned int)NewStatus
	);
}

int
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::IsBlockLocked(
	const void* Ptr
) const {
	return horatio_IsBlockLocked(_PblockArray, Ptr);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::LockBlock(
	const void* pBlock
) {
	horatio_LockBlock(_PblockArray, pBlock);
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::UnlockBlock(
	const void* pBlock
) {
	horatio_SetBlockLockingFlag(_PblockArray, pBlock, (0U));
}

void
#ifndef __NO_NAMESPACES__
daybo::
#endif
THORATIO_BlockArray::ToggleBlockLockingStatus(
	const void* Ptr
) {
	horatio_ToggleBlockLockingStatus(_PblockArray, Ptr);
}
