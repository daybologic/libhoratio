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

/*! \file hlog.c
 * \brief hlog.c Memory usage logging support for Horatio
 *
 * Only included if HORATIO_LOG is defined in build.h
 * (Superseeded by --enable-log configuration option)
 *
 * You might be wondering why I did not just get rid of the whole function if
 * HORATIO_LOG was not defined.  Trouble is, even though it's never called if
 * the macro is not defined, in some compilers there must be at least one
 * external definition so I chose to get rid of the contents and disable the
 * warning about the unused parameter, if getting rid of the warning actually
 * causes a warning on your compiler, I aplogise!
 *
 * TODO: Does this apply any longer?  : DDRP
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h> /* FILE */
#include <string.h> /* strcat() */

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library header */
#include "hlog.h"

#define STRNCAT_FIXEDBUFF(buff, sourcestring) \
    strncat( \
        (buff), \
        (sourcestring), \
        (sizeof((buff))/sizeof((buff)[0])-1) \
    )

/*!
 * \brief Logging function, intended for use by Horatio's own code.
 *
 * \param File Source file name, internal to the library, typically __FILE__
 * \param Line Source file line, internal to the library, typically __LINE__
 * \param Severity The severity level of the logged message
 * \param Message The text of the message to log
 *
 * This function should be called indirectly, via the LOG macro,
 * and provides the logging facility to all internal code.
 * The function may silently fail under some conditions.
 */
void horatio_int_Log(
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Message
) {
	/*
	 * String + safety for addons,
	 * note that mallocations should not be made here
	 */
	char formatMsg[MAX_TRAP_STRING_LENGTH + 1024 + 512];
	char number[64]; /* Paranoia length number to string conversion */

#ifdef HORATIO_LOG
	FILE* HLogFile; /* Handle for log file */
#endif /*HORATIO_LOG*/

	if (Message) {
		if (Message[0]) {
			/* so strncat() knows where to begin */
			formatMsg[0] = '\0';
			STRNCAT_FIXEDBUFF(formatMsg, "HORATIO: \"");
			if ( File ) { /* Convert line number to string */
				sprintf(
					number,
#ifdef HAVE_SNPRINTF
					sizeof(number)-1,
#endif /*HAVE_SNPRINTF*/
					"%u",
					Line
				);
			}
			switch ( Severity ) {
				case HORATIO_LOG_WARNING : {
					STRNCAT_FIXEDBUFF(
						formatMsg, "Warning! "
					);
					break;
				}
				case HORATIO_LOG_ERROR : {
					STRNCAT_FIXEDBUFF(
						formatMsg, "FATAL ERROR! "
					);
					break;
				}
			}
			if ( File ) {
				STRNCAT_FIXEDBUFF(formatMsg, File);
				STRNCAT_FIXEDBUFF(formatMsg, ", L");
				STRNCAT_FIXEDBUFF(formatMsg, number);
				STRNCAT_FIXEDBUFF(formatMsg, ": ");
			}
			STRNCAT_FIXEDBUFF(formatMsg, Message);
			/* Close quotes and end line */
			STRNCAT_FIXEDBUFF(formatMsg, "\"\n");

			/*
			 * Determine what to do with the message
			 * based on it's severity.
			 */
#ifdef HORATIO_LOG
			/* Everything goes in the log... */
			/* Append/overwrite text file */
			HLogFile = fopen("HORATIO.LOG", "at");
			if (HLogFile) { /* Log opened? */
				/* Output log msg to log file */
				fputs(formatMsg, HLogFile);
				fclose(HLogFile); /* Close the log file */
			}
#endif /*HORATIO_LOG*/

			if ( Severity > HORATIO_LOG_MESSAGE ) {
				/* Anything more severe than a warning */
				fprintf(HORATIO_DEV_ERROR, "%s", formatMsg);
			}
		}
	}
	return;
}
