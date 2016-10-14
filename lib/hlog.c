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

#ifdef HAVE_STDBOOL_H
 /* Only used in horatio_int_mongodb_logmsg() */
# include <stdbool.h>
#endif /*HAVE_STDBOOL_H*/

#ifdef USE_SQLITE
# include <sqlite3.h> /* For SQLite logging support */
#endif /*USE_SQLITE*/

#ifdef USE_MYSQL
# include <mysql.h> /* For MySQL logging support */
#endif /*USE_MYSQL*/

#ifdef USE_MONGO
# include <mongo.h> /* For MongoDB logging support */
#endif /*USE_MONGO*/

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

#ifdef USE_SQLITE
static sqlite3 *horatio_int_sqlite3_open(void);
static void horatio_int_sqlite3_schema(sqlite3 *);
#endif /*USE_SQLITE*/

#ifdef USE_MYSQL
static MYSQL *horatio_int_mysql_open(void);
static void horatio_int_mysql_schema(MYSQL *);
#endif /*USE_MYSQL*/

#ifdef USE_MONGO
static mongo_sync_connection *horatio_int_mongodb_open(void);
#endif /*USE_MONGO*/

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
#ifdef USE_SQLITE
static void horatio_int_sqlite3_logmsg(
	const unsigned short Code,
	const char *,
	const unsigned int,
	const unsigned short,
	const char *
);
#endif /*USE_SQLITE*/

#ifdef USE_MYSQL
static void horatio_int_mysql_logmsg(
	const unsigned short Code,
	const char *,
	const unsigned int,
	const unsigned short,
	const char *
);
#endif /*USE_MYSQL*/

#ifdef USE_SQLITE
static sqlite3 *Handle_sqlite = NULL;
#endif /*USE_SQLITE*/

#ifdef USE_MYSQL
static MYSQL *Handle_mysql;
#endif /*USE_MYSQL*/

#ifdef USE_MONGO
static mongo_sync_connection *Handle_mongo;
//static mongoc_collection_t *collection;
#endif /*USE_MONGO*/

#ifdef USE_SQLITE
static sqlite3 *horatio_int_sqlite3_open() {

	sqlite3 *dbh;
	const char *errMsgPtr = NULL;
	int rc = sqlite3_open("horatio.db", &dbh);
	if ( rc ) { // Fail?
		errMsgPtr = sqlite3_errmsg(dbh);
		Trap(0, errMsgPtr);
		sqlite3_close(dbh);
	} else {
		horatio_int_sqlite3_schema(dbh);
	}

	return dbh;
}

static void horatio_int_sqlite3_schema(sqlite3 *dbHandle) {

	int rc;
	const char *const q =
		"CREATE TABLE IF NOT EXISTS debug_log (\n"
			"id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
			"code INTEGER NOT NULL,\n"
			"ts DATETIME NOT NULL,\n"
			"file CHAR(32) NOT NULL,\n"
			"line INTEGER NOT NULL,\n"
			"severity INTEGER NOT NULL default 0,\n"
			"msg VARCHAR(255) NOT NULL\n"
		");";

	rc = sqlite3_exec(dbHandle, q, 0, 0, NULL);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error %u from sqlite3_exec\n", rc);
		return;
	}

	return;
}

static void horatio_int_sqlite3_logmsg(
	const unsigned short Code,
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Msg
) {
	int rc;
	sqlite3_stmt *stmt;
	const char *q =
		"INSERT INTO debug_log (ts, code, file, line, severity, msg) \n"
		"VALUES(\n"
		"  DATETIME('NOW', 'localtime'), ?, ?, ?, ?, ?\n"
		")";

	if ( !Handle_sqlite ) return;

	rc = sqlite3_prepare_v2(Handle_sqlite, q, strlen(q), &stmt, NULL);
	if ( rc != SQLITE_OK ) {
		fprintf(stderr, "Error %u from sqlite3_prepare_v2\n", rc);
		return;
	}

	rc = sqlite3_bind_int(stmt, 1, Code);
	rc = sqlite3_bind_text(stmt, 2, File, -1, SQLITE_STATIC);
	rc = sqlite3_bind_int(stmt, 3, Line);
	rc = sqlite3_bind_int(stmt, 4, Severity);
	rc = sqlite3_bind_text(stmt, 5, Msg, -1, SQLITE_STATIC);
	if ( rc != SQLITE_OK )
		fprintf(stderr, "Error %u from sqlite3_bind_text\n", rc);

	rc = sqlite3_step(stmt);
	if ( rc != SQLITE_DONE )
		fprintf(stderr, "Error %u from sqlite3_step\n", rc);

	rc = sqlite3_finalize(stmt); // Destroy the handle (FIXME, you should re-use it).
	if ( rc != SQLITE_OK )
		fprintf(stderr, "Error %u from sqlite3_finalize\n", rc);

	return;
}
#endif /*USE_SQLITE*/

#ifdef USE_MONGO
static void horatio_int_mongodb_logmsg(
	const unsigned short,
	const char *,
	const unsigned int,
	const unsigned short,
	const char *
);

static mongo_sync_connection *horatio_int_mongodb_open() {
	mongo_sync_connection *client;
	//mongoc_uri_t *uri;
	//mongoc_insert_flags_t flags = MONGOC_INSERT_NONE;
	//mongoc_write_concern_t *write_concern;
	//bson_error_t error;
	//const char *const uriString = "mongodb://username:password@localhost:0/test";

	//uri = mongoc_uri_new(uriString);
	//client = mongoc_client_new_from_uri(uri);
	client = mongo_sync_connect("localhost", 27017, true);
	if (client) {
		mongo_sync_conn_set_auto_reconnect(client, true);
		mongo_sync_cmd_is_master(client);
		if (!mongo_sync_cmd_authenticate(client, "test", "test", "")) {
			fprintf(stderr, "Error: mongo_sync_cmd_authenticate\n");
			perror("mongo_sync_cmd_authenticate");
			mongo_sync_disconnect(client);
			return NULL;
		}
	} else {
		fprintf(stderr, "Error: mongo_sync_connect\n");
		perror("mongo_sync_connect");
		return client;
	}

	return client;
}

static void horatio_int_mongodb_logmsg(
	const unsigned short Code,
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Msg
) {
	//bson_t document;
	bson *document;
	bool qsl;

	//if ( !client || !collection ) return;
	if (!Handle_mongo) return;
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
#endif /*USE_MONGO*/

#ifdef USE_MYSQL
static MYSQL *horatio_int_mysql_open() {

	const char *errMsgPtr = NULL;
	MYSQL *dbh = mysql_init(NULL);

	if (!dbh) {
		fprintf(stderr, "mysql_init: %s\n", mysql_error(dbh));
		return NULL;
	}

	if (mysql_real_connect(dbh, "localhost", "root", "", "horatio", 0, NULL, 0) == NULL) {
		// Fail?
		mysql_close(dbh);
		errMsgPtr = "FIXME";
		Trap(0, errMsgPtr);
		return NULL;
	} else {
		horatio_int_mysql_schema(dbh);
		Handle_mysql = dbh;
	}

	return Handle_mysql;
}

static void horatio_int_mysql_schema(MYSQL *dbHandle) {

	int rc;
	const char *const q =
		"CREATE TABLE IF NOT EXISTS debug_log (\n"
			"id INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL,\n"
			"code INTEGER NOT NULL,\n"
			"ts DATETIME NOT NULL,\n"
			"file CHAR(32) NOT NULL,\n"
			"line INTEGER NOT NULL,\n"
			"severity INTEGER NOT NULL default 0,\n"
			"msg VARCHAR(255) NOT NULL,\n"
			"pid INTEGER NOT NULL\n" /* Process ID */
		");";

	rc = mysql_query(dbHandle, q);
	if (rc) {
		fprintf(
			stderr,
			"Error %u from mysql_query: %s\n",
			rc,
			mysql_error(dbHandle)
		);
		return;
	}

	return;
}

static void horatio_int_mysql_logmsg(
	const unsigned short Code,
	const char *File,
	const unsigned int Line,
	const unsigned short Severity,
	const char *Msg
) {
	my_bool ret;
	int rc;
	pid_t pid;
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[6];
	unsigned long fileStrlen, msgStrlen;

	const char *q = "INSERT INTO debug_log (ts, code, file, line, severity, msg, pid) \n"
	"VALUES(\n"
	"  NOW(), ?, ?, ?, ?, ?, ?\n"
	")";

	memset(bind, 0, sizeof(bind));
	if (!Handle_mysql) return;

	fprintf(stderr, "Got database message %s\n", Msg);
	fprintf(stderr, "Executing query: %s\n", q);
	stmt = mysql_stmt_init(Handle_mysql);
	if (!stmt) {
		fprintf(
			stderr,
			"Error from mysql_stmt_init: %s\n",
			mysql_error(Handle_mysql)
		);
		return;
	}

	rc = mysql_stmt_prepare(stmt, q, strlen(q)); // TODO prepare once
	if (rc) {
		fprintf(
			stderr,
			"Error %d from mysql_stmt_prepare: %s\n",
			rc, mysql_error(Handle_mysql)
		);
		return;
	}

	pid = getpid(); /* Obtain process ID */

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (char *)&Code;
	bind[0].length = 0;
	bind[0].is_null = 0;

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (void *)File;
	fileStrlen = strlen(File);
	bind[1].length = &fileStrlen;
	bind[1].is_null = 0;

	bind[2].buffer_type = MYSQL_TYPE_LONG;
	bind[2].buffer = (char *)&Line;
	bind[2].length = 0;
	bind[2].is_null = 0;

	bind[3].buffer_type = MYSQL_TYPE_SHORT;
	bind[3].buffer = (char *)&Severity;
	bind[3].length = 0;
	bind[3].is_null = 0;

	bind[4].buffer_type = MYSQL_TYPE_STRING;
	bind[4].buffer = (const void *)Msg;
	msgStrlen = strlen(Msg);
	bind[4].length = &msgStrlen;
	bind[4].is_null = 0;

	bind[5].buffer_type = MYSQL_TYPE_SHORT;
	bind[5].buffer = (char *)&pid;
	bind[5].length = 0;
	bind[5].is_null = 0;

	ret = mysql_stmt_bind_param(stmt, bind);
	if (ret) {
		fprintf(
			stderr,
			"mysql_stmt_bind_param: %s\n",
			mysql_error(Handle_mysql)
		);
		return;
	}

	if ((rc = mysql_stmt_execute(stmt))) {
		fprintf(
			stderr,
			"Error %d from mysql_execute: %s\n",
			rc, mysql_error(Handle_mysql)
		);
		return;
	}

	rc = mysql_stmt_close(stmt); // Destroy the handle (FIXME, you should re-use it).
}
#endif /*USE_MYSQL*/

void horatio_int_Log(
	const unsigned short Code,
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
	FILE *HLogFile; /* Handle for log file */

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
			case HORATIO_LOG_MESSAGE  : {
				/* HORATIO_LOG_MESSAGE has no string prefix */
				break;
			}
		}
		if ( File ) {
			STRNCAT_FIXEDBUFF(formatMsg, File);
			STRNCAT_FIXEDBUFF(formatMsg, ", L");
			STRNCAT_FIXEDBUFF(formatMsg, number);
			STRNCAT_FIXEDBUFF(formatMsg, ", ");
		}
		sprintf(
			number,
#ifdef HAVE_SNPRINTF
			sizeof(number)-1,
#endif /*HAVE_SNPRINTF*/
			"code %u, ",
			Code
		);
		STRNCAT_FIXEDBUFF(formatMsg, number); /* Add the Code */
		STRNCAT_FIXEDBUFF(formatMsg, Message);
		/* Close quotes and end line */
		STRNCAT_FIXEDBUFF(formatMsg, "\"\n");

		/*
		 * Determine what to do with the message
		 * based on it's severity.
		 */
		if (_options.enableLog) {
			/* Everything goes in the log... */
			/* Append/overwrite text file */
			HLogFile = fopen("HORATIO.LOG", "at");
			if (HLogFile) { /* Log opened? */
				/* Output log msg to log file */
				fputs(formatMsg, HLogFile);
				fclose(HLogFile); /* Close the log file */
			}
		}

		if ( Severity > HORATIO_LOG_MESSAGE ) {
			/* Anything more severe than a warning */
			fprintf(_options.errorHandle, "%s", formatMsg);
		}

		/* Potentially log to various enabled database engines */
#ifdef USE_SQLITE
		if ( !Handle_sqlite ) Handle_sqlite = horatio_int_sqlite3_open();
		horatio_int_sqlite3_logmsg(Code, File, Line, Severity, Message);
#endif /*USE_MYSQL*/

#ifdef USE_MYSQL
		if ( !Handle_mysql ) Handle_mysql = horatio_int_mysql_open();
		horatio_int_mysql_logmsg(Code, File, Line, Severity, Message);
#endif /*USE_MONGO*/

#ifdef USE_MONGO
		if (!Handle_mongo) Handle_mongo = horatio_int_mongodb_open();
		horatio_int_mongodb_logmsg(Code, File, Line, Severity, Message);
#endif /*USE_MONGO*/
	}
	return;
}
