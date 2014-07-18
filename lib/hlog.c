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

/*
 * Memory usage logging support for HORATIO
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
 * - Does this apply any longer?  : DDRP
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h> /* FILE */
#include <string.h> /* strcat() */

#ifdef HAVE_STDBOOL_H
 /* Only used in horatio_int_mongodb_logmsg() */
# include <stdbool.h>
#endif /*HAVE_STDBOOL_H*/

#ifdef SQLITE
# include <sqlite3.h> /* For SQLite logging support */
#endif /*SQLITE*/

#ifdef USE_MYSQL
# include <mysql.h> /* For MySQL logging support */
#endif /*USE_MYSQL*/

#ifdef MONGO
# include <mongo.h> /* For MongoDB logging support */
#endif /*MONGO*/

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Main library header */
#include "hintdata.h" /* Internal library header */
#include "hlog.h"

#include "htrap.h" /* Trap() */
#define STRNCAT_FIXEDBUFF(buff, sourcestring) \
    strncat( \
        (buff), \
        (sourcestring), \
        (sizeof((buff))/sizeof((buff)[0])-1) \
    )

#ifdef SQLITE
static sqlite3 *horatio_int_sqlite3_open(void);
#endif /*SQLITE*/

#ifdef USE_MYSQL
static MYSQL *horatio_int_mysql_open(void);
#endif /*USE_MYSQL*/

#ifdef MONGO
static mongo_sync_connection *horatio_int_mongodb_open(void);
#endif /*MONGO*/

#ifdef SQLITE
static void horatio_int_sqlite3_logmsg(
  const char *,
  const unsigned int,
  const unsigned short,
  const char *
);
#endif /*SQLITE*/

#ifdef USE_MYSQL
static void horatio_int_mysql_logmsg(
  const char *,
  const unsigned int,
  const unsigned short,
  const char *
);
#endif /*USE_MYSQL*/

#ifdef SQLITE
static sqlite3 *Handle_sqlite = NULL;
#endif /*SQLITE*/

#ifdef USE_MYSQL
static MYSQL *Handle_mysql;
#endif /*USE_MYSQL*/

#ifdef MONGO
static mongo_sync_connection *mongo_client;
//static mongoc_collection_t *collection;
#endif /*MONGO*/

#ifdef SQLITE
static sqlite3 *horatio_int_sqlite3_open() {
  /* TODO:
  This scema must be created
  CREATE TABLE debug_log ( id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, ts DATETIME NOT NULL, file CHAR(32) NOT NULL, line INTEGER NOT NULL, severity INTEGER NOT NULL default 0, msg VARCHAR(255) NOT NULL);
  */

	sqlite3 *dbh;
	const char *errMsgPtr = NULL;
	int rc = sqlite3_open("horatio.db", &dbh);
	if ( rc ) { // Fail?
		errMsgPtr = sqlite3_errmsg(dbh);
		Trap(0, errMsgPtr);
		sqlite3_close(dbh);
	}
	return dbh;
}

static void horatio_int_sqlite3_logmsg(
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

	if ( !Handle_sqlite ) return;

	fprintf(stderr, "Got database message %s\n", Msg);
	fprintf(stderr, "Executing query: %s\n", q);
	rc = sqlite3_prepare_v2(Handle_sqlite, q, strlen(q), &stmt, NULL);
	if ( rc != SQLITE_OK ) {
		fprintf(stderr, "Error %u from sqlite3_prepare_v2\n", rc);
		return;
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
#endif /*SQLITE*/

#ifdef MONGO
static void horatio_int_mongodb_logmsg(
    const char *,
    const unsigned int,
    const unsigned short,
    const char *
);

static mongo_sync_connection *horatio_int_mongodb_open() {
	//mongoc_uri_t *uri;
	//mongoc_insert_flags_t flags = MONGOC_INSERT_NONE;
	//mongoc_write_concern_t *write_concern;
	//bson_error_t error;
	//const char *const uriString = "mongodb://username:password@localhost:0/test";

	//uri = mongoc_uri_new(uriString);
	//client = mongoc_client_new_from_uri(uri);
	client = mongo_sync_connect("localhost", 0, false);

	//collection = mongoc_client_get_collection(client, "test", "m6kvm");

	return client;
}

static void horatio_int_mongodb_logmsg(
    const char *File,
    const unsigned int Line,
    const unsigned short Severity,
    const char *Msg
) {
	//bson_t document;
	bson *document;
	bool qsl;

	//if ( !client || !collection ) return;
	if ( !client ) return;
	fprintf(stderr, "File %s, line %u, severity %u\n", File, Line, Severity);

	fprintf(stderr, "Got database message %s\n", Msg);
	//bson_init(&document);
	document = bson_new();
	/*bson_append_document_begin(&document, "$orderby", -1, &child);
	bson_append_int32(&child, "timestamp", -1, 1);
	bson_append_document_end(&document, &child);
	bson_append_document_begin(&document, "$query", -1, &child);
	bson_append_document_end(&document, &child);*/
	//qsl = mongoc_collection_insert(collection, flags, &document, write_concern, &error);
	qsl = false;
	if ( !qsl ) {
		fprintf(stderr, "Error mongoc_collection_insert\n");
		return;
	}

	bson_free(document);
	return;
}
#endif /*MONGO*/

#ifdef USE_MYSQL
static MYSQL *horatio_int_mysql_open() {
  char *errMsgPtr = NULL;
  if ( mysql_real_connect(Handle_mysql, "hurricane", "dpcrtlmmuser", "hefuZ6po", "dpcrtlmm", 0, NULL, 0) == NULL ) { // Fail?
    errMsgPtr = "FIXME";
    Trap(0, errMsgPtr);
    return 0;
  }

  return Handle_mysql;
}
#endif /*USE_MYSQL*/

#if 0
// Nb. The code here, look like SQLite because we're trying to translate from SQLite to MySQL,
// not because there has been an editing mistake
static void horatio_int_mysql_logmsg(
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

	if ( !Handle_mysql ) return;

	fprintf(stderr, "Got database message %s\n", Msg);
	fprintf(stderr, "Executing query: %s\n", q);
	rc = sqlite3_prepare_v2(DBHandle, q, strlen(q), &stmt, NULL);
	if ( rc != SQLITE_OK ) {
		fprintf(stderr, "Error %u from sqlite3_prepare_v2\n", rc);
		return;
	}
	if ( mysql_query(&dbh, querystring) != 0 ) {
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
#endif /*0*/

void horatio_int_Log(
	const char *File,
	const unsigned int Line,
	const enum hLogSeverity Severity,
	const char *Message
) {
	/*
	 * String + safety for addons,
	 * note that mallocations should not be made here
	 */
	char formatMsg[MAX_TRAP_STRING_LENGTH + 1024 + 512];
	char number[64]; /* Paranoia length number to string conversion */

#ifdef HORATIO_LOG
	FILE *HLogFile; /* Handle for log file */
#endif /*HORATIO_LOG*/

	if ( Message && Message[0] ) {
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
		if ( HLogFile ) { /* Log opened? */
			/* Output log msg to log file */
			fputs(formatMsg, HLogFile);
			fclose(HLogFile); /* Close the log file */
		}
#endif /*HORATIO_LOG*/

		if ( Severity > HORATIO_LOG_MESSAGE ) {
			/* Anything more severe than a warning */
			fprintf(HORATIO_DEV_ERROR, "%s", formatMsg);
		}

#ifdef SQLITE
		if ( !Handle_sqlite ) Handle_sqlite = horatio_int_sqlite3_open();
		horatio_int_sqlite3_logmsg(File, Line, Severity, Message);
#endif /*USE_MYSQL*/

#ifdef USE_MYSQL
		if ( !Handle_mysql ) Handle_mysql = horatio_int_mysql_open();
		horatio_int_mysql_logmsg(File, Line, Severity, Message);
#endif /*MONGO*/

#ifdef MONGO
		if ( !mongo_client ) mongo_client = horatio_int_mongodb_open();
		horatio_int_mongodb_logmsg(File, Line, Severity, Message);
#endif /*MONGO*/
	}
	return;
}
