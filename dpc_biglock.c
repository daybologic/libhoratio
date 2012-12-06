/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2012, David Duncan Ross Palmer, Daybo Logic
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

#define DPCRTLMM_SOURCE
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#ifndef _RECURSIVE
# define _RECURSIVE
#endif /*!_RECURSIVE*/

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif /*!_GNU_SOURCE*/

#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h"
#include "dpcrtlmm.h"
#include "dpc_biglock.h"

#ifdef DPCRTLMM_THREADS

#ifdef DPCRTLMM_THREADS_PTHREAD
#  ifndef DPCRTLMM_THREADS_PTHREAD_NP
#    pragma message "Threads on POSIX configured as max portabillty doesn't support recursion, required for DPCRTLMM access from user callbacks.  Callbacks have the power to deadlock a thread"
#  endif /*!DPCRTLMM_THREADS_PTHREAD_NP*/
#endif /*DPCRTLMM_THREADS_PTHREAD*/

#if defined(DPCRTLMM_THREADS_NT)
#  include <windows.h>
#elif defined(DPCRTLMM_THREADS_PTH)
#  include <pth.h>
#elif defined(DPCRTLMM_THREADS_PTHREAD)
#  include <errno.h>
#  include <pthread.h>
#endif

/*
  Sometimes configure tells us we have non-portable functions available
  but the macro PTHREAD_MUTEX_RECURSIVE_NP is not really available at
  all.  If that is the case, revoke what configure set up for us
*/
#ifdef DPCRTLMM_THREADS_PTHREAD
#  ifdef DPCRTLMM_THREADS_PTHREAD_NP
#    ifndef PTHREAD_MUTEX_RECURSIVE_NP
#      undef DPCRTLMM_THREADS_PTHREAD_NP
#    endif /*!PTHREAD_MUTEX_RECURSIVE_NP*/
#  endif /*DPCRTLMM_THREADS_PTHREAD_NP*/
#endif /*DPCRTLMM_THREADS_PTHREAD*/
/*--------------------------------------------------------------------------*/
#ifdef DPCRTLMM_THREADS_NT

#  define Mutant CRITICAL_SECTION
#  define InitialiseMutant(x) InitializeCriticalSection((x))
#  define LockMutant(x) EnterCriticalSection((x))
#  define UnlockMutant(x) LeaveCriticalSection((x))
#  define DestroyMutant(x) DeleteCriticalSection((x))

#elif defined(DPCRTLMM_THREADS_PTH)

#  define Mutant pth_mutex_t
#  define InitialiseMutant(x) pth_mutex_init((x))
#  define LockMutant(x) pth_mutex_acquire((x), (0), (NULL))
#  define UnlockMutant(x) pth_mutex_release((x))
#  define DestroyMutant(x) pth_mutex_init((x))

#elif defined(DPCRTLMM_THREADS_PTHREAD)

#  define Mutant pthread_mutex_t
#  ifdef DPCRTLMM_THREADS_PTHREAD_NP /* Supporting non-portable extension? */
#    define InitialiseMutant(x) InitNPMutant((x))
#  else
#    define InitialiseMutant(x) pthread_mutex_init((x), NULL)
#  endif /*DPCRTLMM_MAXPORT*/
#  define LockMutant(x) pthread_mutex_lock((x))
#  define UnlockMutant(x) pthread_mutex_unlock((x))
#  define DestroyMutant(x) pthread_mutex_destroy((x))

#endif /*DPCRTLMM_THREADS_NT*/
/*--------------------------------------------------------------------------*/
static Mutant bigLock;

#ifdef DPCRTLMM_THREADS_PTHREAD
#  ifdef DPCRTLMM_THREADS_PTHREAD_NP
    void InitNPMutant(pthread_mutex_t* PMutant);
#  endif /*DPCRTLMM_THREADS_PTHREAD_NP*/
#endif /*DPCRTLMM_THREADS_PTHREAD*/
/*--------------------------------------------------------------------------*/
void dpcrtlmm_int_BigLockInit()
{
  InitialiseMutant(&bigLock);
}
/*--------------------------------------------------------------------------*/
void dpcrtlmm_int_BigLockUninit()
{
  DestroyMutant(&bigLock);
}
/*--------------------------------------------------------------------------*/
void dpcrtlmm_int_BigLock(int LockState)
{
  if ( LockState )
    LockMutant(&bigLock);
  else
    UnlockMutant(&bigLock);
}
/*--------------------------------------------------------------------------*/
#ifdef DPCRTLMM_THREADS_PTHREAD
#ifdef DPCRTLMM_THREADS_PTHREAD_NP
void InitNPMutant(pthread_mutex_t* PMutant)
{
  pthread_mutexattr_t attributes;

  pthread_mutexattr_init(&attributes);
  pthread_mutexattr_setkind_np(&attributes, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(PMutant, &attributes);
  pthread_mutexattr_destroy(&attributes);
}
#endif /*DPCRTLMM_THREADS_PTHREAD_NP*/
#endif /*DPCRTLMM_THREADS_PTHREAD*/
/*--------------------------------------------------------------------------*/
#else /* !DPCRTLMM_THREADS -- Threads not required */
  char dpcrtlmm_int_BigLockDummyVar; /* Need at least one external to comply with ANSI */
#endif /*DPCRTLMM_THREADS*/
