/*
Horatio's Memory Manager
Copyright (c) 2000-2014, David Duncan Ross Palmer (M6KVM), Daybo Logic
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

/*! \file hbuild.h
 * \brief Manually tuned, internal configuration profile.
 *
 * The build header is used to add some more sense to the user options in
 * config.h.  Note: Don't directly edit hbuild.h or config.h, hbuild.h is
 * readonly and is included by the library, config.h is dynamically written
 * by the config program and included in this file.
 *
 *  3rd Mar 2000  - Added HORATIO_SAFETYLIST_MAXSIZE
 * 13th May 2000  - Removed hack SDYNMEN_WANT_IGNORANCE, doesn't apply
 *                  since removal of dependancies to other libs (most notibly
 *                  synmem of course). Sorry this doesn't mean anything to
 *                  programmers outside of Daybo Logic. ignore.
 * 14th June 2000 - Added HORATIO_FARDATA to allow explicit far data
 * 19th July 2000 - Added a macro to disallow NULL as a special built-in
 *                  BLOCKDESCARRAY, the macro is called
 *                  HORATIO_NONULL_BLOCKDESCARRAY, find the define in this file
 *                  and uncomment it to restore old behaviour.
 * 21st July 2000 - Added check for HORATIO_SOURCE to try to stop users
 *                  making the mistake of including this in their programs.
 * 25th July 2000 - Added HORATIO_STDBLOCKLIMIT for free versions
 *  17th Nov 2000 - Stripped all stuff to do with limiting and pricing
 *                  and the like in preparation for GPL release.
 *  24th May 2001 - Refined versioning system, 1.2 goes down to 1.1.4 due
 *                  to HORATIO_VERSION_PATCH.
 *  9th June 2001 - Took most options out and made them the responsibillty
 *                  of the config program and config.h
 */

#ifndef INC_HORATIO_BUILD_H
#define INC_HORATIO_BUILD_H
/*
 * Below follows a non-user configuration ...
 */

/* Try to prevent users including this header */
#ifndef HORATIO_SOURCE
# error this header is not intended for use outside of the library
#endif /*!HORATIO_SOURCE*/

#define HORATIO_LOG (1)

/*! \def MAX_TRAP_STRING_LENGTH
 * \brief Maximum length of a trap string
 *
 * Not user-configurable.
 * Excludes space for NULL terminator.
 * Change if trap/log strings are getting too long.
 */
#ifdef MAX_TRAP_STRING_LENGTH /* Somebody else is using the macro we want? */
# undef MAX_TRAP_STRING_LENGTH
#endif /*MAX_TRAP_STRING_LENGTH*/
#define MAX_TRAP_STRING_LENGTH (191)

/* Force internal calls to sprintf() over to snprintf() */
#ifdef HAVE_SNPRINTF
# ifdef sprintf
#  undef sprintf
# endif /*sprintf*/
# define sprintf snprintf
#endif /*HAVE_SNPRINTF*/

/*
 * Handling for C libraries which prefix %p addresses with 0x,
 * when others don't
 */
#ifdef HAVE_FORMAT_POINTER_PREFIX
# define HORATIO_FMTPTRPFX ""
#else
# define HORATIO_FMTPTRPFX "0x"
#endif /*HAVE_FORMAT_POINTER_PREFIX*/

/*
 * To allow explicit far data pointers which are
 * non-ANSI, configure with configure --enable-fardata
 */
#ifndef __FLAT__ /* Segmented */
#  ifdef HORATIO_WANTFARDATA
#    define HORATIO_FARDATA far /* Non-ANSI, avoid if possible */
#    define HORATIO_MALLOC farmalloc
#    define HORATIO_REALLOC farrealloc
#    define HORATIO_CALLOC farcalloc
#    define HORATIO_FREE farfree
#  else
#    define HORATIO_FARDATA
#    define HORATIO_MALLOC malloc
#    define HORATIO_REALLOC realloc
#    define HORATIO_CALLOC calloc
#    define HORATIO_FREE free
#  endif /*HORATIO_WANTFARDATA*/
#else /* Flat */
#  define HORATIO_FARDATA
#  define HORATIO_MALLOC malloc
#  define HORATIO_REALLOC realloc
#  define HORATIO_CALLOC calloc
#  define HORATIO_FREE free
#endif /*!__FLAT__*/

#endif /*!INC_HORATIO_BUILD_H*/
