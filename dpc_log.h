/*
    DPCRTLMM Memory management internal log interface
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
#ifndef __INC_DPCRTLMM_LOG_H
#define __INC_DPCRTLMM_LOG_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#ifndef DPCRTLMM_SOURCE
#error ("log.h is for DPCRTLMM's internal use only")
#endif /*!DPCRTLMM_SOURCE*/

/* 1st Dec 2000: 1.2b: News: The logging's been reworked, we now support
messages, warnings and errors */

/* Types of logging messages */
#define DPCRTLMM_LOG_MESSAGE (0U) /* Only put in log */
#define DPCRTLMM_LOG_WARNING (1U) /* stderr and log */
#define DPCRTLMM_LOG_ERROR (2U) /* Same as error for now */
/* Even though error and warning do the same thing they display different messages in the log and on stderr */

void dpcrtlmm_int_Log(const char* File, const unsigned int Line, const unsigned short Severity, const char* Message); /* Write the message to the log (or do nothing if the log macro is undefined */

/* To make my life easier... but MESSAGE is only defined for logging builds */
#ifdef DPCRTLMM_LOG
#  define MESSAGE(sfn, sfl, msg) dpcrtlmm_int_Log((sfn), (sfl), (const unsigned short)DPCRTLMM_LOG_MESSAGE, (msg))
#else /* Non logging build */
#  define MESSAGE(sfn, sfl, msg) /* Do nothing with it */
#endif /*DPCRTLMM_LOG*/

#define WARNING(msg) dpcrtlmm_int_Log((__FILE__), (__LINE__), (const unsigned short)DPCRTLMM_LOG_WARNING, (msg))
#define ERROR(msg) dpcrtlmm_int_Log((__FILE__), (__LINE__), (const unsigned short)DPCRTLMM_LOG_ERROR, (msg))

#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_LOG_H*/
