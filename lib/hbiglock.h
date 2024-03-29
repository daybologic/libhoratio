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

/*! \file hbiglock.h
 * \brief Big locking semantics header (like Giant in FreeBSD)
 *
 */
#ifndef INC_HORATIO_BIGLOCK_H
#define INC_HORATIO_BIGLOCK_H

#ifndef HORATIO_SOURCE
# error ("biglock is for internal library use")
#endif

#ifdef HORATIO_THREADS

#ifdef __cplusplus
extern "C" {
#endif

void horatio_int_BigLockInit(void);
void horatio_int_BigLockUninit(void);
void horatio_int_BigLock(int LockState);

#ifdef __cplusplus
}
#endif

#endif /*HORATIO_THREADS*/

#ifdef HORATIO_THREADS
# define LOCK horatio_int_BigLock(1);
# define UNLOCK horatio_int_BigLock(0);
#else
# define LOCK
# define UNLOCK
#endif /*HORATIO_THREADS*/

#endif /*!INC_HORATIO_BIGLOCK_H*/
