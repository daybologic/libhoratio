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

/*! \file hiblkptr.h
 * \brief Internal block pointer information
 *
 * hiblkptr.h is for internal HORATIO usage only
 */
#ifndef INC_HORATIO_IBLKPTR
#define INC_HORATIO_IBLKPTR

#ifndef HORATIO_SOURCE
# error ("hiblkptr.h is for internal HORATIO usage only")
#endif /*HORATIO_SOURCE*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
unsigned int horatio_int_IndexFromBlockPtr(
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA* BlockPtr
);
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#ifdef INC_HORATIO_INTDATA_H /* Build from inside? */
# define _IndexFromBlockPtr horatio_int_IndexFromBlockPtr /* Lazy shortcut */
#else /* Build from outside? */
# error ("This header is for internal library use")
#endif /*INC_HORATIO_INTDATA_H*/

#endif /*INC_HORATIO_IBLKPTR*/
