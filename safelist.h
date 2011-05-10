/**********************************************************************
 *                                                                    *
 * "DPCRTLMM" David Palmer's C-RTL Memory Manager Copyright (c) 2000  *
 * David Duncan Ross Palmer, Daybo Logic all rights reserved.         *
 * http://daybologic.com/Dev/dpcrtlmm                                 *
 *                                                                    *
 * D.D.R. Palmer's official homepage: http://daybologic.com/overlord  *
 * See the included license file for more information.                *
 *                                                                    *
 **********************************************************************
*/
/*
#############################################################################
# Functions for handling the safety list                                    #
# DPCRTLMM--The DayboLogic C-Runtime Memory Manager                         #
# No functions here are for calling by the user, they are only for internal #
# library use.                                                              #
#                                                                           #
#     - by Overlord David Duncan Ross Palmer (Overlord@DayboLogic.co.uk)    #
#                                                                           #
# (C)Copyright 2000 Daybo Logic, all rights reserved.                       #
#############################################################################
*/

#ifndef __INC_DPCRTLMM_SAFETYLIST_H
#define __INC_DPCRTLMM_SAFETYLIST_H
/*-------------------------------------------------------------------------*/
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

#define SafetyList_CountFree(SafetyList) ( (DPCRTLMM_SAFETYLIST_MAXCOUNT) - SafetyList_CountList(safetyList) )
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_SAFETYLIST_H*/
