/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer, Daybo Logic
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
  The debug hook executive and supporting code
  written (and perfomed by David Duncan Ross Palmer.

  File DBGHOOKS.C
  Library: DPCRTLMM Memory Manager
  Created 24th February 2000
*/

#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_log.h" /* LOG macro */
#include "dpc_biglock.h" /* Mutual exclusion */
#include "dpc_dbghooks.h"
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS

static unsigned int BadHookType(
  const unsigned int HookType
);

static unsigned int dpcrtlmm_int_InstallDebugHook(
  const unsigned short HookType,
  unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO)
);

static unsigned int dpcrtlmm_int_GetDebugHookChainCount(
  const unsigned int HookType
);

static unsigned int dpcrtlmm_int_GetDebugHookMatrixCount(
  void
);

static unsigned int dpcrtlmm_int_UninstallDebugHook(
  const unsigned short HookType,
  unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO)
);

#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
unsigned int dpcrtlmm_InstallDebugHook(
  const unsigned short HookType,
  unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO)
) {
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_InstallDebugHook(HookType, NewHookProc);
  UNLOCK

  return ret;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
unsigned int dpcrtlmm_GetDebugHookChainCount(
  const unsigned int HookType
) {
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_GetDebugHookChainCount(HookType);
  UNLOCK

  return ret;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
unsigned int dpcrtlmm_GetDebugHookMatrixCount() {
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_GetDebugHookMatrixCount();
  UNLOCK

  return ret;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
unsigned int dpcrtlmm_UninstallDebugHook(
  const unsigned short HookType,
  unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO)
) {
  unsigned int ret;

  LOCK
  ret = dpcrtlmm_int_UninstallDebugHook(HookType, HookProc2Remove);
  UNLOCK

  return ret;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
void dpcrtlmm_int_InitDebugHookMatrix() {
  /* Initialize or clear the debug hook matrix */
  unsigned int chainI; /* Used during initialization of chains loop */

  for ( chainI = 0U; chainI < DPCRTLMM_HOOKCHAIN_SIZE; chainI++ ) {
    unsigned int hookTypeI; /* Nested loop to process chains for other hook types */

    for ( hookTypeI = 0U; hookTypeI < DPCRTLMM_DEBUGHOOK_LASTHOOK+1; hookTypeI++ ) {
      unsigned int (*NULLHookPtr)(PS_DPCRTLMM_DEBUGHOOKINFO) = NULL; /* Make NULL pointer */

      dpcrtlmm_int__debugHookMatrix[(size_t)chainI][(size_t)hookTypeI] = NULLHookPtr; /* Init element */
    }
  }
  return;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
void dpcrtlmm_int_CallDebugHook(
  const unsigned short HookType,
  const PS_DPCRTLMM_DEBUGHOOKINFO PDebugHookInfo
) {
  /* locals */
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo; /* Local copy of caller's stuff */
  unsigned int allHooksLoop; /* Used to control processing of all the hooks loop */

  /* We must copy the debug hook information from the caller so that hook
  functions may change information before it is passed on to the next hook
  function, if they so desire, without damanging the person who called the
  trap executive (incase they use the original pointers somehow), though I
  can't think how they can offhand, still... */

  if (BadHookType(HookType)) { /* Bad hook type (out of range in matrix) */
    ERROR("CallDebugHook: Internal library error, HookType out of range!");
    return;
  }

  debugHookInfo = *PDebugHookInfo; /* Copy details by auto C deref */

  for ( allHooksLoop = 0U; allHooksLoop < DPCRTLMM_HOOKCHAIN_SIZE; allHooksLoop++ ) { /* All viable hook locations in the chain */
    unsigned int (*HookProc)(PS_DPCRTLMM_DEBUGHOOKINFO) = _debugHookMatrix[allHooksLoop][HookType]; /* Get hook function pointer from chain */

    if ( !HookProc ) /* No hook info */
      continue; /* Move onto next hook pointer */

    if ( !HookProc(&debugHookInfo) ) /* Call hook procedure */
      break; /* Hook requested not to pass information onto following hooks */
  }
  return; /* All hook calls are done */
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
static unsigned int dpcrtlmm_int_InstallDebugHook(
  const unsigned short HookType,
  unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO)
) {
  /* This function has added support for DPCRTLMM_HOOK_ALL */
  unsigned int i; /* looping */
  unsigned int set = 0U; /* set = FALSE */

  if (HookType != DPCRTLMM_HOOK_ALL) { /* Specific hook, not general hook */
    if (BadHookType(HookType)) return 0U; /* Ensure hook type is valid */
    /* Find the first free entry in the chain */
    for ( i = 0U; i < DPCRTLMM_HOOKCHAIN_SIZE; i++ ) {
      if ( !_debugHookMatrix[i][HookType] ) { /* Found free entry? */
        _debugHookMatrix[i][HookType] = NewHookProc; /* Install hook proc */
        set = 1U; /* Remember at least one hook was installed: set 'set' TRUE */
        break; /* Don't keep looping */
      }
    }
  } else { /* General hook that wants everything! */
    unsigned short nextHook;

    for ( nextHook = (unsigned short)0x0000U; nextHook < DPCRTLMM_DEBUGHOOK_LASTHOOK; nextHook++ ) { /* Go through all valid hook types */
      if ( !dpcrtlmm_InstallDebugHook(nextHook, NewHookProc) ) { /* Call ourselves back to sort it out */
        /* Failed to install a hook? */
        dpcrtlmm_UninstallDebugHook(DPCRTLMM_HOOK_ALL, NewHookProc); /* Remove all of the hooks which are for this address */
        return 0U; /* Report failure for the whole lot, FALSE return */
      }
      set = 1U; /* Report success, in a while */
    }
  }

  return set; /* No space for handler */
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
static unsigned int dpcrtlmm_int_GetDebugHookChainCount(
  const unsigned int HookType
) {
  unsigned int i;
  unsigned total = 0U;

  if (!BadHookType(HookType)) {
    for ( i = 0U; i < DPCRTLMM_HOOKCHAIN_SIZE; i++ ) { /* All hook positions */
      if ( _debugHookMatrix[i][HookType] ) /* Hook installed at this point in the chain? */
        total++; /* Increment count */
    }
  }
  return total;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
static unsigned int dpcrtlmm_int_GetDebugHookMatrixCount(void) {
  unsigned int i;
  unsigned total = 0U;

  for ( i = 0U; i <= DPCRTLMM_DEBUGHOOK_LASTHOOK; i++ ) /* All types of hooks */
    total += dpcrtlmm_int_GetDebugHookChainCount(i); /* Add chain contents to total for all chains */

  return total; /* Give total to caller */
}
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
static unsigned int dpcrtlmm_int_UninstallDebugHook(
  const unsigned short HookType,
  unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO)
) {
  /* This function has added support for DPCRTLMM_HOOK_ALL */

  unsigned int i;
  unsigned int retStatus = 0U; /* Return status FALSE by default */

  if (HookType != DPCRTLMM_HOOK_ALL) { /* Specific hook type request */
    if (BadHookType(HookType)) return 0U;

    for ( i = 0U; i < DPCRTLMM_DEBUGHOOK_LASTHOOK; i++ ) { /* Process all entries in the chain */
      if ( _debugHookMatrix[i][HookType] == HookProc2Remove ) { /* Found entry */
        retStatus = 1U; /* We found it! Return TRUE */
        _debugHookMatrix[i][HookType] = NULL; /* Delete address of hook proc */
        /* Not breaking the loop so we can remove duplicates too, say for
        example the user installed the same hook proc twice for the same type */
      }
    }
  } else { /* HookType is general */
    unsigned short si; /* Used for loop */
    retStatus = 1U; /* We always say success */

    for (si = (unsigned short)(0x0000U); si <= DPCRTLMM_DEBUGHOOK_LASTHOOK; si++) /* All possible debug hook types */
      dpcrtlmm_UninstallDebugHook(si, HookProc2Remove); /* Uninstall this hook from this type */
  }

  return retStatus; /* Give status to caller */
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
static unsigned int BadHookType(const unsigned int HookType) {
  unsigned int bad = 0U; /* Not a bad hook type yet... */

#ifndef NDEBUG_ /* Debugging lib */
    assert( HookType <= DPCRTLMM_DEBUGHOOK_LASTHOOK ); /* Check hook type is valid */
  #else /* Not in debug mode, must handle this same trap differenty */
    if ( !(HookType <= DPCRTLMM_DEBUGHOOK_LASTHOOK) ) /* Check hook type is valid */
      bad = 1U; /* bad = TRUE */
  #endif /*!NDEBUG*/

  return bad;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
