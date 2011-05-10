/*
    DPCRTLMM C++ encapsulation
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
Get updates: http://www.daybologic.co.uk/Dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/
// DPCRTLMM 1.2 compatible encapsulation object for C++
// Add this file to your C++ project
// You can have this particular module it's free
// Last modification: Dec 2000
// I'm not going to use new style C++ casts, I want this module to port
// to compilers pre-ANSI/ISO C++
// If you have any warnings mail them to me at Overlord@DayboLogic.co.uk
// but please make sure it's the full compiler output with compiler name
// and version.

#include <stddef.h> /* For NULL */
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP // Same as in library source
#  pragma hdrstop
#endif //DPCRTLMM_HDRSTOP

#include "dpcrtlmm.h" // Library header
#include "dpccap.h" // Class definitions

#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager MemManager;
//----------------------------------------------------------------------------
// Overlord::TDPCRTLMM_MemManager

// Constructor, starts DPCRTLMM
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::TDPCRTLMM_MemManager()
{
  dpcrtlmm_Startup();
}
//----------------------------------------------------------------------------
// Destructor, shuts down DPCRTLMM
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::~TDPCRTLMM_MemManager()
{
  dpcrtlmm_Shutdown();
}
//----------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::InstallDebugHook(const unsigned short HookType, unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO))
{
  return dpcrtlmm_InstallDebugHook(HookType, NewHookProc);
}
//----------------------------------------------------------------------------
unsigned
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::GetDebugHookChainCount(const unsigned int HookType)
{
  return dpcrtlmm_GetDebugHookChainCount(HookType);
}
//----------------------------------------------------------------------------
unsigned
#ifndef __NO_NAMESPACES__
Overlord::
#endif /*__NO_NAMESPACES__*/
TDPCRTLMM_MemManager::GetDebugHookMatrixCount()
{
  return dpcrtlmm_GetDebugHookMatrixCount();
}
//----------------------------------------------------------------------------
unsigned int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::UninstallDebugHook(const unsigned short HookType, unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO))
{
  return dpcrtlmm_UninstallDebugHook(HookType, HookProc2Remove);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Alloc(const size_t NewBlockSize)
{
  return dpcrtlmm_Alloc(NULL, NewBlockSize);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Alloc(const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  return dpcrtlmm_AllocEx(NULL, NewBlockSize, File, Line); 
}
//----------------------------------------------------------------------------
// Don't bother shielding the library from NULLs, the library
// is perfectly capable of finding and reporting all the programmer's
// mistakes without this little fancy layer playing at being DPCRTLMM
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Free(void* Ptr)
{
  dpcrtlmm_Free(NULL, Ptr);
}
//---------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::IsDefaultBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray)
{
  return dpcrtlmm_IsDefaultBlockArray(PBlockArray);
}
//----------------------------------------------------------------------------
size_t
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::GetBlockSize(const void* BlockPtr) const
{
  return dpcrtlmm_GetBlockSize(NULL, (void*)BlockPtr);
}
//----------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::IsBadBlockPtr(const void* BlockPtr) const
{
  return dpcrtlmm_IsBadBlockPtr(NULL, BlockPtr);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Realloc(void* OldBlockPtr, const size_t NewSize)
{
  return dpcrtlmm_Realloc(NULL, OldBlockPtr, NewSize);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Calloc(const unsigned int N, const size_t NewBlockSize)
{
  return dpcrtlmm_Calloc(NULL, N, NewBlockSize);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Calloc(const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  return dpcrtlmm_CallocEx(NULL, N, NewBlockSize, File, Line);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::InstallTrapCallback( void(*UserCallbackProc)(const unsigned int TrapID, const char* TrapMessage), const unsigned int AsHook )
{
  dpcrtlmm_InstallTrapCallback(UserCallbackProc, AsHook);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::RemoveTrapCallback( void(*CurrentCallbackProc)(const unsigned int TrapID, const char* TrapDesc) )
{
  dpcrtlmm_RemoveTrapCallback(CurrentCallbackProc);
}
//----------------------------------------------------------------------------
signed char
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::GetTrapCallbackInfo()
{
  return dpcrtlmm_GetTrapCallbackInfo();
}
//----------------------------------------------------------------------------
unsigned char
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags)
{
  return dpcrtlmm_ModifyDescriptorFlags(NULL, Ptr, PNewFlags);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::SetBlockLockingFlag(const void* Ptr, const int NewStatus)
{
  dpcrtlmm_SetBlockLockingFlag(NULL, Ptr, (const unsigned int)NewStatus);
}
//----------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::IsBlockLocked(const void* Ptr) const
{
  return dpcrtlmm_IsBlockLocked(NULL, Ptr);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::LockBlock(const void* pBlock)
{
  dpcrtlmm_SetBlockLockingFlag(NULL, pBlock, (1U));
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::UnlockBlock(const void* pBlock)
{
  dpcrtlmm_SetBlockLockingFlag(NULL, pBlock, (0U));
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::ToggleBlockLockingStatus(const void* Ptr)
{
  dpcrtlmm_ToggleBlockLockingStatus(NULL, Ptr);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::EnableTraps()
{
  // Change DPCRTLMM externed variable, quicker than a library call
  dpcrtlmm__EnableTraps = 1U;
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::DisableTraps()
{
  dpcrtlmm__EnableTraps = 0U;
}
//----------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::AreTrapsEnabled()
{
  //return dpcrtlmm_AreTrapsEnabled();
  return dpcrtlmm__EnableTraps; // Faster
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::GetStats(PS_DPCRTLMM_STATS PReadStats)
{
  dpcrtlmm_GetStats(PReadStats);
}
//----------------------------------------------------------------------------
unsigned long
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::GetBlockCount()
{
  return dpcrtlmm_GetBlockCount();
}
//----------------------------------------------------------------------------
PS_DPCRTLMM_VERSION
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Ver(PS_DPCRTLMM_VERSION PVerStruct)
{
  return dpcrtlmm_Ver(PVerStruct);
} 
//---------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager::Dump(FILE* Target)
{
  dpcrtlmm_Dump(Target);
}
//---------------------------------------------------------------------------


// Overlord::TDPCRTLMM_BlockArray

#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::TDPCRTLMM_BlockArray()
  :
  _PblockArray(NULL)
{
  _PblockArray = dpcrtlmm_CreateBlockArray();
}
//----------------------------------------------------------------------------
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::~TDPCRTLMM_BlockArray()
{
  dpcrtlmm_DestroyBlockArray(_PblockArray);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::Alloc(const size_t NewBlockSize)
{
  return dpcrtlmm_Alloc(_PblockArray, NewBlockSize);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::Alloc(const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  return dpcrtlmm_AllocEx(_PblockArray, NewBlockSize, File, Line);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::Free(void* Ptr)
{
  dpcrtlmm_Free(_PblockArray, Ptr);
}
//----------------------------------------------------------------------------
size_t
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::GetBlockSize(const void* BlockPtr) const
{
  return dpcrtlmm_GetBlockSize(_PblockArray, (void*)BlockPtr);
}
//----------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::IsBadBlockPtr(const void* BlockPtr) const
{
  return dpcrtlmm_IsBadBlockPtr(_PblockArray, BlockPtr);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::Realloc(void* OldBlockPtr, const size_t NewSize)
{
  return dpcrtlmm_Realloc(_PblockArray, OldBlockPtr, NewSize);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::Calloc(const unsigned int N, const size_t NewBlockSize)
{
  return dpcrtlmm_Calloc(_PblockArray, N, NewBlockSize);
}
//----------------------------------------------------------------------------
void*
#ifndef __NO_NaMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::Calloc(const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line)
{
  return dpcrtlmm_CallocEx(_PblockArray, N, NewBlockSize, File, Line);
}
//----------------------------------------------------------------------------
unsigned char
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags)
{
  return dpcrtlmm_ModifyDescriptorFlags(_PblockArray, Ptr, PNewFlags);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::SetBlockLockingFlag(const void* Ptr, int NewStatus)
{
  dpcrtlmm_SetBlockLockingFlag(_PblockArray, Ptr, (const unsigned int)NewStatus);
}
//----------------------------------------------------------------------------
int
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::IsBlockLocked(const void* Ptr) const
{
  return dpcrtlmm_IsBlockLocked(_PblockArray, Ptr);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::LockBlock(const void* pBlock)
{
  dpcrtlmm_LockBlock(_PblockArray, pBlock);
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::UnlockBlock(const void* pBlock)
{
  dpcrtlmm_SetBlockLockingFlag(_PblockArray, pBlock, (0U));
}
//----------------------------------------------------------------------------
void
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_BlockArray::ToggleBlockLockingStatus(const void* Ptr)
{
  dpcrtlmm_ToggleBlockLockingStatus(_PblockArray, Ptr);
}
//----------------------------------------------------------------------------
