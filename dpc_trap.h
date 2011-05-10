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
#ifndef __INC_DPCRTLMM_TRAP_H
#define __INC_DPCRTLMM_TRAP_H
/*-------------------------------------------------------------------------*/
#ifndef __INC_DPCRTLMM_INTDATA_H
#  error ("Internal library users only!")  // <-- See Overlord DDRP Overlord@DayboLogic.co.uk
#endif /*__INC_DPCRTLMM_INTDATA_H*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
void dpcrtlmm_int__Trap(const unsigned int Id, const char* Message); /* Fire the specified trap */
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

/* NOTE: Prototype for InstallTrapHandler() is in DPCRTLMM.H */

#ifdef __INC_DPCRTLMM_INTDATA_H
#  define _Trap dpcrtlmm_int__Trap
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_TRAP_H*/
