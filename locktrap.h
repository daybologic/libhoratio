/*
    DPCRTLMM Memory management, internal header
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
/* Internal header for a trap I prepared earlier for when a block locking
violation ocours.
Date of creation: 23rd Feb 2000 - Overlord David Duncan Ross Palmer
                                  (Overlord@DayboLogic.co.uk)
Last modified: 4th May 2000 by OverlordDDRP: Stripped BOOL
*/

#ifndef __INC_DPCRTLMM_LOCKTRAP_H
#define __INC_DPCRTLMM_LOCKTRAP_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
unsigned int dpcrtlmm_int_LockTrap(const char* FuncName, const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr);
#ifdef __cplusplus
}
#endif

#ifdef __INC_DPCRTLMM_INTDATA_H
#  define _LockTrap dpcrtlmm_int_LockTrap
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_LOCKTRAP_H*/
