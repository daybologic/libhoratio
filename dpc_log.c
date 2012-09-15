/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2012, David Duncan Ross Palmer, Daybo Logic
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

#define DPCRTLMM_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h> /* FILE */
#include <string.h> /* strcat() */
#include <mysql/mysql.h> /* For MySQL logging support */

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "dpc_intdata.h" /* Internal library header */
#include "dpc_trap.h" /* Trap() */
#include "dpc_log.h"
/*-------------------------------------------------------------------------*/
#define STRNCAT_FIXEDBUFF(buff, sourcestring) \
          strncat( \
            (buff), \
            (sourcestring), \
            (sizeof((buff))/sizeof((buff)[0])-1) \
            )
/*-------------------------------------------------------------------------*/
static MYSQL *dpcrtlmm_int_mysql_open(void);
static void dpcrtlmm_int_mysql_logmsg(
  const char *,
  const unsigned int,
  const unsigned short,
  const char *
);
/*-------------------------------------------------------------------------*/
static MYSQL DBHandle;
/*-------------------------------------------------------------------------*/
static MYSQL *dpcrtlmm_int_mysql_open()
{
  /* TODO:
  This scema must be created
  CREATE TABLE debug_log ( id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ts DATETIME NOT NULL, file CHAR(32) NOT NULL, line INTEGER NOT NULL, severity INTEGER NOT NULL default 0, msg VARCHAR(255) NOT NULL);
  */

  char *errMsgPtr = NULL;
  if ( mysql_real_connect(&DBHandle, "hurricane", "dpcrtlmmuser", "hefuZ6po", "dpcrtlmm", 0, NULL, 0) == NULL) { // Fail?
    errMsgPtr = "FIXME";
    Trap(0, errMsgPtr);
    return 0;
  }

  return &DBHandle;
}
/*-------------------------------------------------------------------------*/
static void dpcrtlmm_int_mysql_logmsg(
  const char *File,
  const unsigned int Line,
  const unsigned short Severity,
  const char *Msg
) {
  int rc;
  sqlite3_stmt *stmt;
  const char *q = "INSERT INTO debug_log (ts, file, line, severity, msg) \n"
    "VALUES(\n"
    "  DATETIME('NOW', 'localtime'), ?, ?, ?, ?\n"
    ")";

  if ( !DBHandle ) return;

  fprintf(stderr, "Got database message %s\n", Msg);
  fprintf(stderr, "Executing query: %s\n", q);
  rc = sqlite3_prepare_v2(DBHandle, q, strlen(q), &stmt, NULL);
  if ( rc != SQLITE_OK ) {
    fprintf(stderr, "Error %u from sqlite3_prepare_v2\n", rc);
    return;
  }
  if (mysql_query(&dbh, querystring) != 0) {
    mysql_close(&con);
    return 0;
  }

  rc = sqlite3_bind_text(stmt, 1, File, -1, SQLITE_STATIC);
  rc = sqlite3_bind_int(stmt, 2, Line);
  rc = sqlite3_bind_int(stmt, 3, Severity);
  rc = sqlite3_bind_text(stmt, 4, Msg, -1, SQLITE_STATIC);
  if ( rc != SQLITE_OK )
    fprintf(stderr, "Error %u from sqlite3_bind_text\n", rc);
  rc = sqlite3_step(stmt);
  if ( rc != SQLITE_DONE )
    fprintf(stderr, "Error %u from sqlite3_step\n", rc);
  rc = sqlite3_finalize(stmt); // Destroy the handle (FIXME, you should re-use it).
  if ( rc != SQLITE_OK )
    fprintf(stderr, "Error %u from sqlite3_finalize\n", rc);
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_Log(
  const char *File,
  const unsigned int Line,
  const unsigned short Severity,
  const char *Message
)
{
  /*
    String + safety for addons,
    note that mallocations should not be made here
  */
  char formatMsg[MAX_TRAP_STRING_LENGTH + 1024 + 512];
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

      if ( !DBHandle ) DBHandle = dpcrtlmm_int_mysql_open();
      dpcrtlmm_int_mysql_logmsg(File, Line, Severity, formatMsg);
    }
  }
  return;
}
/*-------------------------------------------------------------------------*/
