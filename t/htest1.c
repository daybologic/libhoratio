/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2014, David Duncan Ross Palmer, Daybo Logic
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

#define HORATIO_SOURCE /* We need to pretend to be internal */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#ifdef HDRSTOP
# pragma hdrstop
#endif /*HDRSTOP*/

#include "horatio.h"
#include "hbuild.h"

#define DIE(m) Die((__FILE__), (__LINE__), (m))

enum testResultType {
	TEST_FAIL = 1,
	TEST_OK = 0
};

int main(int argc, char *argv[]);

/* Command-line argument processing */
static bool ProcessOptions(int ArgC, char **ArgV);

/* Suite initialisation routines */
static int init_suite_core(void); /* Core library implementation testing: horatio.c  */
static int init_suite_trap(void); /* Trap function testing: dpc_trap.c */
static int init_suite_dbghook(void); /* Debug hook testing: hdbghook.c */
static int init_suite_alloc(void); /* Alloc function test: dpc_alloc.c */
static int init_suite_log(void); /* Log function test: hlog.c */

/* Suite cleanup routines */
static int clean_suite_core(void);
static int clean_suite_trap(void);
static int clean_suite_dbghook(void);
static int clean_suite_alloc(void);
static int clean_suite_log(void);

/* This function aborts the program under extraordinary circumstances */
static void Die(const char *File, const unsigned int Line, const char *Message);

/* Simple support for concatinating short sequences of strings in tests,
you should call Unglue() as soon as the tests are finished */
static const char *Glue(const char *Str1, const char *Str2);
static void Unglue(void);

/* Test suite core */
static void suite_core_Ver(void);

/* Test suite trap */
static void suite_trap_InstallTrapCallback(void);

/* Test suite debug hook */
static void suite_dbghook_InstallDebugHook(void);

/* Test suite allloc */
static void suite_alloc_AllocSimple(void);
static void suite_alloc_AllocLoop(void);

/* Test suite log */
static void suite_log_TODO(void);

/* Incidental functions */
static void test_TrapCallback(const unsigned int, const char*);

static char *GlueStrs[10]; /* A small cache used by the Glue() and Unglue() functions */
static bool SandboxStarted = false;
static unsigned short int DebugLevel = 0U;
static PS_HORATIO_BLOCKDESCARRAY BDASharedSingle = NULL;

static bool ProcessOptions(int ArgC, char **ArgV) {
	int c;
	opterr = 0;

	while ( (c = getopt(ArgC, ArgV, "d:")) != -1 ) {
		switch ( c ) {
			case 'd' : {
				DebugLevel = (unsigned int)(abs(atoi(optarg)));
				break;
			}
			default : {
				return false;
			}
		}
	}

	return true;
}

static int init_suite_core() {
	return 0;
}

static int init_suite_trap() {
	return 0;
}

static int init_suite_alloc() {
	if ( !BDASharedSingle ) {
		BDASharedSingle = horatio_CreateBlockArray();
		if ( !BDASharedSingle ) return 1;
		return 0;
	}

	return 1;
}

static int init_suite_dbghook() {
	return 0;
}

static int init_suite_log() {
	return 0;
}

static int clean_suite_core() {
	return 0;
}

static int clean_suite_trap() {
	return 0;
}

static int clean_suite_alloc() {
	if ( BDASharedSingle ) {
		horatio_DestroyBlockArray(BDASharedSingle);
		BDASharedSingle = NULL;
		return 0;
	}

	return 1;
}

static int clean_suite_log() {
	return 0;
}

static void Die(const char *File, const unsigned int Line, const char *Message) {
	unsigned int l = strlen(Message);
	if ( l && l-1 == '\n' ) {
		fprintf(stderr, "%s", Message);
	} else {
		fprintf(stderr, "%s at line %u in %s\n", File, Line, Message);
	}
	abort();
}

static const char *Glue(const char *Str1, const char *Str2) {
	static const char empty[] = "";
	size_t i;
	int loc = -1;
	size_t c = sizeof(GlueStrs)/sizeof(GlueStrs[0]);
	for ( i = 0U; i < c; i++ ) {
		if ( !GlueStrs[i] ) {
			size_t allocSize = strlen(Str1)+strlen(Str2)+1;
			GlueStrs[i] = malloc(allocSize);
			if ( GlueStrs[i] ) {
				strcpy(GlueStrs[i], Str1);
				strcat(GlueStrs[i], Str2);
				loc = i;
				break;
			} else {
				fprintf(stderr, "Glue() malloc(%u) failed at %s L%u\n", (unsigned int)allocSize, __FILE__, __LINE__);
				return empty;
			}
		}
	}

	if ( loc == -1 ) {
		fprintf(stderr, "Glue() all %u temporary slots exhausted at %s L%u\n", (unsigned int)c, __FILE__, __LINE__);
		return empty;
	}
	return GlueStrs[loc];
}

static void Unglue() {
	size_t i;
	for ( i = 0U; i < sizeof(GlueStrs)/sizeof(GlueStrs[0]); i++ ) {
		if ( GlueStrs[i] ) {
			free(GlueStrs[i]);
			GlueStrs[i] = NULL;
		} else {
			break;
		}
	}
}

int main(int argc, char *argv[]) {
	CU_ErrorCode err;
	unsigned int failCount;
	size_t testI, suiteI;
	static struct {
		const char *TestName;
		void (*TestFunc)(void);
	} CoreTests[] = {
		{ "Ver",
		  &suite_core_Ver
		}
	};
	static struct {
		const char *TestName;
		void (*TestFunc)(void);
	} TrapTests[] = {
		{ "TrapInstallTrapCallback",
		  &suite_trap_InstallTrapCallback
		}
	};
	static struct {
		const char *TestName;
		void (*TestFunc)(void);
	} AllocTests[] = {
		{ "AllocSimple",
		  &suite_alloc_AllocSimple
		}, {
		  "AllocLoop",
		  &suite_alloc_AllocLoop
		}
	};
	static struct {
		const char *TestName;
		void (*TestFunc)(void);
	} LogTests[] = {
		{ "TODO",
		 &suite_log_TODO
		}
	};

	/* Suites */
	static struct {
		const char *SuiteName;
		int (*Init)(void);
		int (*Clean)(void);
	} Suites[] = {
		{ "suite_core", &init_suite_core, &clean_suite_core },
		{ "suite_trap", &init_suite_trap, &clean_suite_trap },
		{ "suite_dbghook", &init_suite_dbghook, &clean_suite_dbghook },
		{ "suite_alloc", &init_suite_alloc, &clean_suite_alloc },
		{ "suite_log", &init_suite_log, &clean_suite_log },
	};

	CU_pSuite pSuite[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

	if ( !ProcessOptions(argc, argv) ) return EXIT_FAILURE; /* Bail, if command-line arguments are bad */

	/* initialize the CUnit test registry */
	if ( CU_initialize_registry() != CUE_SUCCESS )
		return CU_get_error();

	/* Add all suites to the registry*/
	for ( suiteI = 0U; suiteI < sizeof(Suites)/sizeof(Suites[0]); suiteI++ ) {
		pSuite[suiteI] = CU_add_suite(Suites[suiteI].SuiteName, Suites[suiteI].Init, Suites[suiteI].Clean);
		if ( !pSuite[suiteI] ) {
			CU_cleanup_registry();
			return CU_get_error();
		}
	}
	suiteI = 0U;

	/* add the tests to the suite */
	for ( testI = 0; testI < sizeof(CoreTests)/sizeof(CoreTests[0]); testI++ ) {
		if ( !CU_add_test(pSuite[suiteI], CoreTests[testI].TestName, CoreTests[testI].TestFunc) ) {
			CU_cleanup_registry();
			return CU_get_error();
		}
	}
	suiteI++;
	for ( testI = 0; testI < sizeof(TrapTests)/sizeof(TrapTests[0]); testI++ ) {
		if ( !CU_add_test(pSuite[suiteI], TrapTests[testI].TestName, TrapTests[testI].TestFunc) ) {
			CU_cleanup_registry();
			return CU_get_error();
		}
	}
	suiteI++;
	for ( testI = 0; testI < sizeof(AllocTests)/sizeof(AllocTests[0]); testI++ ) {
		if ( !CU_add_test(pSuite[suiteI], AllocTests[testI].TestName, AllocTests[testI].TestFunc) ) {
			CU_cleanup_registry();
			return CU_get_error();
		}
	}
	suiteI++;
	for ( testI = 0; testI < sizeof(DbghookTests)/sizeof(DbghookTests[0]); testI++ ) {
		if ( !CU_add_test(pSuite[suiteI], DbghookTests[testI].TestName, DbghookTests[testI].TestFunc) ) {
					CU_cleanup_registry();
			return CU_get_error();
		}
	}
	suiteI++;
	for ( testI = 0; testI < sizeof(LogTests)/sizeof(LogTests[0]); testI++ ) {
		if ( !CU_add_test(pSuite[suiteI], LogTests[testI].TestName, LogTests[testI].TestFunc) ) {
			CU_cleanup_registry();
			return CU_get_error();
		}
	}
	/*suiteI++;*/ /* Don't forget to uncomment this before adding more tests */

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	horatio_Startup();
	CU_basic_run_tests();
	failCount = CU_get_number_of_failure_records();
	CU_cleanup_registry();
	err = CU_get_error();
	horatio_Shutdown();
	if ( err != CUE_SUCCESS ) return err;
	if ( failCount ) return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

static void suite_core_Ver() {
	S_HORATIO_VERSION ver;
	PS_HORATIO_VERSION pver;

	pver = horatio_Ver(&ver);
	CU_ASSERT_PTR_EQUAL(pver, &ver);
	CU_ASSERT_EQUAL(pver->Major, HORATIO_VERSION_MAJOR);
	CU_ASSERT_EQUAL(pver->Minor, HORATIO_VERSION_MINOR);
	CU_ASSERT_EQUAL(pver->Patch, HORATIO_VERSION_PATCH);
}

static void suite_trap_InstallTrapCallback() {
	horatio_InstallTrapCallback(test_TrapCallback, 0);
}

static void suite_dbghook_InstallDebugHook() {
	// TODO
}

static void suite_alloc_AllocSimple() {
	void HORATIO_FARDATA *ptrDefault, *ptrSharedSingle;

	ptrDefault = horatio_Alloc(NULL, 1024);
	CU_ASSERT_PTR_NOT_NULL(ptrDefault);
	ptrSharedSingle = horatio_Alloc(BDASharedSingle, 1024);
	CU_ASSERT_PTR_NOT_NULL(ptrSharedSingle);

	horatio_Free(NULL, ptrDefault);
	horatio_Free(BDASharedSingle, ptrSharedSingle);
}

static void suite_alloc_AllocLoop() {
	unsigned int blockI;
	void HORATIO_FARDATA *blocksDefault[48];
	void HORATIO_FARDATA *blocksSharedSingle[64];

	for ( blockI = 0U; blockI < sizeof(blocksDefault)/sizeof(blocksDefault[0]); blockI++ ) {
		blocksDefault[blockI] = horatio_Alloc(NULL, blockI * 32);
		CU_ASSERT_PTR_NOT_NULL(blocksDefault[blockI]);
	}
	for ( blockI = 0U; blockI < sizeof(blocksSharedSingle)/sizeof(blocksSharedSingle[0]); blockI++ ) {
		blocksSharedSingle[blockI] = horatio_Alloc(BDASharedSingle, blockI * 96);
		CU_ASSERT_PTR_NOT_NULL(blocksSharedSingle[blockI]);
	}

	/* Cleanup */
	for ( blockI = 0U; blockI < sizeof(blocksDefault)/sizeof(blocksDefault[0]); blockI++ ) {
		horatio_Free(NULL, blocksDefault[blockI]);
	}
	for ( blockI = 0U; blockI < sizeof(blocksSharedSingle)/sizeof(blocksSharedSingle[0]); blockI++ ) {
		horatio_Free(BDASharedSingle, blocksSharedSingle[blockI]);
	}
}

static void suite_log_TODO() {
	horatio_int_Log(
		"dummy.c", /* File */
		1234, /* Line */
		0, /* Severity FIXME: forgotten what we do here, Y U NO HAVE ENUM? */
		"Horatio has merely sniffed his breakfast" /* Message */
	);
}

static void test_TrapCallback(const unsigned int tn, const char* str) {
}
