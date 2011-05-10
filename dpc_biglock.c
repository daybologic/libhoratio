/*
    DPCRTLMM Memory Manager Library : Big library thread safe lock.
    Copyright (C) 2000 Overlord David Duncan Ross Palmer, Daybo Logic.
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
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/

#define DPCRTLMM_SOURCE
#include <stdio.h>
#include "dpcrtlmm.h"
#include "dpc_build.h"
#ifdef DPCRTLMM_THREADS

#if ( !defined(__UNIX__) && !defined(__WIN32__) )
#  error ("Not currently a supported platform, build as config --disable-threads")
#endif

#if ( defined(__UNIX__) && defined(DPCRTLMM_MAXPORT) )
#  pragma message "Threads on POSIX configured as max portabillty doesn't support recursion, required for DPCRTLMM access from user callbacks.  Callbacks have the power to deadlock a thread"
#endif

#ifdef __WIN32__
#  include <windows.h>
#else
#  include <pthread.h>
#endif /*__WIN32__*/
/*--------------------------------------------------------------------------*/
#ifdef __WIN32__
#  define Mutant CRITICAL_SECTION
#  define InitialiseMutant(x) InitializeCriticalSection((x))
#  define LockMutant(x) EnterCriticalSection((x))
#  define UnlockMutant(x) LeaveCriticalSection((x))
#  define DestroyMutant(x) DeleteCriticalSection((x))
#else
#  define Mutant pthread_mutex_t
#  ifdef DPCRTLMM_MAXPORT /* Maxport, use POSIX fast mutex */
#    define InitialiseMutant(x) pthread_mutex_init((x), NULL)
#  else /* Use nonportable extension */
#    define InitialiseMutant(x) InitNPMutant((x))
#  endif /*DPCRTLMM_MAXPORT*/
#  define LockMutant(x) pthread_mutex_lock((x))
#  define UnlockMutant(x) pthread_mutex_unlock((x))
#  define DestroyMutant(x) pthread_mutex_destroy((x))
#endif /*__WIN32__*/
/*--------------------------------------------------------------------------*/
static Mutant _bigLock;

#if ( defined(__UNIX__) && !defined(DPCRTLMM_MAXPORT) )
void InitNPMutant(pthread_mutex_t* PMutant);
#endif /*__UNIX__ && !DPCRTLMM_MAXPORT*/
/*--------------------------------------------------------------------------*/
void dpcrtlmm_int_BigLockInit()
{
  InitialiseMutant(&_bigLock);
}
/*--------------------------------------------------------------------------*/
void dpcrtlmm_int_BigLockUninit()
{
  DestroyMutant(&_bigLock);
}
/*--------------------------------------------------------------------------*/
void dpcrtlmm_int_BigLock(int LockState)
{
  if ( LockState )
    LockMutant(&_bigLock);
  else
    UnlockMutant(&_bigLock);
}
/*--------------------------------------------------------------------------*/
#if ( defined(__UNIX__) && !defined(DPCRTLMM_MAXPORT) )
void InitNPMutant(pthread_mutex_t* PMutant)
{
  pthread_mutexattr_t attributes;

  pthread_mutexattr_init(&attributes);
  pthread_mutexattr_setkind_np(&attributes, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(PMutant, &attributes);
  pthread_mutexattr_destroy(&attributes);
}
#endif /*__UNIX__ && !DPCRTLMM_MAXPORT*/
/*--------------------------------------------------------------------------*/
#else /* Threads not required */
  char dpcrtlmm_int_BigLockDummyVar; /* Need at least one external to comply with ANSI */
#endif /*DPCRTLMM_THREADS*/
