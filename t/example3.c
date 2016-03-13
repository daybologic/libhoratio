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
 * This example should help end users to understand how to convert
 * old code (code which has not been built around the intimate HORATIO
 * functions) into code which can use HORATIO without being changed too
 * much.  Remember to use the define in every module.  The define
 * requires HORATIO version 1.1.4, the NULL array used by the macros
 * secretly only came about in HORATIO 1.1.0 (I think), so don't try to
 * use it with the old proprietary closed source HORATIOs
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h>
#include <stdlib.h> /* Old code would be using this anyway but also for atexit() */
#include <string.h>

#ifdef HORATIO_THREADS_PTH
# ifdef HAVE_PTH_H
#  include <pth.h>
# endif /*HAVE_PTH_H*/
#endif /*HORATIO_THREADS_PTH*/

#ifdef HORATIO_HDRSTOP
# pragma hdrstop  /* Precompile above headers if compiler supports it */
#endif /*HORATIO_HDRSTOP*/

#define USING_HORATIO /* Activate malloc() etc as macros */
#include "horatio.h"

/* main() belongs to HORATIO, rename your old main my_main or something */
static unsigned int hookCounter(PS_HORATIO_DEBUGHOOKINFO debugHookInfo);
static int my_main(const int argc, const char* argv[]);
static void handler(char** vector); /* Incase we can't allocate enough */
static void PrintInfo(char** vector); /* Prints strings from vector */
static void InitVector(char** vector, unsigned int n);
static void Title(void); /* Just displays some information */
static void Version(void); /* Prints the library version */
static void strdup_test(void);

static unsigned short int hookCount = 0U;

int main(const int argc, const char *argv[]) {
	if ( atexit(horatio_Shutdown) == -1 ) {
		printf("Can\'t register horatio_Shutdown, aborting.\n");
		return EXIT_FAILURE;
	}

#ifdef HORATIO_THREADS_PTH
	if ( !pth_init() ) {
		puts("Can\'t initialise GNU Portable Threads\n");
		return EXIT_FAILURE;
	}
#endif /*HORATIO_THREADS_PTH*/

	horatio_Startup();
	return my_main(argc, argv);
}

static unsigned int hookCounter(PS_HORATIO_DEBUGHOOKINFO debugHookInfo) {
	printf("hookCounter called for %s\n", (const char *)debugHookInfo->Misc0);
	hookCount++;
	return 0;
}

static int my_main(const int argc, const char* argv[]) {
	/*
	 * This is where the original program will begin, here's a
	 * quick example of your program, allocating all the arguments,
	 * using the horatio_Dump and then releasing them.
	 */

	int i;
	char **copyvector; /* NULL terminated vector version of arguments */

	Title();
	strdup_test();
	horatio_InstallDebugHook(
		HORATIO_HOOK_ALLOC,
		hookCounter
	);

	/* Allocate vector */
	copyvector = (char**)calloc((argc + 1),sizeof(char*));
	if ( !copyvector ) {
		handler(copyvector);
		return EXIT_FAILURE;
	}
	InitVector(copyvector, argc + 1);

	for ( i = 0; i < argc; i++ ) {
		if ( argv[i] ) {
			if ( argv[i][0] ) { /* Not blank string */
				copyvector[i] = (char*)malloc(
					sizeof(char) * (strlen(argv[i]) + 1)
				);
				if ( !copyvector[i] ) {
					handler(copyvector);
					return EXIT_FAILURE;
				}
				strcpy(copyvector[i], argv[i]);
			}
		}
	}
	PrintInfo(copyvector);
#ifndef NDEBUG
	horatio_Dump(stdout);
#endif /*!NDEBUG*/
	handler(copyvector); /* normal clean up */

	printf("hookCount total %u\n", hookCount);
	return EXIT_SUCCESS;
}

static void handler(char** vector) {
	/* Vector cleaner */
	size_t i = 0U;

	if ( !vector ) return;
	while ( vector[i] ) {
		free(vector[i]);
		vector[i++] = NULL;
	}
	free(vector);
}

static void PrintInfo(char** vector) {
	size_t i = 0U;

	if ( !vector ) return;
	while ( vector[i] ) {
		printf(
			"Vector element %u : \"%s\"\n",
			(unsigned int)i, vector[i]
		);
		i++;
	}
}

static void InitVector(char** vector, unsigned int n) {
	unsigned int i;
	if ( !vector ) return;

	for ( i = 0U; i < n; i++ )
		vector[i] = NULL;
}

static void Title() {
	printf("HORATIO ");
	Version();
	printf(" example program.\n");
	printf("The command line is copied into a NULL\n");
	printf("terminated vector type as much as you like and see the owners\n");
	printf("of the individual addresses.\n\n");
}

static void Version() {
	S_HORATIO_VERSION version;

	horatio_Ver(&version); /* Call lib to get version */
	printf("%u.%u.%u", version.Major, version.Minor, version.Patch);

	if ( (version.Flags & HORATIO_VERSION_BETA) )
		fputc((int)'b', stdout); /* b for beta of library */
	if ( (version.Flags & HORATIO_VERSION_DEBUG) )
		printf("%s", " (debug)");
	if ( (version.Flags & HORATIO_VERSION_PRIVATE) )
		printf("%s", " (private)");
	if ( (version.Flags & HORATIO_VERSION_SPECIAL) )
		printf("%s", " (special)");
	if ( (version.Flags & HORATIO_VERSION_MT) )
		printf("%s", " (multi-threaded)");

	return;
}

static void strdup_test() {
	const char *testStr = "Horatio will soon arrive!";
	char *testStrCopy;

	testStrCopy = strdup(testStr);
	if ( testStrCopy && strcmp(testStrCopy, testStr) == 0 ) {
		printf("strdup returned \"%s\": OK\n", testStrCopy);
		free(testStrCopy);
		return;
	}
	puts("strdup failed");
	abort();
}
