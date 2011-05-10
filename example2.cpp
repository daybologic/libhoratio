/*
    DPCRTLMM Memory Management Library example program (C)
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
Get updates: http://daybologic.com/Dev/dpcrtlmm
My official site: http://daybologic.com/overlord
*/


/* This is my little test app for my DPCRTLMM library
   I use it to simmulate a caller to debug my library - C++ version */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpcrtlmm.h"
#include "dpccap.h"

static void PrintStats(const PS_DPCRTLMM_STATS PStats);
static void InitArrays(void);
static void PrintVersion(void);
static void myTrapHandler(const unsigned int TrapID, const char* TrapMsg);
static void* _nullarrptrs[32]; /* 32 * 32k = 1MB */
static void* _arrptrs[64]; /* 64 * 16k = 1MB */

#ifndef __NO_NAMESPACES__
  using namespace Overlord;
#endif /*!__NO_NAMESPACES__*/
/*-------------------------------------------------------------------------*/
int main(const int argc, const char* argv[])
{
  unsigned int i;
  S_DPCRTLMM_STATS stats;
  TDPCRTLMM_BlockArray blockDesc;

  InitArrays();
  MemManager.InstallTrapCallback(myTrapHandler, 1U); /* Wow, a hook for a change ;-), I just wanted the stats on trap */
  printf("DPCRTLMM TEST\n");
  printf("-------------\n\n");
  printf("starting library\n");
  PrintVersion();

  printf("ATTENTION! ATTENTION! 2MB test + leaks!!!!\n\n");
  printf("NULL array test: ");
  printf("Allocating 32 blocks of 32KB (1MB) - in NULL block array\n");
  printf("Block: ");
  for ( i = 0U; i < sizeof(_nullarrptrs)/sizeof(_nullarrptrs[0]); i++ )
  {
    printf("#%d ", i);
    _nullarrptrs[i] = MemManager.Calloc(32, 1024); /* Allocate block */
    if (!_nullarrptrs[i])
      printf("Failure");
  }
  printf("\n\n");

  /* Output the statistics */
  MemManager.GetStats(&stats);
  PrintStats(&stats);

  printf("Allocating the explictly allocated block array, 64 16KB blocks\n");
  printf("Block ");
  for ( i = 0U; i < sizeof(_arrptrs)/sizeof(_arrptrs[0]); i++ )
  {
    printf("#%d ", i);
    _arrptrs[i] = blockDesc.Calloc(16, 1024); /* Allocate block */
    if (!_arrptrs[i])
      printf("Failure");
  }
  printf("\n\n");

  /* Output stats again, this will test the peak indicator */
  MemManager.GetStats(&stats);
  PrintStats(&stats);

  printf("I'm freeing everything, if a random number is even I will \"forget\" to\n");
  printf("to free the first block in the dynamic array\n");

  for ( i = 0U; i < sizeof(_nullarrptrs)/sizeof(_nullarrptrs[0]); i++ )
    MemManager.Free(_nullarrptrs[i]);

  srand((unsigned)time(NULL));
  for ( i = 0U; i < sizeof(_arrptrs)/sizeof(_arrptrs[0]); i++ )
  {
    if ( i == 0U && !(rand()%2) ) i++; /* Skip a block */
    blockDesc.Free(_arrptrs[i]);
  }

  printf("stop (wait for info dump if applicable!!)\n");
  MemManager.GetStats(&stats);
  PrintStats(&stats);

  printf("Successful execution\n");
  return 0;
}
/*-------------------------------------------------------------------------*/
static void PrintStats(const PS_DPCRTLMM_STATS PStats)
{
  if (PStats)
  {
    printf("Blocks allocated: %lu (peaked at %lu)\n", PStats->Blocks.Allocated, PStats->Blocks.Peak);
    printf("Amount locked: %lu\n", PStats->Blocks.Locked);
    printf("Amount marked as unswappable: %lu\n", PStats->Blocks.Unswappable);
    printf("Charge: %lu, peak: %lu\n", PStats->Charge.Allocated, PStats->Charge.Peak);
  }
}
/*-------------------------------------------------------------------------*/
static void InitArrays()
{
  size_t i;

  for ( i = 0U; i < sizeof(_nullarrptrs)/sizeof(_nullarrptrs[0]); i++ )
    _nullarrptrs[i] = NULL;

  for ( i = 0U; i < sizeof(_arrptrs)/sizeof(_arrptrs[0]); i++ )
    _arrptrs[i] = NULL;
}
/*-------------------------------------------------------------------------*/
static void PrintVersion()
{
  S_DPCRTLMM_VERSION ver;

  printf("Gathering library version info...");
  dpcrtlmm_Ver(&ver);
  printf("Version: %u.%u.%u", ver.Major, ver.Minor, ver.Patch);
  if ((ver.Flags) & 1) printf("b"); /* BETA */
  printf("\n");
}
/*-------------------------------------------------------------------------*/
static void myTrapHandler(const unsigned int TrapID, const char* TrapMsg)
{
  /* Why the hell am I handling this trap?  I'm not, I install this as a
  hook only.  Which means control is returned to the DPCRTLMM rap handler
  afterwards. */

  S_DPCRTLMM_STATS stats;
  printf("------------Caution! Stats display from trap hook!-----------\n\n");
  MemManager.GetStats(&stats);
  PrintStats(&stats);
  MemManager.Dump(stdout);
  /* What could I have done with the trap message?  Sorry for the warning,
  hey it's only a test */
  return;
}
/*-------------------------------------------------------------------------*/
