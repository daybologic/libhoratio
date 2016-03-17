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

/*! \file hvptrap.h
 * \brief Verify pointer trap header
 *
 * Routines for triggering a trap where a pointer block desciptor is invalid.
 * See horatio_int_VerifyPtrs
 */
#ifndef INC_HORATIO_VPTRAP_H
#define INC_HORATIO_VPTRAP_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
void horatio_int_VerifyPtrs(
	const char *FuncName,
	const PS_HORATIO_BLOCKDESCARRAY PBlockArray,
	const void HORATIO_FARDATA *BlockPtr
); /* BlockPtr can be NULL */
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#ifdef INC_HORATIO_INTDATA_H
/*! \def _VerifyPtrs
 * \brief Shortcut to call horatio_int_VerifyPtrs
 *
 * This macro is only availablr when INC_HORATIO_INTDATA_H is defined
 */
# define _VerifyPtrs horatio_int_VerifyPtrs
#endif /*INC_HORATIO_INTDATA_H*/

#endif /*!INC_HORATIO_VPTRAP_H*/
