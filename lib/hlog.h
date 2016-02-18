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

/*! \file hlog.h
 * \brief Internal logging functions
 *
 * 1st Dec 2000: 1.2b: News: The logging's been reworked, we now support
 * messages, warnings and errors
 *
 * hlog.h is for HORATIO's internal use only
 */
#ifndef INC_HORATIO_LOG_H
#define INC_HORATIO_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#ifndef HORATIO_SOURCE
# error ("hlog.h is for HORATIO's internal use only")
#endif /*!HORATIO_SOURCE*/

/* Types of logging messages */

/*! \def HORATIO_LOG_MESSAGE
 * \brief Only put in log
 */
#define HORATIO_LOG_MESSAGE (0U)

/*! \def HORATIO_LOG_WARNING
 * \brief stderr and log
 */
#define HORATIO_LOG_WARNING (1U)

/*! \def HORATIO_LOG_ERROR
 * \brief Same as error for now
 */
#define HORATIO_LOG_ERROR   (2U)

/*
 * Even though error and warning do the same thing, they display different
 * messages in the log and on stderr
 */

/*
 * TODO: Doxygen
 * Write the message to the log (or do nothing if the log macro is undefined
 */
void horatio_int_Log(
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Message
);

/*
 * To make our lives easier... but MESSAGE is only defined for logging builds
 */
#ifdef HORATIO_LOG
# define MESSAGE(sfn, sfl, msg)                                             \
    horatio_int_Log((sfn), (sfl),                                           \
    (const unsigned short)HORATIO_LOG_MESSAGE, (msg)                        \
)
#else /* Non logging build */
# define MESSAGE(sfn, sfl, msg) /* Do nothing with it */
#endif /*HORATIO_LOG*/

#define WARNING(msg)                                                        \
    horatio_int_Log(                                                        \
        (__FILE__), (__LINE__),                                             \
        (const unsigned short)HORATIO_LOG_WARNING, (msg)                    \
    )

#define ERROR(msg)                                                          \
    horatio_int_Log(                                                        \
    (__FILE__), (__LINE__),                                                 \
    (const unsigned short)HORATIO_LOG_ERROR, (msg)                          \
)

#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

#endif /*!INC_HORATIO_LOG_H*/
