/*
    DPCRTLMM Memory Management logger
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
#define DPCRTLMM_SOURCE
/*
#############################################################################
# Memory usage logging support for DPCRTLMM                                 #
# Only included if DPCRTLMM_LOG is defined in build.h                       #
# (Superseeded by --enable-log configuration option)                        #
#############################################################################

You might be wondering why I did not just get rid of the whole function if
DPCRTLMM_LOG was not defined.  Trouble is, even though it's never called if
the macro is not defined, in some compilers there must be at least one
external definition so I chose to get rid of the contents and disable the
warning about the unused parameter, if getting rid of the warning actually
causes a warning on your compiler, I aplogise!

  - Does this apply any longer?  : DDRP
*/

#include <stdio.h> /* FILE */
#include <string.h> /* strcat() */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library header */
#include "log.h"
/*-------------------------------------------------------------------------*/
#define STRNCAT_FIXEDBUFF(buff, sourcestring) \
          strncat((buff), (sourcestring), (sizeof((buff))/sizeof((buff)[0])-1))
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_Log(const char* File, const unsigned int Line, const unsigned short Severity, const char* Message)
{
  char formatMsg[MAX_TRAP_STRING_LENGTH + 1024 + 512]; /* String + safety for addons, note that mallocations should not be made here */
  char number[64]; /* Paranoia length number to string conversion */

  #ifdef DPCRTLMM_LOG
  FILE* HLogFile; /* Handle for log file */
  #endif /*DPCRTLMM_LOG*/

  if (Message)
  {
    if (Message[0])
    {
      formatMsg[0] = '\0'; /* so strncat() knows where to begin */
      STRNCAT_FIXEDBUFF(formatMsg, "DPCRTLMM: \"");
      if ( File ) sprintf(number, "%u", Line); /* Convert line number to string */
      switch ( Severity )
      {
        case DPCRTLMM_LOG_WARNING :
	{
          STRNCAT_FIXEDBUFF(formatMsg, "Warning! ");
          break;
        }
        case DPCRTLMM_LOG_ERROR :
	{
          STRNCAT_FIXEDBUFF(formatMsg, "FATAL ERROR! ");
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
      STRNCAT_FIXEDBUFF(formatMsg, "\"\n"); /* Close quotes and end line */

      /* Determine what do do with the message based on it's severity */
      /* Everything goes in the log... */
      #ifdef DPCRTLMM_LOG
      HLogFile = fopen("DPCRTLMM.LOG", "at"); /* Append/overwrite text file */
      if (HLogFile) /* Log opened? */
      {
        fputs(formatMsg, HLogFile); /* Output log msg to log file */
        fclose(HLogFile); /* Close the log file */
      }
      #endif /*DPCRTLMM_LOG*/

      if ( Severity > DPCRTLMM_LOG_MESSAGE ) /* Anything more severe than a warning */
        fprintf(DPCRTLMM_DEV_ERROR, formatMsg);
    }
  }
  return;
}
