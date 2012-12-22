/*
Daybo Logic C RTL Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer, Daybo Logic
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


/* This is my little test app for my DPCRTLMM library
   I use it to simmulate a caller to debug my library - C++ version */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifdef DPCRTLMM_THREADS_PTH
# ifdef HAVE_PTH_H
#  include <pth.h>
# endif /*HAVE_PTH_H*/
#endif /*DPCRTLMM_THREADS_PTH*/

#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#define USING_DPCRTLMM /* Needed for calloc() re-definition */
#include "dpcrtlmm.h"
#include "dpccap.h"

static void PrintStats(const PS_DPCRTLMM_STATS PStats);
static void InitArrays(void);
static void PrintVersion(void);
static void myTrapHandler(const unsigned int TrapID, const char* TrapMsg);
static void* nullarrptrs[4];
static void* arrptrs[16];

#ifndef __NO_NAMESPACES__
  using namespace Overlord;
#endif /*!__NO_NAMESPACES__*/
/*-------------------------------------------------------------------------*/
int main() {
  unsigned int i;
  S_DPCRTLMM_STATS stats;
  TDPCRTLMM_BlockArray blockDesc(false);

#ifdef DPCRTLMM_THREADS_PTH
  if ( !pth_init() ) {
    puts("Can\'t initialise GNU Portable Threads\n");
    return EXIT_FAILURE;
  }
#endif /*DPCRTLMM_THREADS_PTH*/

  MemManager.Startup();
  blockDesc.Init();
  InitArrays();
  MemManager.InstallTrapCallback(myTrapHandler, 1U); /* Wow, a hook for a change ;-), I just wanted the stats on trap */
  printf("DPCRTLMM TEST\n");
  printf("-------------\n\n");
  printf("starting library\n");
  PrintVersion();
  printf("NULL array test: ");
  printf("Allocating blocks in NULL block array\n");
  printf("Block: ");
  for ( i = 0U; i < sizeof(nullarrptrs)/sizeof(nullarrptrs[0]); i++ ) {
    printf("#%d ", i);
    nullarrptrs[i] = calloc(32, 8); /* Allocate block */
    if (!nullarrptrs[i])
      printf("Failure");
  }
  printf("\n\n");

  /* Output the statistics */
  MemManager.GetStats(&stats);
  PrintStats(&stats);

  printf("Allocating blocks in explictly allocated block array\n");
  printf("Block ");
  for ( i = 0U; i < sizeof(arrptrs)/sizeof(arrptrs[0]); i++ ) {
    printf("#%d ", i);
    arrptrs[i] = dpcrtlmm_block_Calloc(blockDesc, 8, 128); /* Allocate block */
    if (!arrptrs[i])
      printf("Failure");
  }
  printf("\n\n");

  /* Output stats again, this will test the peak indicator */
  MemManager.GetStats(&stats);
  PrintStats(&stats);

  for ( i = 0U; i < sizeof(nullarrptrs)/sizeof(nullarrptrs[0]); i++ )
    MemManager.Free(nullarrptrs[i]);

  for ( i = 0U; i < sizeof(arrptrs)/sizeof(arrptrs[0]); i++ )
    blockDesc.Free(arrptrs[i]);

  printf("stop (wait for info dump if applicable!!)\n");
  MemManager.GetStats(&stats);
  PrintStats(&stats);

  printf("Successful execution\n");
  return 0;
}
/*-------------------------------------------------------------------------*/
static void PrintStats(
  const PS_DPCRTLMM_STATS PStats
) {
  if (PStats) {
    printf("Blocks allocated: %lu (peaked at %lu)\n", PStats->Blocks.Allocated, PStats->Blocks.Peak);
    printf("Amount locked: %lu\n", PStats->Blocks.Locked);
    printf("Amount marked as unswappable: %lu\n", PStats->Blocks.Unswappable);
    printf("Charge: %lu, peak: %lu\n", PStats->Charge.Allocated, PStats->Charge.Peak);
  }
}
/*-------------------------------------------------------------------------*/
static void InitArrays() {
  size_t i;

  for ( i = 0U; i < sizeof(nullarrptrs)/sizeof(nullarrptrs[0]); i++ )
    nullarrptrs[i] = NULL;

  for ( i = 0U; i < sizeof(arrptrs)/sizeof(arrptrs[0]); i++ )
    arrptrs[i] = NULL;
}
/*-------------------------------------------------------------------------*/
static void PrintVersion()
{
  S_DPCRTLMM_VERSION ver;

  printf("Gathering library version info...");
  dpcrtlmm_Ver(&ver);
  printf("Version: %u.%u.%u", ver.Major, ver.Minor, ver.Patch);
  printf("\n");
}
/*-------------------------------------------------------------------------*/
static void myTrapHandler(
  const unsigned int TrapID,
  const char* TrapMsg
) {
  /* Why the hell am I handling this trap?  I'm not, I install this as a
  hook only.  Which means control is returned to the DPCRTLMM rap handler
  afterwards. */

  S_DPCRTLMM_STATS stats;
  printf("------------Caution! Stats display from trap hook!-----------\n");
  printf("TrapID %u, message: %s\n\n", TrapID, TrapMsg);
  MemManager.GetStats(&stats);
  PrintStats(&stats);
  MemManager.Dump(stdout);
  /* What could I have done with the trap message?  Sorry for the warning,
  hey it's only a test */
  return;
}
/*-------------------------------------------------------------------------*/
