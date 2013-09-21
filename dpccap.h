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
// HORATIO 1.2 compatible encapsulation header for C++
// include this and add dpccap.cpp to your project
// You can have this particular module, it's free
// I'm writing this in ANSI/ISO C++, that doesn't mean it'll work on all
// old compilers!  define __NO_NAMESPACES__ if you don't want the namespace.
// I've got rid of all inline functions as a wicked article on C++
// porting on Mozilla said not to bother with them more or less, they'd
// know, look how far Mozilla and Netscape has been ported!
// I know the keyword bool is new, so I'm not using it, I'll use int thanks

#ifndef INC_HORATIO_CAP_H
#define INC_HORATIO_CAP_H

// main object, don't create more than one, the object is externed

#ifndef __NO_NAMESPACES__
namespace Overlord {
#endif /*!__NO_NAMESPACES__*/
  class THORATIO_MemManager {
  private:
    int firstAccess;
  public:
    THORATIO_MemManager();

    ~THORATIO_MemManager();

    void Startup(); // Explicit startup (not necessary but you can call it)

    int InstallDebugHook(const unsigned short HookType, unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO));

    unsigned GetDebugHookChainCount(const unsigned int HookType);

    unsigned GetDebugHookMatrixCount();

    unsigned int UninstallDebugHook(const unsigned short HookType, unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO));

    void* Alloc(const size_t NewBlockSize);

    void* Alloc(const size_t NewBlockSize, const char* File, const unsigned int Line);

    void Free(void* Ptr);

    // Only of use within a hook, see my docs for this function in HORATIO, it's just the same
    int IsDefaultBlockArray(PS_HORATIO_BLOCKDESCARRAY PBlockArray);

    // I shouldn't have been using void* in this function really, correct that
    // for the C++ layer
    size_t GetBlockSize(const void* BlockPtr);

    int IsBadBlockPtr(const void* BlockPtr);

    void* Realloc(void* OldBlockPtr, const size_t NewSize);

    void* Calloc(const unsigned int N, const size_t NewBlockSize);

    void* Calloc(const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line);

    void InstallTrapCallback( void(*UserCallbackProc)(const unsigned int TrapID, const char* TrapMessage), const unsigned int AsHook );

    void RemoveTrapCallback( void(*CurrentCallbackProc)(const unsigned int TrapID, const char* TrapDesc) );
      
    signed char GetTrapCallbackInfo();

    unsigned char ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags);

    void SetBlockLockingFlag(const void* Ptr, const int NewStatus);

    int IsBlockLocked(const void* Ptr);

    void LockBlock(const void* pBlock);

    void UnlockBlock(const void* pBlock);

    void ToggleBlockLockingStatus(const void* Ptr);

    void EnableTraps();
    void DisableTraps();
    int AreTrapsEnabled();

    void GetStats(PS_HORATIO_STATS PReadStats);

    unsigned long GetBlockCount();

    PS_HORATIO_VERSION Ver(PS_HORATIO_VERSION PVerStruct);

    void Dump(FILE* Target);
  };

  class THORATIO_BlockArray {
  private:
      PS_HORATIO_BLOCKDESCARRAY _PblockArray;
  public:
    THORATIO_BlockArray(bool Init);
    THORATIO_BlockArray();
    int Init();

    // Destroys the block array, destructs the object
    ~THORATIO_BlockArray();

    void* Alloc(const size_t NewBlockSize); /* Allocates a block in this block array */

    void* Alloc(const size_t NewBlockSize, const char* File, const unsigned int Line);

    void Free(void* Ptr);

    // dpcrtlmm should be using a constant pointer for this!
    size_t GetBlockSize(const void* BlockPtr) const;

    int IsBadBlockPtr(const void* BlockPtr) const;

    void* Realloc(void* OldBlockPtr, const size_t NewSize);

    void* Calloc(const unsigned int N, const size_t NewBlockSize);

    void* Calloc(const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line);

    unsigned char ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags);

    void SetBlockLockingFlag(const void* Ptr, int NewStatus);

    int IsBlockLocked(const void* Ptr) const;

    void LockBlock(const void* pBlock);

    void UnlockBlock(const void* pBlock);

    void ToggleBlockLockingStatus(const void* Ptr);
  };

#ifndef __NO_NAMESPACES__
} /*namespace Overlord*/
#endif

extern
#ifndef __NO_NAMESPACES__
Overlord::
#endif
THORATIO_MemManager MemManager; /* This is the object you should use */

/* Legacy support */
#define TDPCRTLMM_MemManager THORATIO_MemManager
#define TDPCRTLMM_BlockArray THORATIO_BlockArray

#endif //!INC_HORATIO_CAP_H
