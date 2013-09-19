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
#ifndef INC_HORATIO_H
# define INC_HORATIO_H
# define INC_DPCRTLMM_H /* Legacy sentry */
/*-------------------------------------------------------------------------*/
/*
Created 15th Feb 2000
Author: David Duncan Ross Palmer (M6KVM), Daybo Logic.
File: horatio.h
Description: Horatio's Memory Manager
Comments:
          What is the purpose of this library?
          It is a memory tracking tool.  Although it can't detect memory
          over runs on pointer accesses or corruption.  It can be used
          to detect failures to release memory (as long as it's allocated
          by us) and attempts to release memory which we do not own.
          It has an automatic garbage collection feature to free up all
          unfreed memory too.  It should be used by the entire program to
          be most effective, it can provide statistics on memory left an
          such like too.  The best thing about the MM is that it provides
          a central place for dynamic memory allocation in a program so
          we can trace all activity, an optional log mode is avialable
          though a define.  Swapping cannot be implemented as the MM has
          no control over pointer-based memory accesses.  This is indeed
          why we don't see memory overruns. If the program is going to
          be released even though it contains memory bugs, the traps can
          be turned off or passed to a handler which will only execute the
          serious ones.

Direction: We intend to log statistics to optional back-end systems such
          as MongoDB, MySQL and SQLite.  Although this work is ongoing,
          it has yet to be merged into mainline.

          At some point we will write a dummy library, which will allow
          programs which are designed around libhoratio to effectively
          remove the library, without being re-written.

Language: Everything here is portable to ANSI C, We try our best to make
this library as portable as possible and appreciate any feedback for /any/
system on which a flaw is seen, whether this is embedded or games console.

*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
  Users who don't have dpc_build.h won't have HORATIO_FARDATA defined,
  so just remove it to stop any problems happening.

  FIXME: The above comment does not appear to make sense, given the
  following code.  Further investigation is necessary.
*/
#ifndef HORATIO_FARDATA
#  define HORATIO_FARDATA
#  define DPCRTLMM_FARDATA /* Legacy */
#endif /*!HORATIO_FARDATA*/

/* File / Line: This might be paranoia but I don't know if __FILE__ and
__LINE__ are defined on every compiler.  I'm not taking the risk */
#ifndef __FILE__
#  define __FILE__ "unknown"
#endif /*!__FILE__*/

#ifndef __LINE__
#  define __LINE__ (0)
#endif /*!__LINE__*/


/*
   Special flags, 0-Locked - Cannot be freed or resized by the library
   1-NoSwap - Do not swap out (unimplemented)
   2-7 - Reserved, must be zero
*/
typedef struct _S_HORATIO_BLOCKDESCRIPTOR { /* A block descriptor */
  void HORATIO_FARDATA *PBase; /* Raw pointer to the base address of the block */
  size_t Size; /* Number of bytes which block is in length */
  unsigned char Flags;
  unsigned int SourceLine; /* Line number at which the block was allocated */
  char *SourceFile; /* Dynamic, filename of place where allocation was requested */
} S_HORATIO_BLOCKDESCRIPTOR, HORATIO_FARDATA *PS_HORATIO_BLOCKDESCRIPTOR,
  S_DPCRTLMM_BLOCKDESCRIPTOR, DPCRTLMM_FARDATA *PS_DPCRTLMM_BLOCKDESCRIPTOR;

typedef struct _S_HORATIO_BLOCKDESCARRAY { /* Array of block descriptors */
  unsigned int Count; /* Number of elements (block descriptors) */
  PS_HORATIO_BLOCKDESCRIPTOR Descriptors; /* Raw array of blocks descs (each element is a BLOCKDESCRIPTOR, NOT a pointer to a BLOCKDESCRIPTOR) */
} S_HORATIO_BLOCKDESCARRAY, HORATIO_FARDATA *PS_HORATIO_BLOCKDESCARRAY,
  S_DPCRTLMM_BLOCKDESCARRAY, HORATIO_FARDATA *PS_DPCRTLMM_BLOCKDESCARRAY;

typedef struct _S_HORATIO_STATS { /* Statistics info for horatio_GetStats() */
  struct {
    unsigned long Allocated, /* Blocks allocated */
    Locked, /* Number of blocks locked (cannot be released), <= Allocated */
    Unswappable, /* Number of blocks which may not be swapped */
    Peak; /* The peak number of blocks allocated */
  } Blocks;
  struct {
    unsigned long Allocated, /* Contains number of bytes used by all blocks */
    Peak; /* The peak of memory usage, how much was used by all the blocks at one time (Allocated's peak) */
  } Charge;
} S_HORATIO_STATS, HORATIO_FARDATA *PS_HORATIO_STATS,
  S_DPCRTLMM_STATS, HORATIO_FARDATA *PS_DPCRTLMM_STATS;

typedef struct _S_HORATIO_VERSION {
  unsigned char Major, Minor, Patch;
  unsigned char Flags; /* See mnemonics below */
} S_HORATIO_VERSION, HORATIO_FARDATA *PS_HORATIO_VERSION,
  S_DPCRTLMM_VERSION, HORATIO_FARDATA *PS_DPCRTLMM_VERSION;

/* Version flag mnemonics (1.1.6) */
#define HORATIO_VERSION_DEBUG    (0x1)
#define HORATIO_VERSION_SPECIAL  (0x2)
#define HORATIO_VERSION_PRIVATE  (0x4)
#define HORATIO_VERSION_BETA     (0x8)
#define HORATIO_VERSION_MT      (0x10) /* Multi-threaded */

/* Legacy version flags */
#define DPCRTLMM_VERSION_DEBUG (HORATIO_VERSION_DEBUG)
#define DPCRTLMM_VERSION_SPECIAL (HORATIO_VERSION_SPECIAL)
#define DPCRTLMM_VERSION_PRIVATE (HORATIO_VERSION_PRIVATE)
#define DPCRTLMM_VERSION_BETA (HORATIO_VERSION_BETA)
#define DPCRTLMM_VERSION_MT (HORATIO_VERSION_MT)
/*----------------------- D E B U G   H O O K S ---------------------------*/

/* These are the definitions for debug hooks, the hooks can be installed
with a routine which will be described later, for now, these types
are related to the installation, chained storage and execution of the
debug hooks. - This support was added so that the user program could
generate statistics on how many blocks were allocated and such like. */

typedef struct _S_HORATIO_DEBUGHOOKINFO { /* Information passed to hooks */
  PS_HORATIO_BLOCKDESCARRAY PRelArr; /* Relevent block array (if NULL ignore), if resizing this is the old state */
  PS_HORATIO_BLOCKDESCRIPTOR PRelDesc; /* Relevent block descriptor (if NULL ignore) */
  unsigned short HookType; /* The identifier is used so that a hook function can determine the hooktype so that it can handle more than one type of hook if it wants to */
  unsigned int AllocReq; /* Allocation / new size request */
  unsigned int Success; /* If set call was successful, if FALSE the call failed or caused a trap */
  unsigned long Misc0; /* Not reserved, used for extra information which is hook type dependant */
  unsigned long Misc1;
  /* Reserved for future use, all are zero at the moment */
  unsigned int ReservedUINT;
  unsigned short ReservedUSHORT;
  unsigned long ReservedULONG;

} S_HORATIO_DEBUGHOOKINFO, HORATIO_FARDATA *PS_HORATIO_DEBUGHOOKINFO,
  S_DPCRTLMM_DEBUGHOOKINFO, HORATIO_FARDATA *PS_DPCRTLMM_DEBUGHOOKINFO;

/* Hook types, used for determining hook events or installing / removing hook functions,
only one call to the hook function is called per routine call, if Success is
set to FALSE, some of the other parameter may not be valid, always test the
pointers prior to access */

#define HORATIO_HOOK_ALLOC ((unsigned short)(0x0000U))
#define DPCRTLMM_HOOK_ALLOC (HORATIO_HOOK_ALLOC)
/* Allocation block request (success), PRelArr, PRelDesc, HookType, AllocReq
are all set, AllocReq is used for consistency but the information can also
be obtained from the block descriptor */

#define HORATIO_HOOK_FREE ((unsigned short)(0x0001U))
#define DPCRTLMM_HOOK_FREE (HORATIO_HOOK_FREE)
/* Release block request (success), PRelArr set, PRelDesc is no longer
valid so don't try and access the base pointer through it, it is the state
of the descriptor before the release, HookType is set. AllocReq is set to
PRelDesc->Size, although this is a reverse use of the member since it is
not an allocation request, it is a release request, still it seems a
suitable purpose that doesn't stray too far across the bounries of taste
and decency. */

#define HORATIO_HOOK_CREATEBLOCKARRAY ((unsigned short)(0x0002U))
#define DPCRTLMM_HOOK_CREATEBLOCKARRAY (HORATIO_HOOK_CREATEBLOCKARRAY)
/* Create a new block (success), PRelArr is set to the new block array,
PRelDesc is not used, HookType is set, AllocReq is set to
sizeof(S_HORATIO_BLOCKDESCARRAY) + sizeof(PS_HORATIO_BLOCKDESCARRAY),
in other words the size of the new block descriptor array object and the
new pointer to hold it in the safety list, this sort of information is
mainly for memory statistics. */

#define HORATIO_HOOK_DESTROYBLOCKARRAY ((unsigned short)(0x0003U))
#define DPCRTLMM_HOOK_DESTROYBLOCKARRAY (HORATIO_HOOK_DESTROYBLOCKARRAY)
/* Destruction of a block descriptor array, PRelArr is set to the array
which was just destroyed, or requested to be destroyed if the function
failed. PRelDesc is always NULL, HookType is set, AllocReq is not used. */

#define HORATIO_HOOK_STARTUP ((unsigned short)(0x0004U))
#define DPCRTLMM_HOOK_STARTUP (HORATIO_HOOK_STARTUP)
/* Although the hook is valid, this shouldn't happen because the hook chain
is not prepared for use before Startup is called, the hook will be executed
if Startup() is ever called again.  The parameters (apart from Success) will
nbot be used and of course Success will be FALSE (0U) because multiple calls
of Startup() are NOT allowed! */

#define HORATIO_HOOK_SHUTDOWN ((unsigned short)(0x0005U))
#define DPCRTLMM_HOOK_SHUTDOWN (HORATIO_HOOK_SHUTDOWN)
/* The hook is used when Shutdown() is called, the hook should only be
executed once in a properly written program, if it is called more than once
then the program has problems! */

/* Hook are not available for GetBlockSize() & IsBad...() functions as there
is little point in debugging the calls. */

#define HORATIO_HOOK_REALLOC ((unsigned short)(0x0006U))
#define DPCRTLMM_HOOK_REALLOC (HORATIO_HOOK_REALLOC)
/* Called on a reallocation request, descriptor array set, block ptr is set
to old block. HookType is set. AllocReq is set to the size difference between
the old size and the new size.  Misc0 is used: It has bit 0 set if the
AllocReq is a negative number.  Misc1 should be cast to (void*), it is the
address of the new block. */

#define HORATIO_HOOK_CALLOC ((unsigned short)(0x0007U))
#define DPCRTLMM_HOOK_CALLOC (HORATIO_HOOK_CALLOC)
/* Called on a Callocation! PRelArr is set, PRelDesc is set only on Success
HookType is set. AllocReq is the N * NewBlockSize which is passed to
calloc(). */

#define HORATIO_HOOK_INSTTRAPCALLBACK ((unsigned short)(0x0008U))
#define DPCRTLMM_HOOK_INSTTRAPCALLBACK (HORATIO_HOOK_INSTTRAPCALLBACK)
/* A trap handler was installed (or an attempt was made): If Success was
set TRUE Misc0 is the pointer to the trap handler which must be cast into
the correct pointer type. HookType is set, Misc1 is has bit 0 set if
the handler was infact in hook mode */

#define HORATIO_HOOK_REMTRAPCALLBACK ((unsigned short)(0x0009U))
#define DPCRTLMM_HOOK_REMTRAPCALLBACK (HORATIO_HOOK_REMTRAPCALLBACK)
/* A trap handler was removed (or an attempt was made): Misc0 is the handler
which was just removed (it might be a very bad idea to call it incase of
something mental like dynamic code in a self modifying program or virus.
Success is set TRUE or FALSE but Misc0 is set regardless. */

/* No hook is provided for GetTrapCallbackInfo() because debugging that
doesn't have a lot of point. */

#define HORATIO_HOOK_MODIFYDESCFLAGS ((unsigned short)(0x000AU))
#define DPCRTLMM_HOOK_MODIFYDESCFLAGS (HORATIO_HOOK_MODIFYDESCFLAGS)
/* Called dpcrtlmm_ModifyDescriptorFlags(), block and array pointers are
set, HookType is set, AllocReq is not set. Success is set and should be
checked first. Misc0 (lo-word's lo-byte) set to old flags.  New flags are
set in Misc1's lo-word's lo-byte. */

/* No hooks are available for locking functions as monitoring the descriptor
flags (bit 0) is enough to be able to monitor the locking status. */

/* This hook no longer exists
#define HORATIO_HOOK_TRAPENABLEMENT ((unsigned short)(0x000BU))
*/

/* This hook no longer exists, it could not be supported!
#define HORATIO_HOOK_INSTHOOK ((unsigned short)(0x000BU))
#define HORATIO_HOOK_REMHOOK ((unsigned short)(0x000CU))
*/

#define HORATIO_HOOK_ALL ((unsigned short)(~0U))
#define DPCRTLMM_HOOK_ALL (HORATIO_HOOK_ALL)
/* This special value can be sent to the trap installation / removal
to install the trap handler into all hook types, the program could then
have one handler which looks at HookType to determine the action, this
minimizes function exit and entry code it the program installs all hooks,
or if the code is similar in all hook procs. */

#define HORATIO_DEBUGHOOK_LASTHOOK ( HORATIO_HOOK_MODIFYDESCFLAGS ) /* Last available hook type */
#define DPCRTLMM_DEBUGHOOK_LASTHOOK (HORATIO_DEBUGHOOK_LASTHOOK)

/* This is the definition of a hook function for reference only :
        BOOL (*HookFunc)( PS_HORATIO_DEBUGHOOKINFO PDebugHookInfo );

        Information: PDebugHookInfo on the stack of the hook executive when the
        hook function sees it through a pointer, the hook function can take
        all the information it wants and tamper with details if it wishes
        (I take no responsibility), when the hook function returns the information
        (modified or not) is passed to the next entry in the hook chain for this
        type, to prevent the information being passed to the next hook return
        FALSE.  As the information is on the stack of the hook executive it has
        been copied from other sources (the functions which were called in the
        first place), therefore changing the debug hook info does not effect
        the actual memory manager's main copies, just the information recieved
        by the next hook.
*/

/* The debug hook executive is internal to the library and cannot
be be called from outside (well it is possible with a bit of
trickery if the name is known, put without the prototype the
program might crash), anyway I'm begining to ramble again so let's
get back to the point.  There are lots of user functions (as
listed below), they are not entrypoints into the debug hook
executive because the executive is contained in one function, they
are support functions which are contained in the same module as
the executive, call them freely as I have designated for user
callings, infact they aren't even called from inside. */

unsigned int dpcrtlmm_InstallDebugHook(
  const unsigned short HookType,
  unsigned int(*NewHookProc)(PS_HORATIO_DEBUGHOOKINFO)
);

/*
  Counts the number of hooks installed for this type of hook
*/
unsigned int dpcrtlmm_GetDebugHookChainCount(
  const unsigned int HookType
);

/*
  Counts the number of hooks in the entire debug hook matrix,
  that is the total number of hooks for all types
*/
unsigned int dpcrtlmm_GetDebugHookMatrixCount(
  void
);

/*
  Remove the said hook, type of hook must be specified in order to find it
*/
unsigned int dpcrtlmm_UninstallDebugHook(
  const unsigned short HookType,
  unsigned int(*HookProc2Remove)(PS_HORATIO_DEBUGHOOKINFO)
);

/*------------------ E N D   D E B U G   H O O K S ------------------------*/

/* All of the functions below expect a pointer to the block descriptor
array, which describes how many blocks their are, a pointer to all of
their descriptors, which leads on to special information, base addresses
and block sizes.  To obtain one of these (their should only be one per
program or per module), call CreateBlockArray(), before program shutdown
call DestroyBlockArray(), at which point all unallocated memory will be
trapped. */

/* Alloc() - NewBlockSize - Pass size in bytes of new block, the block
is allocated and the pointer is returned, which must be typecasted to
a pointer to the desired type before it is accessed so C knows how to
do pointer arithmetic.  NULL is returned if there is not enough continuous
memory to allocate a block of NewBlockSize. */
void HORATIO_FARDATA* dpcrtlmm_AllocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
);

/* Alloc() "backwards compatibillity", it's actually a lot easier to use
this version, so I recommend it.  This adds transparent file/line support for
the logs. */
#define dpcrtlmm_Alloc(blkarray, blksize) \
        dpcrtlmm_AllocEx((blkarray), (blksize), (__FILE__), (__LINE__))

/* Free() - BlockPtr - Pass a pointer to the block to free, attempting to
free a block we don't own will cause a trap which crashes the program,
passing the wrong block descriptor array would cause this problem too of
cause because Free() wouldn't be able to see it. */
void dpcrtlmm_Free(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *Ptr
);

/* CreateBlockArray() takes no parameters and returns a dynamically
allocated array of blocks which the user should not play with, all the
neccersary functions are here for manipulating the contained data safely.
The function DestroyBlockArray() must be called on the array pointer
once all memory management tasks have ended. Traps can be fired if the
wrong pointer is passed to the Freer, or if Shutdown() is called before
all arrays are destroyed.  It is possible that not enough memory can
be allocated, if so NULL is returned, a trap is not fired, perhaps the
caller can cope or crash the program with a trap themselves. */
PS_HORATIO_BLOCKDESCARRAY dpcrtlmm_CreateBlockArray(
  void
);

void dpcrtlmm_DestroyBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray
);

/* In debug hooks and the like, the user may be returned a pointer
which does not corrospond to any of the pointers which were allocated,
in this case it may be the resolved NULL pointer to the default array
because the array is not directly available for comparison use this function
to determine if that is the case, it will also report 1U (TRUE) for NULL */
unsigned int dpcrtlmm_IsDefaultBlockArray(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray
);

/* Before any of these functions are used, internal initialization of the
memory manager is neccersary and must be done ONCE and ONLY ONCE per
program initialization.  This is done with the function Startup()
*/
void dpcrtlmm_Startup(void);

/* Normally unfreed memory is trapped during the destroying of an array,
however, what if an entire array for a program or module is unreleased,
this is where the library cleanup can help.  When the library was started to
be used, the library function Startup() was used, so call Shutdown() before
the program ends to guarentee all memory was released. */
void dpcrtlmm_Shutdown(void);

/* To test whether the library is started call this, normally use for
conditional startup/shutdown when it is not clear whether a host program
is already using HORATIO (if you're building a library which uses
HORATIO. */
unsigned int dpcrtlmm_IsStarted(void);

/*
  GetBlockSize() - BlockPtr - Pass a pointer to the block
  the size of the block is returned, a trap is fired stating
  "Attempt to GetBlockSize() unknown block at base 0x????????,
  in array base: 0x????????" A nonzero value is always returned,
  if traps are disabled anything could happen when we try to retrieve
  the block size!
*/
size_t dpcrtlmm_GetBlockSize(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *BlockPtr
);

/*
  IsBadBlockPtr() - BlockPtr - Pass a pointer to the block base address,
  the function only fires a trap if the array base address is invalid.
  If the base address of the block is not found in the array of descriptors
  the function returns TRUE (meaning, TRUE the block pointer is bad), if
  FALSE then accessing the block is safe.  This does not check for
  corruption or anything, just checks the pointer is valid.  Checksums for
  block are NOT maintained, otherwise direct pointer write access for the
  blocks for users would not be an option.
*/
unsigned int dpcrtlmm_IsBadBlockPtr(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *BlockPtr
);

/* IsBadArrayPtr() - PblockArray - A different use for our block array pointer,
verifies it's a valid address assigned by CreateBlockArray(), does not fire
any traps, TRUE is returned if the address is bad, FALSE is returned if the
array of block descriptors is safe. */
unsigned int dpcrtlmm_IsBadArrayPtr(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray
);

/* Some other operations we might want to do on memory which is our
responsibility... */

/* Realloc() - OldBlockPtr - Used to shrink or enlarge the block, blocks are
often relocated by the operating system's memory manager to try to prevent
linear address fragmentation, therefore the pointer may change, so always
accept the return value unless it is NULL.  NULL is returned when the block
could not be enlarged or relocated to an area where continuous memory is
large enough, in short, there is not enough memory to enlarge the block.
NewSize - New size in bytes of the block. */
void HORATIO_FARDATA *dpcrtlmm_Realloc(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  void HORATIO_FARDATA *OldBlockPtr, const size_t NewSize
);

/* Calloc() - N - Number of items
                                  NewBlockSize - Size of one item
        Allocates a block of (N * NewBlockSize), used for allocating an array
        of items in one go.  Just calls Alloc() with (N * NewBlockSize), I hardly
        ever use is personally, the C run-time library uses it though so I might
        as well do so too.  NULL is returned if the block cannot be allocated.
*/
void HORATIO_FARDATA *dpcrtlmm_CallocEx(
  PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const unsigned int N,
  const size_t NewBlockSize,
  const char *File,
  const unsigned int Line
);

#define dpcrtlmm_Calloc(blkarray, n, blksize) \
        dpcrtlmm_CallocEx((blkarray), (n), (blksize), (__FILE__), (__LINE__))

/* When a trap is fired, the default behaviour of the program is to put
the message on stderr and call abort, this behaviour can be changed to
call (for example) the Daybo Logic fatal trap handler (which puts up a
blue screen), or something like that.  The trap handler takes a constant
pointer to a string (const char*), it should not return it should always
terminate the program.  Perhaps memory errors could be ignored by not
doing so but behaviour of the program afterwards might be undefined.
If the specified function is installed as a hook it is called when a
trap is fired but if it returns the default library handler is called
anyway.  It just gets a preview.  Do not try to remove a handler by
passing NULL to this function, use dpcrtlmm_RemoveTrapCallback() */
void dpcrtlmm_InstallTrapCallback(
  void(*UserCallbackProc)(
    const unsigned int TrapID,
    const char *TrapMessage
  ),
  const unsigned int AsHook
);

/*
  These are other misc. trap handler functions. ---
  Remove a trap handler which was installed earlier, must pass address so we
  know you know and are not making a dreadful mistake
*/
void dpcrtlmm_RemoveTrapCallback(
  void(*CurrentCallbackProc)(
    const unsigned int TrapID,
    const char *TrapDesc
  )
);

/*
  Returns -1 for no user trap handler, 0 indicates it is installed as
  a handler, 1 as a hook.
*/
signed char dpcrtlmm_GetTrapCallbackInfo(void);

/* These functions implement direct flag access (not reconmended except
for advanced programmers), casual programmers should use the specialist
locking functions or other functions which change the flags but don't
actually explictly state that is what they are doing, these flag modifying
functions are usually used by the library internally and not by the
users of the library, I have only made them available to the users because
of unforeseen circumstances arising, it's not very likely that users will
need to use these functions. */

/* dpcrtlmm_ModifyDescriptorFlags() - Pass pointer to new flags, old flags
are returned, to get the flags without modifying them, pass NULL as this
pointer.  It is feasible that the function could return after a trap if
traps are off or a user handler is called for the trap, which returned.
It is not possible to tell, if anything is suspected on running the
program, at redesign your code so that it does not use a user trap (just
comment out the line installing the handler, or don't turn off trapping) */
unsigned char dpcrtlmm_ModifyDescriptorFlags(
  const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
  const void HORATIO_FARDATA *Ptr,
  const unsigned char *PNewFlags
);

/* These functions deal with locking, when a block of memory is locked,
it means it cannot be freed or resized, if a trap is fired as a result of
the call, the function ModifyDescriptorFlags() is mentioned as the location
of the trap, so don't get confused! */
void dpcrtlmm_SetBlockLockingFlag(PS_HORATIO_BLOCKDESCARRAY PBlockArray, const void HORATIO_FARDATA* Ptr, const unsigned int NewStatus);
unsigned int dpcrtlmm_IsBlockLocked(PS_HORATIO_BLOCKDESCARRAY PBlockArray, const void HORATIO_FARDATA* Ptr);
/* These next two are simple shortcuts and are therefore implemented as macros */
#define dpcrtlmm_LockBlock(pArr, pBlock) dpcrtlmm_SetBlockLockingFlag(pArr, pBlock, (1U));
#define dpcrtlmm_UnlockBlock(pArr, pBlock) dpcrtlmm_SetBlockLockingFlag(pArr, pBlock, (0U));
void dpcrtlmm_ToggleBlockLockingStatus(PS_HORATIO_BLOCKDESCARRAY PBlockArray, const void HORATIO_FARDATA* Ptr); /* If locked, unlocks, if unlocked, locks */

/* These are for switching on and off traps.  dpcrtlmm__EnableTraps the
variable itself is supported for backward compatibillty only.  Internally
in the library it is used of course but direct access by the user of the
library is not thread safe, it is more preferable that the functions
be used as provided. */

unsigned char dpcrtlmm_AreTrapsEnabled(void);
void dpcrtlmm_DisableTraps(void);
void dpcrtlmm_EnableTraps(void);

extern unsigned char dpcrtlmm__EnableTraps; /* Obsolete */

/* Statistics functions */
unsigned long dpcrtlmm_GetBlockCount(void); /* Returns number of allocated blocks */
/* Look at the structure S_HORATIO_STATS, it's all returned in the
structure you pass */
void dpcrtlmm_GetStats(PS_HORATIO_STATS PReadStats);
void dpcrtlmm_Dump(FILE* Target); /* Dumps a table of all active allocations with lots of detail */

/* Call this to get the library version info */
PS_HORATIO_VERSION dpcrtlmm_Ver(PS_HORATIO_VERSION PVerStruct);

/* I have my own MIN/MAXs here, use these only if you want to */
#define HORATIO_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define HORATIO_MAX(a,b) (((a) > (b)) ? (a) : (b))
/* Legacy MIN/MAX macros */
#define DPCRTLMM_MIN(a,b) HORATIO_MIN((a),(b))
#define DPCRTLMM_MAX(a,b) HORATIO_MAX((a),(b))

/* Trap numbers, these aren't used by the default handler but if the
user installs a user handler it could look at the number and decide
to ignore certain traps. */
#define HORATIO_TRAP_UNKNOWN              (0x0)  /* Unknown trap */
#define HORATIO_TRAP_INDEX_GEN_FAILED     (0x1)  /* Block not found, index not generated */
#define HORATIO_TRAP_BAD_HANDLER_REMOVAL  (0x2)  /* Can't remove the trap handler or trap hook by installing a NULL one */
#define HORATIO_TRAP_NULL_HANDLER         (0x3)  /* Attempt to install a NULL trap handler or trap hook */
#define HORATIO_TRAP_UNAUTH_REMOVE        (0x4)  /* Caller cannot remove the trap handler or trap hook because they do not know it's address (and so probally don't own it) */
#define HORATIO_TRAP_MUL_STARTUP          (0x5)  /* Multiple calls of Startup() (since library should be started before user handler or hook is installed user callbacks would never see this trap number) */
#define HORATIO_TRAP_MUL_SHUTDOWN         (0x6)  /* Multiple calls of Shutdown() */
#define HORATIO_TRAP_UNFREED_DATA         (0x7)  /* User failed to destroy everything before calling Shutdown() */
#define HORATIO_TRAP_BAD_BLOCK_ARRAY      (0x8)  /* The user supplied a pointer to a block array which is not known */
#define HORATIO_TRAP_BAD_BLOCK            (0x9)  /* Block not in array so it's details cannot be accessed */
#define HORATIO_TRAP_UNOWNED_FREE         (0xA)  /* Attempt made to free memory we don't own or memory we have already freed */
#define HORATIO_TRAP_BAD_RANGE_MOVEUP     (0xB)  /* This is an internal error only: On freeing a block from an array the array is shifted up with Moveup() perhaps Moveup was called incorrectly */
#define HORATIO_TRAP_SHRINKARR_WHILE_NOWT (0xC)  /* Internal error: Shrinking block array while block array has no size! */
#define HORATIO_TRAP_SHRINKARR_TOOMUCH    (0xD)  /* Internal error: Shrinking block array mote than current size! */
#define HORATIO_TRAP_UNFREED_BLOCKS       (0xE)  /* User failed to free blocks in the array which was just requested to be destroyed */
#define HORATIO_TRAP_BASENONZERO          (0xF)  /* A base pointer was expected to be zero (not valid) at this time.  (more likely an internal error or user tampering of the descriptor details) */
#define HORATIO_TRAP_LOCKINGVIOLATION     (0x10) /* This block is locked and so the specified operation is illegal */

/* Legacy trap macros */
#define DPCRTLMM_TRAP_UNKNOWN              (HORATIO_TRAP_UNKNOWN)
#define DPCRTLMM_TRAP_INDEX_GEN_FAILED     (HORATIO_TRAP_INDEX_GEN_FAILED)
#define DPCRTLMM_TRAP_BAD_HANDLER_REMOVAL  (HORATIO_TRAP_BAD_HANDLER_REMOVAL)
#define DPCRTLMM_TRAP_NULL_HANDLER         (HORATIO_TRAP_NULL_HANDLER)
#define DPCRTLMM_TRAP_UNAUTH_REMOVE        (HORATIO_TRAP_UNAUTH_REMOVE)
#define DPCRTLMM_TRAP_MUL_STARTUP          (HORATIO_TRAP_MUL_STARTUP)
#define DPCRTLMM_TRAP_MUL_SHUTDOWN         (HORATIO_TRAP_MUL_SHUTDOWN)
#define DPCRTLMM_TRAP_UNFREED_DATA         (HORATIO_TRAP_UNFREED_DATA)
#define DPCRTLMM_TRAP_BAD_BLOCK_ARRAY      (HORATIO_TRAP_BAD_BLOCK_ARRAY)
#define DPCRTLMM_TRAP_BAD_BLOCK            (HORATIO_TRAP_BAD_BLOCK)
#define DPCRTLMM_TRAP_UNOWNED_FREE         (HORATIO_TRAP_UNOWNED_FREE)
#define DPCRTLMM_TRAP_BAD_RANGE_MOVEUP     (HORATIO_TRAP_BAD_RANGE_MOVEUP)
#define DPCRTLMM_TRAP_SHRINKARR_WHILE_NOWT (HORATIO_TRAP_SHRINKARR_WHILE_NOWT)
#define DPCRTLMM_TRAP_SHRINKARR_TOOMUCH    (HORATIO_TRAP_SHRINKARR_TOOMUCH)
#define DPCRTLMM_TRAP_UNFREED_BLOCKS       (HORATIO_TRAP_UNFREED_BLOCKS)
#define DPCRTLMM_TRAP_BASENONZERO          (HORATIO_TRAP_BASENONZERO)
#define DPCRTLMM_TRAP_LOCKINGVIOLATION     (HORATIO_TRAP_LOCKINGVIOLATION)

/* New in 1.1.4, define USING_DPCRTLMM before including this header if
you wish to make normal C runtime using code switch to libhoratio code
without changing all the calls.  In some custom distributions this was
done with usedpcrtlmm.h or similar.  Nb. USING_DPCRTLMM is the legacy
name for USING_HORATIO */

#ifdef USING_DPCRTLMM
  /* Legacy support */
#  define USING_HORATIO
#endif /*USING_DPCRTLMM*/

#ifdef USING_HORATIO
#  ifdef HORATIO_NONULL_BLOCKDESCARRAY
#    error ("You must configure as --enable-null-array to use USING_HORATIO")
#  else
#    ifdef __cplusplus
#      define malloc(s)     MemManager.Alloc((s), __FILE__, __LINE__)
#      define free(p)       MemManager.Free(p)
#      define calloc(n, s)  MemManager.Calloc((n), (s), __FILE__, __LINE__)
#      define realloc(p, s) MemManager.Realloc((p), (s))
#    else
#      define malloc(s)     dpcrtlmm_Alloc(NULL, (s))
#      define free(p)       dpcrtlmm_Free(NULL, (p))
#      define calloc(n, s)  dpcrtlmm_Calloc(NULL, (n), (s))
#      define realloc(p, s) dpcrtlmm_Realloc(NULL, (p), (s))
#    endif /*__cplusplus*/
#  endif
#endif /*USING_HORATIO*/

/*
  These macros are available to C++ and C users, and were added in able
  to easily and correctly record the __FILE__/__LINE__ information
  associated with the allocation.
*/
#ifdef __cplusplus
#  define dpcrtlmm_block_Alloc(bd, s) (bd).Alloc(s, __FILE__, __LINE__)
#  define dpcrtlmm_block_Free(bd, p) (bd).Free(p)
#  define dpcrtlmm_block_Calloc(bd, n, s) (bd).Calloc((n), (s), __FILE__, __LINE__)
#  define dpcrtlmm_block_Realloc(bd, p, s) (bd).Realloc((p), (s))
#else
#  define dpcrtlmm_block_Alloc(bd, s) dpcrtlmm_Alloc((bd), (s))
#  define dpcrtlmm_block_Free(bd, p) dpcrtlmm_Free((bd), (p))
#  define dpcrtlmm_block_Calloc(bd, n, s) dpcrtlmm_Calloc((bd), (n), (s))
#  define dpcrtlmm_block_Realloc(bd, p, s) dpcrtlmm_Realloc((bd), (p), (s))
#endif /*__cplusplus*/

/* Hacks for laziness in typing, to use these rather than the long names define
HORATIO_LAZYHACK just before including dpcrtlmm.h in the user source, these
names are not used internally by the library and are intended solely for the
users.  I'm not saying these are always going to be here, if I ever remove
them it won't be hard to write your own hack table. */

#ifdef DPCRTLMM_LAZYHACK
  /* Legacy support */
#  define HORATIO_LAZYHACK
#endif /*DPCRTLMM_LAZYHACK*/

#ifdef HORATIO_LAZYHACK
  /* Short function names */
#  define dpcinstalldebughook dpcrtlmm_InstallDebugHook
#  define dpcgetdebughookchaincount dpcrtlmm_GetDebugHookChainCount
#  define dpcgetdebughookmatrixcount dpcrtlmm_GetDebugHookMatrixCount
#  define dpcuninstalldebughook dpcrtlmm_UninstallDebugHook
#  define dpcalloc dpcrtlmm_Alloc
#  define dpcfree dpcrtlmm_Free
#  define dpccreateblockarray dpcrtlmm_CreateBlockArray
#  define dpcdestroyblockarray dpcrtlmm_DestroyBlockArray
#  define dpcstartup dpcrtlmm_Startup
#  define dpcshutdown dpcrtlmm_Shutdown
#  define dpcgetblocksize dpcrtlmm_GetBlockSize
#  define dpcisbadblockptr dpcrtlmm_IsBadBlockPtr
#  define dpcisbadarrayptr dpcrtlmm_IsBadArrayPtr
#  define dpcrealloc dpcrtlmm_Realloc
#  define dpccalloc dpcrtlmm_Calloc
#  define dpcinstalltrapcallback dpcrtlmm_InstallTrapCallback
#  define dpcremovetrapcallback dpcrtlmm_RemoveTrapCallback
#  define dpcgettrapcallbackinfo dpcrtlmm_GetTrapCallbackInfo
#  define dpcmodifydescriptorflags dpcrtlmm_ModifyDescriptorFlags
#  define dpcsetblocklockingflag dpcrtlmm_SetBlockLockingFlag
#  define dpcisblocklocked dpcrtlmm_IsBlockLocked
#  define dpclockblock dpcrtlmm_LockBlock
#  define dpcunlockblock dpcrtlmm_UnlockBlock
#  define dpctoggleblocklockingstatus dpcrtlmm_ToggleBlockLockingStatus
#  define dpcenabletraps dpcrtlmm_EnableTraps
#  define dpcdisabletraps dpcrtlmm_DisableTraps
#  define dpcaretrapsenabled dpclrtmm_AreTrapsEnabled
#  define dpcgetblockcount dpcrtlmm_GetBlockCount
#  define dpcgetstats dpcrtlmm_GetStats
#  define dpcdump dpcrtlmm_Dump
#  define dpcver dpcrtlmm_Ver
#  define dpcisdefaultblockarray dpcrtlmm_IsDefaultBlockArray
#  define dpcblockalloc dpcrtlmm_block_Alloc
#  define dpcblockfree dpcrtlmm_block_Free
#  define dpcblockcalloc dpcrtlmm_block_Calloc
#  define dpcblockrealloc dpcrtlmm_block_Realloc

  /* Shorter structure names */
#  define S_DPC_BLOCKDESCRIPTOR S_HORATIO_BLOCKDESCRIPTOR
#  define PS_DPC_BLOCKDESCRIPTOR PS_HORATIO_BLOCKDESCRIPTOR

#  define S_DPC_BLOCKDESCARRAY S_HORATIO_BLOCKDESCARRAY
#  define PS_DPC_BLOCKDESCARRAY PS_HORATIO_BLOCKDESCARRAY

#  define S_DPC_DEBUGHOOKINFO S_HORATIO_DEBUGHOOKINFO
#  define PS_DPC_DEBUGHOOKINFO PS_HORATIO_DEBUGHOOKINFO

#  define S_DPC_STATS S_HORATIO_STATS
#  define PS_DPC_STATS PS_HORATIO_STATS

#  define S_DPC_VERSION S_HORATIO_VERSION
#  define PS_DPC_VERSION PS_HORATIO_VERSION
#endif /*HORATIO_LAZYHACK*/

#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/
/*-------------------------------------------------------------------------*/
#endif /*!INC_HORATIO_H*/
