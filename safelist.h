/*
    DPCRTLMM Memory management library
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
/*
#############################################################################
# Functions for handling the safety list                                    #
# DPCRTLMM--The DayboLogic C-Runtime Memory Manager                         #
# No functions here are for calling by the user, they are only for internal #
# library use.                                                              #
#############################################################################
*/

#ifndef __INC_DPCRTLMM_SAFETYLIST_H
#define __INC_DPCRTLMM_SAFETYLIST_H
/*-------------------------------------------------------------------------*/

#ifndef DPCRTLMM_SOURCE /* Looks like a user inclusion! */
#error The safety list is for internal use by DPCRTLMM only
#endif /*!DPCRTLMM_SOURCE*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

unsigned int dpcrtlmm_SafetyList_IsArrayPtrPresent(const PS_DPCRTLMM_BLOCKDESCARRAY ArrayBase);
void dpcrtlmm_SafetyList_Init(void);
unsigned int dpcrtlmm_SafetyList_CountUsed(void);
unsigned int dpcrtlmm_SafetyList_AddBase(const PS_DPCRTLMM_BLOCKDESCARRAY ArrayBase);
unsigned int dpcrtlmm_SafetyList_RemoveBase(const PS_DPCRTLMM_BLOCKDESCARRAY ArrayBase);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

/* Note these names are slightly over qualified so that things are safe with
any given host program, slightly shorter names are available internally, and
these functions are ONLY called internally anyway, the over qualification was
to make the map file look cleaner so programmers working on something can
ignore this library. */
#define SafetyList_IsArrayPtrPresent    dpcrtlmm_SafetyList_IsArrayPtrPresent
#define SafetyList_Init                 dpcrtlmm_SafetyList_Init
#define SafetyList_CountUsed            dpcrtlmm_SafetyList_CountUsed
#define SafetyList_AddBase              dpcrtlmm_SafetyList_AddBase
#define SafetyList_RemoveBase           dpcrtlmm_SafetyList_RemoveBase

#define SafetyList_CountFree(SafetyList) ( (DPCRTLMM_SAFETYLIST_MAXSIZE) - SafetyList_CountList(safetyList) )
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_SAFETYLIST_H*/
