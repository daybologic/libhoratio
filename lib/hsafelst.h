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

/*! \file hsafelst.h
 * \brief Functions for handling the safety list
 *
 * No functions here are for calling by the user, they are only for internal
 * library use.
 */

#ifndef INC_HORATIO_SAFETYLIST_H
#define INC_HORATIO_SAFETYLIST_H

#ifndef HORATIO_SOURCE /* Looks like a user inclusion! */
# error The safety list is for internal use by HORATIO only
#endif /*!HORATIO_SOURCE*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

unsigned int horatio_SafetyList_IsArrayPtrPresent(
	const PS_HORATIO_BLOCKDESCARRAY ArrayBase
);
void horatio_SafetyList_Init(void);
unsigned int horatio_SafetyList_CountUsed(void);
unsigned int horatio_SafetyList_AddBase(
	const PS_HORATIO_BLOCKDESCARRAY ArrayBase
);
unsigned int horatio_SafetyList_RemoveBase(
	const PS_HORATIO_BLOCKDESCARRAY ArrayBase
);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

/*
 * Note these names are slightly over qualified so that things are safe with
 * any given host program, slightly shorter names are available internally, and
 * these functions are ONLY called internally anyway, the over qualification was
 * to make the map file look cleaner so programmers working on something can
 * ignore this library.
 */
#define SafetyList_IsArrayPtrPresent    horatio_SafetyList_IsArrayPtrPresent
#define SafetyList_Init                 horatio_SafetyList_Init
#define SafetyList_CountUsed            horatio_SafetyList_CountUsed
#define SafetyList_AddBase              horatio_SafetyList_AddBase
#define SafetyList_RemoveBase           horatio_SafetyList_RemoveBase

#define SafetyList_CountFree(SafetyList) \
  ( (HORATIO_SAFETYLIST_MAXSIZE) - SafetyList_CountList(safetyList) )

#endif /*!INC_HORATIO_SAFETYLIST_H*/
