/*
    DPCRTLMM Memory Management Library : Stats management
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
#define DPCRTLMM_SOURCE
/*
  Functions for returning statistics about memory handled by the library
  Programmer: Overlord David Duncan Ross Palmer
  Contact: Overlord@DayboLogic.co.uk
  Created: 27th July 2000
  Last modified: 11th Dec 2000
  Library: DPCRTLMM 1.0
  Language: ANSI C (1990)
  Revision #2

  11th Dec 2000 : Removed the structure that communicated between both
                  functions to carry counts of flags.  Bug fix, the
                  swappable and locked block counts in the statistics
                  didn't make it back to the caller's structure.

  24th May 2001 : Added dump and supported functions.
*/
#include <stddef.h>
#include <stdio.h>
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Public library header */
#include "intdata.h" /* Internal library data */

static void CountFlagsInUse(PS_DPCRTLMM_STATS PFlagsStats);
static void DumpOnArray(FILE* Target, PS_DPCRTLMM_BLOCKDESCARRAY CurrentArray);
static void CrackAndPrintFlags(FILE* Target, unsigned char Flags);
/*-------------------------------------------------------------------------*/
unsigned long dpcrtlmm_GetBlockCount()
{
  return _blockCount;
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_GetStats(PS_DPCRTLMM_STATS PReadStats)
{
  if (PReadStats)
  {
    PReadStats->Blocks.Allocated = dpcrtlmm_GetBlockCount();
    CountFlagsInUse(PReadStats); /* Loop through the entire load counting us flags */
    PReadStats->Blocks.Peak = _blockCountPeak;
    PReadStats->Charge.Allocated = _allocCharge;
    PReadStats->Charge.Peak = _allocPeak;
  }
}
/*-------------------------------------------------------------------------*/
static void CountFlagsInUse(PS_DPCRTLMM_STATS PFlagsStats)
{
  if (PFlagsStats)
  {
    unsigned int i;

    /* First reset the counts in the stats struct */
    PFlagsStats->Blocks.Locked = 0UL;
    PFlagsStats->Blocks.Unswappable = 0UL;
 
    /* Go through normal arrays */
    for ( i = 0U; i < DPCRTLMM_SAFETYLIST_MAXSIZE; i++ )
    {
      if (_safetyList[i]) /* Used entry? */
      {
	unsigned int j;
	for ( j = 0U; j < _safetyList[i]->Count; j++ )
	{
	  unsigned char flags = _safetyList[i]->Descriptors[j].Flags;

	  if ( (flags & 1) == 1) /* Lock bit set */
	    PFlagsStats->Blocks.Locked++;
	  if ( (flags & 2) == 2) /* NoSwap bit set */
	    PFlagsStats->Blocks.Unswappable++;
	}
      }
    }
    /* Extra support for the "NULL array" */
    #ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY /* Source purchasers can turn disable this support */
    for ( i = 0U; i < _defaultArray.Count; i++ )
    {
      unsigned char flags = _defaultArray.Descriptors[i].Flags;

      if ( (flags & 1) == 1) /* Lock bit set */
	PFlagsStats->Blocks.Locked++;
      if ( (flags & 2) == 2) /* NoSwap bit set */
	PFlagsStats->Blocks.Unswappable++;
    }
    #endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/
  }
}
/*-------------------------------------------------------------------------*/
void dpcrtlmm_Dump(FILE* Target)
{
  if ( Target ) {
    unsigned int i;

    for ( i = 0U; i < DPCRTLMM_SAFETYLIST_MAXSIZE; i++ ) {
      if ( _safetyList[i] ) { /* Used entry? */
        unsigned int j;

        for ( j = 0U; j < _safetyList[i]->Count; j++ )
          DumpOnArray(Target, _safetyList[i]);
      }
    }
    #ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY
    DumpOnArray(Target, &_defaultArray);
    #endif
  }
  return;
}
/*-------------------------------------------------------------------------*/
static void DumpOnArray(FILE* Target, PS_DPCRTLMM_BLOCKDESCARRAY CurrentArray)
{
  unsigned int j; /* Just so I don't get confused with the other function */

  for ( j = 0U; j < CurrentArray->Count; j++ ) {
    char defaultFilename[] = "(unknown)";
    char* filename;
    unsigned char flags = CurrentArray->Descriptors[j].Flags;

    if ( CurrentArray->Descriptors[j].SourceFile )
      filename = CurrentArray->Descriptors[j].SourceFile;
    else
      filename = defaultFilename;

    fprintf(Target, "Address: %p, owner: %s, line %u is %u bytes. ",
                    CurrentArray->Descriptors[j].PBase,
                    filename,
                    CurrentArray->Descriptors[j].SourceLine,
                    CurrentArray->Descriptors[j].Size
    );
    CrackAndPrintFlags(Target, flags);
  }
  return;
}
/*-------------------------------------------------------------------------*/
static void CrackAndPrintFlags(FILE* Target, unsigned char Flags)
{
  if ( Target ) {
    int comma = 0;
    fprintf(Target, "Flags=");
    if ( (Flags & 1) == 1 ) {
      fprintf(Target, "LOCKED");
      comma = 1;
    }
    if ( (Flags & 2) == 2 ) {
      if ( comma ) {
        /*comma = 0;*/
        fprintf(Target, ", ");
      }
      fprintf(Target, "NOSWAP");
      /*comma = 1;*/
    }
    fprintf(Target, "\n");
  }
  return;
}
/*-------------------------------------------------------------------------*/
