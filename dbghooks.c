/*
    DPCRTLMM Memory Management Library : Debug hooks
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
#define DPCRTLMM_SOURCE
/* The debug hook executive and supporting code
written (and perfomed by Overlord David
Duncan Ross Palmer.

File DBGHOOKS.C
Library: DPCRTLMM Memory Manager
Date of creation: 24th Feb 2000
Last modified by Overlord David Duncan Ross Palmer on 21st July 2000
*/

#include <assert.h>
#include <stddef.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library data */
#include "log.h" /* LOG macro */
#include "dbghooks.h"
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
static unsigned int BadHookType(const unsigned int HookType);
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
void dpcrtlmm_int_InitDebugHookMatrix()
{
  /* Initialize or clear the debug hook matrix */
  unsigned int chainI; /* Used during initialization of chains loop */

  for ( chainI = 0U; chainI < DPCRTLMM_HOOKCHAIN_SIZE; chainI++ )
  {
    unsigned int hookTypeI; /* Nested loop to process chains for other hook types */

    for ( hookTypeI = 0U; hookTypeI < DPCRTLMM_DEBUGHOOK_LASTHOOK+1; hookTypeI++ )
    {
      unsigned int (*NULLHookPtr)(PS_DPCRTLMM_DEBUGHOOKINFO) = NULL; /* Make NULL pointer */

      dpcrtlmm_int__debugHookMatrix[(size_t)chainI][(size_t)hookTypeI] = NULLHookPtr; /* Init element */
    }
  }
  return;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
void dpcrtlmm_int_CallDebugHook(const unsigned short HookType, const PS_DPCRTLMM_DEBUGHOOKINFO PDebugHookInfo)
{
  /* locals */
  S_DPCRTLMM_DEBUGHOOKINFO debugHookInfo; /* Local copy of caller's stuff */
  unsigned int allHooksLoop; /* Used to control processing of all the hooks loop */

  /* We must copy the debug hook information from the caller so that hook
  functions may change information before it is passed on to the next hook
  function, if they so desire, without damanging the person who called the
  trap executive (incase they use the original pointers somehow), though I
  can't think how they can offhand, still... */

  if (BadHookType(HookType)) /* Bad hook type (out of range in matrix) */
  {
    ERROR("CallDebugHook: Internal library error, HookType out of range!");
    return;
  }

  debugHookInfo = *PDebugHookInfo; /* Copy details by auto C deref */

  for ( allHooksLoop = 0U; allHooksLoop < DPCRTLMM_HOOKCHAIN_SIZE; allHooksLoop++ ) /* All viable hook locations in the chain */
  {
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
unsigned int dpcrtlmm_InstallDebugHook(const unsigned short HookType, unsigned int(*NewHookProc)(PS_DPCRTLMM_DEBUGHOOKINFO))
{
  /* This function has added support for DPCRTLMM_HOOK_ALL */
  unsigned int i; /* looping */
  unsigned int set = 0U; /* set = FALSE */

  if (HookType != DPCRTLMM_HOOK_ALL) /* Specific hook, not general hook */
  {
    if (BadHookType(HookType)) return 0U; /* Ensure hook type is valid */
    /* Find the first free entry in the chain */
    for ( i = 0U; i < DPCRTLMM_HOOKCHAIN_SIZE; i++ )
    {
      if ( !_debugHookMatrix[i][HookType] ) /* Found free entry? */
      {
        _debugHookMatrix[i][HookType] = NewHookProc; /* Install hook proc */
        set = 1U; /* Remember at least one hook was installed: set 'set' TRUE */
        break; /* Don't keep looping */
      }
    }
  }
  else /* General hook that wants everything! */
  {
    unsigned short nextHook;

    for ( nextHook = (unsigned short)0x0000U; nextHook < DPCRTLMM_DEBUGHOOK_LASTHOOK; nextHook++ ) /* Go through all valid hook types */
    {
      if ( !dpcrtlmm_InstallDebugHook(nextHook, NewHookProc) ) /* Call ourselves back to sort it out */
      {
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
unsigned dpcrtlmm_GetDebugHookChainCount(const unsigned int HookType)
{
  unsigned int i;
  unsigned total = 0U;

  if (!BadHookType(HookType))
  {
    for ( i = 0U; i < DPCRTLMM_HOOKCHAIN_SIZE; i++ ) /* All hook positions */
    {
      if ( _debugHookMatrix[i][HookType] ) /* Hook installed at this point in the chain? */
        total++; /* Increment count */
    }
  }
  return total;
}
#endif /*DPCRTLMM_DEBUGHOOKS*/
/*-------------------------------------------------------------------------*/
unsigned dpcrtlmm_GetDebugHookMatrixCount(void)
{
  unsigned int i;
  unsigned total = 0U;

  for ( i = 0U; i <= DPCRTLMM_DEBUGHOOK_LASTHOOK; i++ ) /* All types of hooks */
  {
    total += dpcrtlmm_GetDebugHookChainCount(i); /* Add chain contents to total for all chains */
  }
  return total; /* Give total to caller */
}
/*-------------------------------------------------------------------------*/
#ifdef DPCRTLMM_DEBUGHOOKS
unsigned int dpcrtlmm_UninstallDebugHook(const unsigned short HookType, unsigned int(*HookProc2Remove)(PS_DPCRTLMM_DEBUGHOOKINFO))
{
  /* This function has added support for DPCRTLMM_HOOK_ALL */

  unsigned int i;
  unsigned int retStatus = 0U; /* Return status FALSE by default */

  if (HookType != DPCRTLMM_HOOK_ALL) /* Specific hook type request */
  {
    if (BadHookType(HookType)) return 0U;

    for ( i = 0U; i < DPCRTLMM_DEBUGHOOK_LASTHOOK; i++ ) /* Process all entries in the chain */
    {
      if ( _debugHookMatrix[i][HookType] == HookProc2Remove ) /* Found entry */
      {
        retStatus = 1U; /* We found it! Return TRUE */
        _debugHookMatrix[i][HookType] = NULL; /* Delete address of hook proc */
        /* Not breaking the loop so we can remove duplicates too, say for
        example the user installed the same hook proc twice for the same type */
      }
    }
  }
  else /* HookType is general */
  {
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
static unsigned int BadHookType(const unsigned int HookType)
{
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
