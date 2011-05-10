/**********************************************************************
 *                                                                    *
 * "DPCRTLMM" David Palmer's C-RTL Memory Manager Copyright (c) 2000  *
 * David Duncan Ross Palmer, Daybo Logic all rights reserved.         *
 * http://daybologic.com/Dev/dpcrtlmm                                 *
 *                                                                    *
 * D.D.R. Palmer's official homepage: http://daybologic.com/overlord  *
 * See the included license file for more information.                *
 *                                                                    *
 **********************************************************************
*/
#define DPCRTLMM_SOURCE
/*
  Functions for returning statistics about memory handled by the library
  Programmer: Overlord David Duncan Ross Palmer
  Contact: Overlord@DayboLogic.co.uk
  Created: 27th July 2000
  Last modified: 28th July 2000
  Library: DPCRTLMM 1.0
  Language: ANSI C (1990)
  Revision #1
*/

#include <stddef.h>
#include <string.h> /* memset() */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Public library header */
#include "intdata.h" /* Internal library data */

/* Local types other people don't need to know about */
typedef struct _S_FLAGCOUNTS
{
  unsigned long Locked, Unswappable; /* More will be added on the end */
} S_FLAGCOUNTS, *PS_FLAGCOUNTS;

static void CountFlagsInUse(PS_FLAGCOUNTS Pfc);
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
    S_FLAGCOUNTS flagCounts;
    memset(PReadStats, 0, sizeof(S_DPCRTLMM_STATS)); /* Init the stats structure */

    PReadStats->Blocks.Allocated = dpcrtlmm_GetBlockCount();
    CountFlagsInUse(&flagCounts); /* Loop through the entire load counting us flags */
    PReadStats->Blocks.Peak = _blockCountPeak;
    PReadStats->Charge.Allocated = _allocCharge;
    PReadStats->Charge.Peak = _allocPeak;
  }
}
/*-------------------------------------------------------------------------*/
static void CountFlagsInUse(PS_FLAGCOUNTS Pfc)
{
  if (Pfc)
  {
    unsigned int i;

    memset(Pfc, 0, sizeof(S_FLAGCOUNTS)); /* Init the structure */
    /* First go through normal arrays */
    for ( i = 0U; i < DPCRTLMM_SAFETYLIST_MAXSIZE; i++ )
    {
      if (_safetyList[i]) /* Used entry? */
      {
	unsigned int j;
	for ( j = 0U; j < _safetyList[i]->Count; j++ )
	{
	  unsigned char flags = _safetyList[i]->Descriptors[j].Flags;

	  if ( (flags & 1) == 1) /* Lock bit set */
	    Pfc->Locked++;
	  if ( (flags & 2) == 2) /* NoSwap bit set */
	    Pfc->Unswappable++;
	}
      }
    }
    /* Extra support for the "NULL array" */
    #ifndef DPCRTLMM_NONULL_BLOCKDESCARRAY /* Source purchasers can turn disable this support */
    for ( i = 0U; i < _defaultArray.Count; i++ )
    {
      unsigned char flags = _defaultArray.Descriptors[i].Flags;

      if ( (flags & 1) == 1) /* Lock bit set */
	Pfc->Locked++;
      if ( (flags & 2) == 2) /* NoSwap bit set */
	Pfc->Unswappable++;
    }
    #endif /*!DPCRTLMM_NONULL_BLOCKDESCARRAY*/
  }
}
/*-------------------------------------------------------------------------*/
