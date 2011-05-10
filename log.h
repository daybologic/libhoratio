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
#ifndef __INC_DPCRTLMM_LOG_H
#define __INC_DPCRTLMM_LOG_H
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/* The following function is intended for internal library use ONLY */
void dpcrtlmm_int_Log(const char* Message); /* Write the message to the log (or do nothing if the log macro is undefined */

#ifdef __cplusplus
} /* extern "C" */
#endif /*__cplusplus*/

/* This header should only be used internally but let's check anyway
before creating the lazy programmer's macro */
#ifdef __INC_DPCRTLMM_INTDATA_H /* Internal header included? */
#  ifdef DPCRTLMM_LOG /* Log supported in this build? */

     /* NOTE: The shortcut _Log is no longer available, use LOG macro */
     /* Definition of the LOG macro */
#    ifdef LOG
#      undef LOG /* Get rid of anybody else's version of LOG */
#    endif /*LOG*/
#    define LOG(msg) dpcrtlmm_int_Log(msg);

#  else /*!DPCRTLMM_LOG - Log is NOT supported in this build */

     /* People will still be using the macro LOG even though it's not
     supported.  Remove the call from the source code by making a dummy
     macro. */
#    define LOG(msg)

#  endif /*DPCRTLMM_LOG*/
#endif /*__INC_DPCRTLMM_INTDATA_H*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_LOG_H*/