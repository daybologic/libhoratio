/* This is my little test app for C's DPCRTLMM library
all my my own hand :) */

#include <stdio.h>
#include <string.h>
#pragma hdrstop

#include "dpcrtlmm.h"

static void PrintStats(const PS_DPCRTLMM_STATS PStats);
/*-------------------------------------------------------------------------*/
int main(const int argc, const char* argv[])
{
  unsigned int i;
  S_DPCRTLMM_STATS stats;
  void* ptrs[32];
  PS_DPCRTLMM_BLOCKDESCARRAY Parr;
  unsigned int allocs = 0U;

  printf("DPCRTLMM TEST\n");
  printf("-------------\n\n");
  printf("starting libraruy\n");
  dpcrtlmm_Startup();
  Parr = dpcrtlmm_CreateBlockArray();

  for (i=0;i<sizeof(ptrs)/sizeof(ptrs[0]); i++)
    ptrs[i] = NULL;

  printf("WARNING! WARNING! 1GB test + leaks!!!!\n\n");
  printf("NULL array test: ");
  printf("Allocating 5 blocks in NULL array\n");
  for (i=0; i<sizeof(ptrs)/sizeof(ptrs[0]); i++)
  {
    printf("\rBlock #%d ", i);
    ptrs[i] = dpcrtlmm_Alloc(NULL, 1024); /* Allocate block */
    if (ptrs[i])
    {
      printf("Success!");
      allocs++;
    }
    else
    {
      printf("Failure");
    }
  }
  printf("\n\n");

  /* Output the statistics */
  dpcrtlmm_GetStats(&stats);
  PrintStats(&stats);

  printf("Freeing half allocated blocks...\n");
  for ( i = 0; i < allocs /2; i++ )
  {
    printf("\rBlock #%d", i);
    if (ptrs[i])
      dpcrtlmm_Free(NULL, ptrs[i]);
  }

  /* Output stats again, this will test the peak indicator */
  dpcrtlmm_GetStats(&stats);
  PrintStats(&stats);

  printf("stop (wait for info dump!!)\n");
  dpcrtlmm_DestroyBlockArray(Parr);
  dpcrtlmm_Shutdown();

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