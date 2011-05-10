/*
    DPCRTLMM memory management library source
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
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/
#ifndef __INC_DPCRTLMM_IBLKPTR
#define __INC_DPCRTLMM_IBLKPTR
/*-------------------------------------------------------------------------*/
#ifndef DPCRTLMM_SOURCE
#  error ("iblkptr.h is for internal DPCRTLMM usage only")
#endif /*DPCRTLMM_SOURCE*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
unsigned int dpcrtlmm_int_IndexFromBlockPtr(const PS_DPCRTLMM_BLOCKDESCARRAY PBlockArray, const void DPCRTLMM_FARDATA* BlockPtr);
#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#ifdef __INC_DPCRTLMM_INTDATA_H /* Build from inside? */
#  define _IndexFromBlockPtr dpcrtlmm_int_IndexFromBlockPtr /* Lazy shortcut */
#else /* Build from outside? */
#  error ("Clear off! This header is for internal library use")
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*__INC_DPCRTLMM_IBLKPTR*/
