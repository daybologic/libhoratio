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
// DPCRTLMM 1.0 compatible encapsulation header for C++
// include this and add dpccap.cpp to your project
// You can have this particular module, it's free
// Last update: 3rd August 2000

#ifndef __INC_DPCRTLMM_CAP_H
#define __INC_DPCRTLMM_CAP_H
//---------------------------------------------------------------------------
// main object, don't create more than one, the object is externed

namespace DPCRTLMM
{
  class TMemManager
  {
  public:
    inline TMemManager()
      { dpcrtlmm_Startup(); }

    inline ~TMemManager()
      { dpcrtlmm_Shutdown(); }

    inline bool InstallDebugHook(const unsigned short HookType, unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO))
      { return (bool)dpcrtlmm_InstallDebugHook(HookType, NewHookProc); }

    inline unsigned GetDebugHookChainCount(const unsigned int HookType)
      { return dpcrtlmm_GetDebugHookChainCount(HookType); }

    inline unsigned GetDebugHookMatrixCount()
      { return dpcrtlmm_GetDebugHookMatrixCount(); }

    inline unsigned int UninstallDebugHook(const unsigned short HookType, unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO))
      { return dpcrtlmm_UninstallDebugHook(HookType, HookProc2Remove); }

    void* Alloc(const size_t NewBlockSize);

    // Don't bother shielding the library from NULLs, the library
    // is perfectly capable of finding and reporting all the programmer's
    // mistakes without this little fancy layer playing at being DPCRTLMM
    inline void Free(void* Ptr)
      { dpcrtlmm_Free(NULL, Ptr); }

    // Only of use within a hook, see my docs for this function in DPCRTLMM, it's just the same
    inline bool IsDefaultBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray)
      { return (bool)dpcrtlmm_IsDefaultBlockArray(PBlockArray); }

    // I shouldn't have been using void* in this function really, correct that
    // for the C++ layer
    inline size_t GetBlockSize(const void* BlockPtr) const
      { return dpcrtlmm_GetBlockSize(NULL, (void*)BlockPtr); }

    inline bool IsBadBlockPtr(const void* BlockPtr) const
      { return (bool)dpcrtlmm_IsBadBlockPtr(NULL, BlockPtr); }

    inline void* Realloc(void* OldBlockPtr, const size_t NewSize)
      { return dpcrtlmm_Realloc(NULL, OldBlockPtr, NewSize); }

    inline void* Calloc(const unsigned int N, const size_t NewBlockSize)
      { return dpcrtlmm_Calloc(NULL, N, NewBlockSize); }

    inline void InstallTrapCallback( void(*UserCallbackProc)(const unsigned int TrapID, const char* TrapMessage), const unsigned int AsHook )
      { dpcrtlmm_InstallTrapCallback(UserCallbackProc, AsHook); }

    inline void RemoveTrapCallback( void(*CurrentCallbackProc)(const unsigned int TrapID, const char* TrapDesc) )
      { dpcrtlmm_RemoveTrapCallback(CurrentCallbackProc); }
      
    inline signed char GetTrapCallbackInfo()
      { return dpcrtlmm_GetTrapCallbackInfo(); }

    inline unsigned char ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags)
      { return dpcrtlmm_ModifyDescriptorFlags(NULL, Ptr, PNewFlags); }

    inline void SetBlockLockingFlag(const void* Ptr, const bool NewStatus)
      { dpcrtlmm_SetBlockLockingFlag(NULL, Ptr, (const unsigned int)NewStatus); }

    inline bool IsBlockLocked(const void* Ptr) const
      { return (bool)dpcrtlmm_IsBlockLocked(NULL, Ptr); }

    inline void LockBlock(const void* pBlock)
      { dpcrtlmm_SetBlockLockingFlag(NULL, pBlock, (1U)); }

    inline void UnlockBlock(const void* pBlock)
      { dpcrtlmm_SetBlockLockingFlag(NULL, pBlock, (0U)); }

    inline void ToggleBlockLockingStatus(const void* Ptr)
      { dpcrtlmm_ToggleBlockLockingStatus(NULL, Ptr); }

    inline void EnableTraps() { dpcrtlmm_EnableTraps(); }
    inline void DisableTraps() { dpcrtlmm_DisableTraps(); }
    inline void AreTrapsEnabled() { dpcrtlmm_AreTrapsEnabled(); }

    inline void GetStats(PS_DPCRTLMM_STATS PReadStats)
      { dpcrtlmm_GetStats(PReadStats); }

    inline unsigned long GetBlockCount() { return dpcrtlmm_GetBlockCount(); }

    inline PS_DPCRTLMM_VERSION Ver(PS_DPCRTLMM_VERSION PVerStruct)
      { return dpcrtlmm_Ver(PVerStruct); }  
  };

  class TBlockArray
  {
  private:
      PS_DPCRTLMM_BLOCKDESCARRAY _PblockArray;
  public:
    TBlockArray(); // Constructs the object, creates the block array

    // Destroys the block array, destructs the object
    inline ~TBlockArray()
      { dpcrtlmm_DestroyBlockArray(_PblockArray); }

    void* Alloc(const size_t NewBlockSize); /* Allocates a block in this block array */

    inline void Free(void* Ptr)
      { dpcrtlmm_Free(_PblockArray, Ptr); }

    // dpcrtlmm should be using a constant pointer for this!
    inline size_t GetBlockSize(const void* BlockPtr) const
      { return dpcrtlmm_GetBlockSize(_PblockArray, (void*)BlockPtr); }

    inline bool IsBadBlockPtr(const void* BlockPtr) const
      { return (bool)dpcrtlmm_IsBadBlockPtr(_PblockArray, BlockPtr); }

    inline void* Realloc(void* OldBlockPtr, const size_t NewSize)
      { return dpcrtlmm_Realloc(_PblockArray, OldBlockPtr, NewSize); }

    inline void* Calloc(const unsigned int N, const size_t NewBlockSize)
      { return dpcrtlmm_Calloc(_PblockArray, N, NewBlockSize); }

    inline unsigned char ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags)
      { return dpcrtlmm_ModifyDescriptorFlags(_PblockArray, Ptr, PNewFlags); }

    inline void SetBlockLockingFlag(const void* Ptr, const bool NewStatus)
      { dpcrtlmm_SetBlockLockingFlag(_PblockArray, Ptr, (const unsigned int)NewStatus); }

    inline bool IsBlockLocked(const void* Ptr) const
      { return (bool)dpcrtlmm_IsBlockLocked(_PblockArray, Ptr); }

    inline void LockBlock(const void* pBlock)
      { dpcrtlmm_SetBlockLockingFlag(_PblockArray, pBlock, (1U)); }

    inline void UnlockBlock(const void* pBlock)
      { dpcrtlmm_SetBlockLockingFlag(_PblockArray, pBlock, (0U)); }

    inline void ToggleBlockLockingStatus(const void* Ptr)
      { dpcrtlmm_ToggleBlockLockingStatus(_PblockArray, Ptr); }
  };
}; /*namespace DPCRTLMM*/

extern DPCRTLMM::TMemManager MemManager; /* This is the object you should use */
//---------------------------------------------------------------------------
#endif //__INC_DPCRTLMM_CAP_H