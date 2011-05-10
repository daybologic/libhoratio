/*
    DPCRTLMM configuration at build time file
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
/*
The build header is designed to localize settings for the build of the
library, everybody has permission to change the build header.  It's
intended to help the library build by keeping settings.  It's not intended
for inclusion in user programs.

When officially released the build header was written by Overlord David
Duncan Ross Palmer from Daybo Logic.  It is only modified by end users
of the library who have purchased the absolutely mental version of the
library. - 24th Feb 2000 : 10:04

When re-released as version 1.2 on GPL a lot of settings for different pay-
for version of the library were taken out.

Overlord@DayboLogic.co.uk


3rd Mar 2000 - Overlord Added DPCRTLMM_SAFETYLIST_MAXSIZE
13th May 2000 - Overlord removed hack SDYNMEN_WANT_IGNORANCE, doesn't apply
                since removal of dependancies to other libs (most notibly
                synmem of course). Sorry this doesn't mean anything to
                programmers outside of Daybo Logic. ignore.
14th June 2000 - Overlord Added DPCRTLMM_FARDATA to allow explicit far data
19th July 2000 - Overlord added a macro to disallow NULL as a special built-in
                 BLOCKDESCARRAY, the macro is called
                 DPCRTLMM_NONULL_BLOCKDESCARRAY, find the define in this file &
                 uncomment it to restore old behaviour.
21st July 2000 - Overlord added check for DPCRTLMM_SOURCE to try to stop users
		 making the mistake of including this in their programs.
25th July 2000 - Overlord added DPCRTLMM_STDBLOCKLIMIT for free versions
17th Nov 2000 - Overlord stripped all stuff to do with limiting and pricing
                and the like in preparation for GPL release.
*/

#ifndef __INC_DPCRTLMM_BUILD_H
#define __INC_DPCRTLMM_BUILD_H
/*-------------------------------------------------------------------------*/
/* Try to prevent users including this header */
#ifndef DPCRTLMM_SOURCE
#error this header is not intended for use outside of the library
#endif /*!DPCRTLMM_SOURCE*/


/* Build parameters may be changed here */

#ifndef NDEBUG
#  define DPCRTLMM_LOG
#endif /*!NDEBUG*/
/* comment out the above line to disable logging activity.
If you've defined NDEBUG which is a standard macro which means
no debugging, I automatically define it here, change it if you
want to.
When DPCRTLMM_LOG is defined a file called DPCRTLMM.LOG is created
otherwise, stderr is still used for errors and warnings but normal
messages such as allocation successful will not be displayed st all.
*/

#define DPCRTLMM_DEBUGHOOKS
/* Comment out the define line to disable advanced debug hook support */

/* Definition of MAX_TRAP_STRING_LENGTH (change if trap/log strings are
	getting too long*/

#ifdef MAX_TRAP_STRING_LENGTH /* Somebody else is using the macro we want? */
#  undef MAX_TRAP_STRING_LENGTH
#endif /*MAX_TRAP_STRING_LENGTH*/

#define MAX_TRAP_STRING_LENGTH (191) /* Maximum length of a trap string (excluding space for NULL terminator) */

/* Definition of DPCRTLMM_HOOKCHAIN_SIZE, hook chain sizes used to differ
depending on how much money was payed for the library, now they default to
32 */

#if DPCRTLMM_HOOKCHAIN_SIZE /* Somebody else is using the macro we want? */
#  undef DPCRTLMM_HOOKCHAIN_SIZE /* Get shot of that crap */
#endif /*DPCRTLMM_HOOKCHAIN_SIZE*/

#define DPCRTLMM_HOOKCHAIN_SIZE (32) /* Quite large hook chain sizes, change if you feel you need to go mental on me of need less to save memory */

/* Definition of DPCRTLMM_SAFETYLIST_MAXSIZE, the maximum number of modules
which may have their own array of blocks looked after byte this library is
determined by the maxmimum number of entries which may be stored in the
safety list.  The safety list is a fixed size structure in the data segment.
In this GPL release it's 512 default, allowing for a very large application,
but it may be changed manually here, remember changes require rebuilding
DPCRTLMM */

#if DPCRTLMM_SAFETYLIST_MAXSIZE /* Somebody else is using the macro we want? */
#  undef DPCRTLMM_SAFETYLIST_MAXSIZE /* Again, get shot of that crap, they were just trying to disrupt the library! */
#endif /*DPCRTLMM_SAFETYLIST_MAXSIZE*/

#define DPCRTLMM_SAFETYLIST_MAXSIZE (512) /* Programmers change this value if you run out of space */

/* Definition of DPCRTLMM_FARDATA, to allow explicit far data pointers which are
non-ANSI define DPCRTLMM_WANTFARDATA, for normal mode which means you will have
to ensure the memory modeal has far data comment the line below out (default)
*/
/* Un-comment for explicit far data!  (Ignored in flat memory model) */
/*#define DPCRTLMM_WANTFARDATA*/

/* Don't touch this part */
#ifndef __FLAT__ /* Segmented */
#  ifdef DPCRTLMM_WANTFARDATA
#    define DPCRTLMM_FARDATA __far /* Non-ANSI, avoid if possible */
#    define DPCRTLMM_MALLOC farmalloc
#    define DPCRTLMM_REALLOC farrealloc
#    define DPCRTLMM_CALLOC farcalloc
#    define DPCRTLMM_FREE farfree
#  else
#    define DPCRTLMM_FARDATA
#    define DPCRTLMM_MALLOC malloc
#    define DPCRTLMM_REALLOC realloc
#    define DPCRTLMM_CALLOC calloc
#    define DPCRTLMM_FREE free
#  endif /*DPCRTLMM_WANTFARDATA*/
#else /* Flat */
#  define DPCRTLMM_FARDATA
#  define DPCRTLMM_MALLOC malloc
#  define DPCRTLMM_REALLOC realloc
#  define DPCRTLMM_CALLOC calloc
#  define DPCRTLMM_FREE free
#endif /*!__FLAT__*/


/* Uncomment the next line to return to the old behaviour, no NULL as default
for block desc arrays, by default the next line is commented out because it's
a new feature and I wouldn't want to assume you don't like my new feature :)
*/
/*#define DPCRTLMM_NONULL_BLOCKDESCARRAY*/

/* Version information can be set here */
#define DPCRTLMM_VERSION_MAJOR (1)
#define DPCRTLMM_VERSION_MINOR (2)
/* Comment out the next line before releasing the library */
#define DPCRTLMM_VERSION_TEST
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_BUILD_H*/
