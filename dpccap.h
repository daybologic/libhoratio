/*
    DPCRTLMM C++ user's interface header
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
// DPCRTLMM 1.2 compatible encapsulation header for C++
// include this and add dpccap.cpp to your project
// You can have this particular module, it's free
// Last update: December 2000
// I'm writing this in ANSI/ISO C++, that doesn't mean it'll work on all
// old compilers!  define __NO_NAMESPACES__ if you don't want the namespace.
// I've got rid of all inline functions as a wicked article on C++
// porting on Mozilla said not to bother with them more or less, they'd
// know, look how far Mozilla and Netscape has been ported!
// I know the keyword bool is new, so I'm not using it, I'll use int thanks

#ifndef __INC_DPCRTLMM_CAP_H
#define __INC_DPCRTLMM_CAP_H
//---------------------------------------------------------------------------
// main object, don't create more than one, the object is externed

#ifndef __NO_NAMESPACES__
namespace Overlord
{
#endif /*!__NO_NAMESPACES__*/
  class TDPCRTLMM_MemManager
  {
  public:
    TDPCRTLMM_MemManager();

    ~TDPCRTLMM_MemManager();

    int InstallDebugHook(const unsigned short HookType, unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO));

    unsigned GetDebugHookChainCount(const unsigned int HookType);

    unsigned GetDebugHookMatrixCount();

    unsigned int UninstallDebugHook(const unsigned short HookType, unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO));

    void* Alloc(const size_t NewBlockSize);

    void* Alloc(const size_t NewBlockSize, const char* File, const unsigned int Line);

    void Free(void* Ptr);

    // Only of use within a hook, see my docs for this function in DPCRTLMM, it's just the same
    int IsDefaultBlockArray(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray);

    // I shouldn't have been using void* in this function really, correct that
    // for the C++ layer
    size_t GetBlockSize(const void* BlockPtr) const;

    int IsBadBlockPtr(const void* BlockPtr) const;

    void* Realloc(void* OldBlockPtr, const size_t NewSize);

    void* Calloc(const unsigned int N, const size_t NewBlockSize);

    void* Calloc(const unsigned int N, const size_t NewBlockSize, const char* File, const unsigned int Line);

    void InstallTrapCallback( void(*UserCallbackProc)(const unsigned int TrapID, const char* TrapMessage), const unsigned int AsHook );

    void RemoveTrapCallback( void(*CurrentCallbackProc)(const unsigned int TrapID, const char* TrapDesc) );
      
    signed char GetTrapCallbackInfo();

    unsigned char ModifyDescriptorFlags(const void* Ptr, const unsigned char* PNewFlags);

    void SetBlockLockingFlag(const void* Ptr, int NewStatus);

    int IsBlockLocked(const void* Ptr) const;

    void LockBlock(const void* pBlock);

    void UnlockBlock(const void* pBlock);

    void ToggleBlockLockingStatus(const void* Ptr);

    void EnableTraps();
    void DisableTraps();
    int AreTrapsEnabled();

    void GetStats(PS_DPCRTLMM_STATS PReadStats);

    unsigned long GetBlockCount();

    PS_DPCRTLMM_VERSION Ver(PS_DPCRTLMM_VERSION PVerStruct);

    void Dump(FILE* Target);
  };

  class TDPCRTLMM_BlockArray
  {
  private:
      PS_DPCRTLMM_BLOCKDESCARRAY _PblockArray;
  public:
    TDPCRTLMM_BlockArray(); // Constructs the object, creates the block array

    // Destroys the block array, destructs the object
    ~TDPCRTLMM_BlockArray();

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
}; /*namespace Overlord*/
#endif

extern
#ifndef __NO_NAMESPACES__
Overlord::
#endif
TDPCRTLMM_MemManager MemManager; /* This is the object you should use */
//---------------------------------------------------------------------------
#endif //__INC_DPCRTLMM_CAP_H
