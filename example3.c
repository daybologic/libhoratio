/*
    DPCRTLMM Memory Manager transparent usage example.
    Copyright (C) 2000 Overlord David Duncan Ross Palmer, Daybo Logic.
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

/* This example should help end users to understand how to convert
old code (code which has not been built around the intimate DPCRTLMM
functions) into code which can use DPCRTLMM without being changed too
much.  Remember to use the define in every module.  The define
requires DPCRTLMM version 1.1.4, the NULL array used by the macros
secretly only came about in DPCRTLMM 1.1.0 (I think), so don't try to
use it with the old proprietary closed source DPCRTLMMs */

#include <stdio.h> /* Required for dpcrtlmm.h since 1.1.4 */
#include <stdlib.h> /* Old code would be using this anyway but also for atexit() */
#ifdef HDRSTOP
#  pragma hdrstop  /* Precompile above headers if compiler supports it */
#endif

#define USING_DPCRTLMM /* Activate malloc() etc as macros */
#include "dpcrtlmm.h"

/* main() belongs to DPCRTLMM, rename your old main my_main or something */
static int my_main(const int argc, const char* argv[]);
static void handler(char** vector); /* Incase we can't allocate enough */
static void PrintInfo(char** vector); /* Prints strings from vector */
static void InitVector(char** vector, unsigned int n);
static void Title(void); /* Just displays some information */
static void Version(void); /* Prints the library version */

int main(const int argc, const char* argv[])
{
  if ( atexit(dpcrtlmm_Shutdown) == -1 ) {
    printf("Can\'t register dpcrtlmm_Shutdown, aborting.\n");
    return EXIT_FAILURE;
  }
  dpcrtlmm_Startup();
  return my_main(argc, argv);
}

static int my_main(const int argc, const char* argv[])
{
  /* This is where the original program will begin, here's a
  quick example of your program, allocating all the arguments,
  using the dpcrtlmm_Dump and then releasing them. */

  int i;
  char** copyvector; /* NULL terminated vector version of arguments */

  Title(); /* Nothing important */
  copyvector = (char**)malloc( (argc + 1) * sizeof(char*) ); /* Allocate vector (array of pointers) */
  if ( !copyvector ) {
    handler(copyvector);
    return EXIT_FAILURE;
  }
  InitVector(copyvector, argc + 1);

  for ( i = 0; i < argc; i++ ) {
    if ( argv[i] ) {
      if ( argv[i][0] ) { /* Not blank string */
        copyvector[i] = (char*)malloc( sizeof(char) * (strlen(argv[i]) + 1) );
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
  dpcrtlmm_Dump(stdout);
#endif /*!NDEBUG*/
  handler(copyvector); /* normal clean up */
  return EXIT_SUCCESS;
}

static void handler(char** vector)
{
  /* Vector cleaner */
  if ( vector ) {
    size_t i = 0U;

    while ( vector[i] ) {
      free(vector[i]);
      vector[i++] = NULL;
    }
    free(vector);
  }
}

static void PrintInfo(char** vector)
{
  if ( vector ) {
    size_t i = 0U;

    while ( vector[i] ) {
      printf("Vector element %u : \"%s\"\n", i, vector[i]);
      i++;
    }
  }
}

static void InitVector(char** vector, unsigned int n)
{
  if ( vector ) {
    unsigned int i;

    for ( i = 0U; i < n; i++ )
      vector[i] = NULL;
  }
}

static void Title()
{
  printf("DPCRTLMM ");
  Version();
  printf(" example program.  The command line is copied into a NULL\n");
  printf("terminated vector type as much as you like and see the owners\n");
  printf("of the individual addresses.\n\n");
}

static void Version()
{
  S_DPCRTLMM_VERSION version;

  dpcrtlmm_Ver(&version); /* Call lib to get version */
  printf("%u.%u.%u", version.Major, version.Minor, version.Patch);
  if ( (version.Flags & 1) == 1 ) /* Test build bit set? */
    fputc((int)'b', stdout); /* b for beta of library */
  return;
}
