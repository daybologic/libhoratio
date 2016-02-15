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

/*! \file hbiglock.c
 * \brief Internal library structure locking semantics for threaded builds.
 *
 * None of the functionality present in this file is directly used by callers
 * to the library.  Always check there is no macro within hbiglock.h before
 * calling any function directly.
 *
 * The big lock can cause contention, any attempt to lock it should attempt
 * to place the majority of time-sensitive code outside of the period the
 * lock is held by the thread.  This is known as the "critical section".
 */

#define HORATIO_SOURCE
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#ifndef _RECURSIVE
# define _RECURSIVE
#endif /*!_RECURSIVE*/

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif /*!_GNU_SOURCE*/

#include <stdio.h>
#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h"
#include "horatio.h"
#include "hbiglock.h"

#ifdef HORATIO_THREADS

#ifdef HORATIO_THREADS_PTHREAD
# ifndef HORATIO_THREADS_PTHREAD_NP
#  pragma message "Threads on POSIX configured as max portabillty doesn't support recursion, required for HORATIO access from user callbacks.  Callbacks have the power to deadlock a thread"
# endif /*!HORATIO_THREADS_PTHREAD_NP*/
#endif /*HORATIO_THREADS_PTHREAD*/

#if defined(HORATIO_THREADS_NT)
# include <windows.h>
#elif defined(HORATIO_THREADS_PTH)
# include <pth.h>
#elif defined(HORATIO_THREADS_PTHREAD)
# include <errno.h>
# include <pthread.h>
#endif

/*
 * Sometimes configure tells us we have non-portable functions available
 * but the macro PTHREAD_MUTEX_RECURSIVE_NP is not really available at
 * all.  If that is the case, revoke what configure set up for us
 */
#ifdef HORATIO_THREADS_PTHREAD
# ifdef HORATIO_THREADS_PTHREAD_NP
#  ifndef PTHREAD_MUTEX_RECURSIVE_NP
#   undef HORATIO_THREADS_PTHREAD_NP
#  endif /*!PTHREAD_MUTEX_RECURSIVE_NP*/
# endif /*HORATIO_THREADS_PTHREAD_NP*/
#endif /*HORATIO_THREADS_PTHREAD*/

#ifdef HORATIO_THREADS_NT

# define Mutant CRITICAL_SECTION
# define InitialiseMutant(x) InitializeCriticalSection((x))
# define LockMutant(x) EnterCriticalSection((x))
# define UnlockMutant(x) LeaveCriticalSection((x))
# define DestroyMutant(x) DeleteCriticalSection((x))

#elif defined(HORATIO_THREADS_PTH)

# define Mutant pth_mutex_t
# define InitialiseMutant(x) pth_mutex_init((x))
# define LockMutant(x) pth_mutex_acquire((x), (0), (NULL))
# define UnlockMutant(x) pth_mutex_release((x))
# define DestroyMutant(x) pth_mutex_init((x))

#elif defined(HORATIO_THREADS_PTHREAD)

# define Mutant pthread_mutex_t
# ifdef HORATIO_THREADS_PTHREAD_NP /* Supporting non-portable extension? */
#  define InitialiseMutant(x) InitNPMutant((x))
# else
#  define InitialiseMutant(x) pthread_mutex_init((x), NULL)
# endif /*HORATIO_MAXPORT*/
# define LockMutant(x) pthread_mutex_lock((x))
# define UnlockMutant(x) pthread_mutex_unlock((x))
# define DestroyMutant(x) pthread_mutex_destroy((x))

#endif /*HORATIO_THREADS_NT*/

static Mutant bigLock;

#ifdef HORATIO_THREADS_PTHREAD
# ifdef HORATIO_THREADS_PTHREAD_NP
void InitNPMutant(pthread_mutex_t* PMutant);
# endif /*HORATIO_THREADS_PTHREAD_NP*/
#endif /*HORATIO_THREADS_PTHREAD*/

void horatio_int_BigLockInit() {
	InitialiseMutant(&bigLock);
}

void horatio_int_BigLockUninit() {
	DestroyMutant(&bigLock);
}

void horatio_int_BigLock(int LockState) {
	if ( LockState )
		LockMutant(&bigLock);
	else
		UnlockMutant(&bigLock);
}

#ifdef HORATIO_THREADS_PTHREAD
#ifdef HORATIO_THREADS_PTHREAD_NP
void InitNPMutant(pthread_mutex_t* PMutant) {
	pthread_mutexattr_t attributes;

	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_setkind_np(&attributes, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(PMutant, &attributes);
	pthread_mutexattr_destroy(&attributes);
}
#endif /*HORATIO_THREADS_PTHREAD_NP*/
#endif /*HORATIO_THREADS_PTHREAD*/

#else /* !HORATIO_THREADS -- Threads not required */
/* Need at least one external to comply with ANSI */
char horatio_int_BigLockDummyVar;
#endif /*HORATIO_THREADS*/
