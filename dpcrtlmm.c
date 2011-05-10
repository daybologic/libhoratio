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
/* Incase you're wondering why DPCRTLMM_SOURCE appears at the top of all my
source, it's to do with build.h.  That header is for the compilation fo the
library only, not to be included in user programs.  If users include build.h
the definition won't exist and build.h will tell them off! */
#define DPCRTLMM_SOURCE
/* NOTE on internal allocations:  Internal allocations for processing
reasons should be allocated using xmalloc() so that the program is ended
if a memory allocation problem happens.  Allocation of behalf of the caller
should be handled by malloc() so that the caller sees the problem. */

#include <stdlib.h> /* For normal C memory functions */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h"
/*-------------------------------------------------------------------------*/

/*
Minimal stuff here please, if possible promote granularity by using other C files
*/

#pragma warn -use /* Don't hastle me about unused identifiers */

unsigned char dpcrtlmm__EnableTraps = 1U;
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD)
static const char CopyrightString[] =
	"Overlord David Duncan Ross Palmer\'s C-RTL Memory Manager Library Release Version 1.0 (Freeware version, not intended for commercial use, see license)"
	"   Copyright 2000 Daybo Logic, all rights reserved.";
#endif
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_PRO)
static const char CopyrightString[] =
	"Overlord David Duncan Ross Palmer\'s C-RTL Professional Memory Manager Library Release Version 1.0"
	"   Copyright 2000 Daybo Logic, all rights reserved.";
#endif
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_MENTAL)
static const char CopyrightString[] =
	"Overlord David Duncan Ross Palmer\'s C-RTL Professional Memory Manager Library Release Version 1.0 (absolutely mental enterprise edition)"
	"   Copyright 2000 Daybo Logic, all rights reserved.";
#endif

static const char URLString[] = "http://www.daybologic.com";
/*-------------------------------------------------------------------------*/
PS_DPCRTLMM_VERSION dpcrtlmm_Ver(PS_DPCRTLMM_VERSION PVerStruct)
{
  if (PVerStruct)
  {
    /* Load version information into the caller's structure */
    PVerStruct->Major = DPCRTLMM_VERSION_MAJOR;
    PVerStruct->Minor = DPCRTLMM_VERSION_MINOR;
    PVerStruct->Flags = (unsigned char)0U;
    #ifdef DPCRTLMM_VERSION_TEST /* Test build? */
    PVerStruct->Flags |= 1;
    #endif /*DPCRTLMM_VERSION_TEST*/
  }
  return NULL;
}
/*-------------------------------------------------------------------------*/
