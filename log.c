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
#############################################################################
# Memory usage logging support for DPCRTLMM                                 #
# Only included if DPCRTLMM_LOG is defined in build.h                       #
#                                                                           #
# DPCRTLMM by Overlord David Duncan Ross Palmer,                            #
# (C) Copyright 2000, Daybo Logic, all rights reserved.                     #
#############################################################################

You might be wondering why I did not just get rid of the whole function if
DPCRTLMM_LOG was not defined.  Trouble is, even though it's never called if
the macro is not defined, in some compilers there must be at least one
external definition so I chose to get rid of the contents and disable the
warning about the unused parameter, if getting rid of the warning actually
causes a warning on your compiler, I applogise!
*/

#include <stdio.h> /* FILE */
#ifdef DPCRTLMM_HDRSTOP
#  pragma hdrstop
#endif /*DPCRTLMM_HDRSTOP*/

#include "build.h" /* General build parameters */
#include "dpcrtlmm.h" /* Main library header */
#include "intdata.h" /* Internal library header */
#include "log.h"
/*-------------------------------------------------------------------------*/
void dpcrtlmm_int_Log(const char* Message)
{
  #ifdef DPCRTLMM_LOG

  FILE* HLogFile; /* Handle for log file */

  if (Message)
  {
    if (Message[0])
    {
      HLogFile = fopen("DPCRTLMM.LOG", "at"); /* Append/overwrite text file */
      if (HLogFile) /* Log opened? */
      {
        fputs("MM message: \"", HLogFile); /* Prefix and start quotes */
        fputs(Message, HLogFile); /* Output log msg to log file */
        fputc('\"', HLogFile); /* End quotes */
        fputc('\n', HLogFile); /* End line */
        fclose(HLogFile); /* Close the log file */
      }
    }
  }

  #else /* No logging */

  #pragma warn -par /* Don't warn about unused parameter 'Message' */

  #endif
}