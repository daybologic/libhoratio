/*
Horatio's Memory Manager
Copyright (c) 2000-2013, David Duncan Ross Palmer (M6KVM), Daybo Logic
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
  Functions for returning statistics about memory handled by the library
  Programmer: David Duncan Ross Palmer
  Contact: http://www.daybologic.co.uk/mailddrp/
  Created: 27th July 2000
  Library: HORATIO 1.0
  Language: ANSI C (1990)
  Revision #5
*/

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>

#ifdef HORATIO_HDRSTOP
#  pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "dpc_build.h" /* General build parameters */
#include "horatio.h" /* Public library header */
#include "dpc_intdata.h" /* Internal library data */
#include "dpc_biglock.h" /* Mutual exclusion */

static void CountFlagsInUse(
  PS_HORATIO_STATS PFlagsStats
);
static void DumpOnArray(
  FILE *Target,
  PS_HORATIO_BLOCKDESCARRAY CurrentArray
);
static void CrackAndPrintFlags(
  FILE *Target,
  unsigned char Flags
);

unsigned long horatio_GetBlockCount() {
  unsigned long ret;

  LOCK
  ret = _blockCount;
  UNLOCK

  return ret;
}

void horatio_GetStats(
  PS_HORATIO_STATS PReadStats
) {
  LOCK
  if (PReadStats) {
    PReadStats->Blocks.Allocated = _blockCount;
    /* Loop through the entire load counting us flags */
    CountFlagsInUse(PReadStats);
    PReadStats->Blocks.Peak = _blockCountPeak;
    PReadStats->Charge.Allocated = _allocCharge;
    PReadStats->Charge.Peak = _allocPeak;
  }
  UNLOCK
}

static void CountFlagsInUse(
  PS_HORATIO_STATS PFlagsStats
) {
  if (PFlagsStats) {
    unsigned int i;

    /* First reset the counts in the stats struct */
    PFlagsStats->Blocks.Locked = 0UL;
    PFlagsStats->Blocks.Unswappable = 0UL;

    /* Go through normal arrays */
    for ( i = 0U; i < HORATIO_SAFETYLIST_MAXSIZE; i++ ) {
      if (_safetyList[i]) { /* Used entry? */
        unsigned int j;
        for ( j = 0U; j < _safetyList[i]->Count; j++ ) {
          unsigned char flags = _safetyList[i]->Descriptors[j].Flags;

          if ( (flags & 1) == 1) /* Lock bit set */
            PFlagsStats->Blocks.Locked++;
          if ( (flags & 2) == 2) /* NoSwap bit set */
            PFlagsStats->Blocks.Unswappable++;
        }
      }
    }
    /* Extra support for the "NULL array" */
    #ifndef HORATIO_NONULL_BLOCKDESCARRAY
    for ( i = 0U; i < _defaultArray.Count; i++ ) {
      unsigned char flags = _defaultArray.Descriptors[i].Flags;

      if ( (flags & 1) == 1) /* Lock bit set */
        PFlagsStats->Blocks.Locked++;
      if ( (flags & 2) == 2) /* NoSwap bit set */
        PFlagsStats->Blocks.Unswappable++;
    }
    #endif /*!HORATIO_NONULL_BLOCKDESCARRAY*/
  }
}

void horatio_Dump(
  FILE *Target
) {
  LOCK
  if ( Target ) {
    unsigned int i;

    for ( i = 0U; i < HORATIO_SAFETYLIST_MAXSIZE; i++ ) {
      if ( _safetyList[i] ) /* Used entry? */
        DumpOnArray(Target, _safetyList[i]);
    }
    #ifndef HORATIO_NONULL_BLOCKDESCARRAY
    DumpOnArray(Target, &_defaultArray);
    #endif
  }
  UNLOCK
  return;
}

static void DumpOnArray(
  FILE *Target,
  PS_HORATIO_BLOCKDESCARRAY CurrentArray
) {
  unsigned int j; /* Just so I don't get confused with the other function */

  for ( j = 0U; j < CurrentArray->Count; j++ ) {
    char defaultFilename[] = "(unknown)";
    char* filename;
    unsigned char flags = CurrentArray->Descriptors[j].Flags;

    if ( CurrentArray->Descriptors[j].SourceFile )
      filename = CurrentArray->Descriptors[j].SourceFile;
    else
      filename = defaultFilename;

    fprintf(
      Target,
      "Address: %s%p, (Array: %s%p), owner: %s, line %u is %u bytes. ",
      HORATIO_FMTPTRPFX, CurrentArray->Descriptors[j].PBase,
      HORATIO_FMTPTRPFX, (void*)CurrentArray,
      filename,
      CurrentArray->Descriptors[j].SourceLine,
      (unsigned int)CurrentArray->Descriptors[j].Size
    );
    CrackAndPrintFlags(Target, flags);
  }
  return;
}

static void CrackAndPrintFlags(
  FILE *Target,
  unsigned char Flags
) {
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

