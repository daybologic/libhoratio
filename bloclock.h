/*
    DPCRTLMM Memory Manager Library.
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

#ifndef __INC_DPCRTLMM_BLOCLOCK_H
#define __INC_DPCRTLMM_BLOCLOCK_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
  extern "C" {
#endif /*__cplusplus*/

#ifndef DPCRTLMM_SOURCE
#  error ("This header is not for you")
#endif /*DPCRTLMM_SOURCE*/

void dpcrtlmm_int_SetBlockLockingFlag(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr, const unsigned int NewStatus);
unsigned int dpcrtlmm_int_IsBlockLocked(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr);
void dpcrtlmm_int_ToggleBlockLockingStatus(PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* Ptr);

#ifdef __cplusplus
  }
#endif /*__cplusplus*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_BLOCLOCK_H*/
