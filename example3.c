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

/* This little example attempts to overload the library with too much work,
   it creates as much stress & load as it can */

#include <stdio.h>
#include <stdlib.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "dpcrtlmm.h"
#define DPCRTLMM_SOURCE /* Hack to allow library violation */
/* The following are library violations! */
#include "build.h"
#include "intdata.h"
#include "safelist.h"
/*--------------------------------------------------------------------------*/
static void Title(void);
/*--------------------------------------------------------------------------*/
int main()
{
  S_DPCRTLMM_BLOCKDESCARRAY* currentArrayPtr;
  unsigned int count = 0U;

  Title();

  dpcrtlmm_Startup();
  do
  {
    currentArrayPtr = dpcrtlmm_CreateBlockArray();
    if (currentArrayPtr)
    {
      size_t j;
      void* blocks[5];

      count++;
      for ( j = 0; j < sizeof(blocks)/sizeof(blocks[0]); j++ )
      {
        blocks[j] = dpcrtlmm_Alloc(currentArrayPtr, (1024*1024)*5);
        if (!blocks[j])
          printf("Allocation %u failed block iterator: %u\n", j, count);
      }

      for ( j = 0; j < sizeof(blocks)/sizeof(blocks[0]); j++ )
      {
        if (blocks[j])
          dpcrtlmm_Free(currentArrayPtr, blocks[j]);
      }
    }
  } while (currentArrayPtr);
  printf("Created %u arrays\n", count);

  printf("Cleaning up (library violation)\n");
  for ( count = 0U; count < sizeof(dpcrtlmm_int__safetyList)/sizeof(dpcrtlmm_int__safetyList[0]); count++ )
  {
    if ( dpcrtlmm_int__safetyList[count] )
      dpcrtlmm_DestroyBlockArray(dpcrtlmm_int__safetyList[count]);
  }

  dpcrtlmm_Shutdown();
  return EXIT_SUCCESS;
}
/*--------------------------------------------------------------------------*/
static void Title()
{
  printf("DPCRTLMM!  Stress!  Have a caution\n");
  printf("----------------------------------\n\n");
}
