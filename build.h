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
/*
The build header is designed to localize settings for the build of the
library, everybody has permission to change the build header.  The build
header is only available in absolutely mental versions of the library
because it is not intended to be included in the program which uses the
library.  It is only used to build the library.

When officially released the build header was written by Overlord David
Duncan Ross Palmer from Daybo Logic.  It is only modified by end users
of the library who have purchased the absolutely mental version of the
library. - 24th Feb 2000 : 10:04

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
*/

#ifndef __INC_DPCRTLMM_BUILD_H
#define __INC_DPCRTLMM_BUILD_H
/*-------------------------------------------------------------------------*/
/* Try to prevent users including this header */
#ifndef DPCRTLMM_SOURCE
#error this header is not intended for use outside of the library
#endif /*!DPCRTLMM_SOURCE*/


/* Build parameters may be changed here */

/* These are release constants, don't change these, well it wouldn't cause
any problems as such (as lnog as none matched) */
#define DPCRTLMM_RELEASETYPE_STD (0) /* Standard release (boring) */
#define DPCRTLMM_RELEASETYPE_PRO (1) /* Professional version with logging and advanced debug hook support (smart) */
#define DPCRTLMM_RELEASETYPE_MENTAL (2) /* Mental version everything in professional version + full debug builds and source code */

/* This is the release type, change this to alter copyright string,
it also disables things which should not be released on that flavour
of the lib */
#define DPCRTLMM_RELEASETYPE ( DPCRTLMM_RELEASETYPE_MENTAL )

#ifndef NDEBUG
#  define DPCRTLMM_LOG
#endif /*!NDEBUG*/
/* comment out the above line to disable logging activity,
I know it's also possible with a function, but it won't remove the
string and code overhead, if you have the source, commenting out
this is preferable if you will be releasing a program.
If you've defined NDEBUG which is a standard macro which means
no debugging, I automatically define it here, change it if you
want to. */

/*------ THIS STUFF IS FOR PROFESSIONAL AND ABSOLUTELY MENTAL VERSIONS ----*/
#if (DPCRTLMM_RELEASETYPE > DPCRTLMM_RELEASETYPE_STD) /* Better than standard version */

#  define DPCRTLMM_DEBUGHOOKS
  /* Comment out the define line to disable advanced debug hook support */
#endif /* End block for better than standard versions only */
/*-------------------------------------------------------------------------*/

/* Definition of MAX_TRAP_STRING_LENGTH (change if trap/log strings are
	getting too long*/

#ifdef MAX_TRAP_STRING_LENGTH /* Somebody else is using the macro we want? */
#  undef MAX_TRAP_STRING_LENGTH
#endif /*MAX_TRAP_STRING_LENGTH*/

#define MAX_TRAP_STRING_LENGTH (191) /* Maximum length of a trap string (excluding space for NULL terminator) */

/* Definition of DPCRTLMM_HOOKCHAIN_SIZE, hook chain sizes differ between
flavors of the library, standard does not support debug hooks, professional
supports hook chains containing 16 hooks per type.  Absolutely mental version
defaults to 32 but programmer can change the source code and rebuild the
library because all source code is supplied. */

#if DPCRTLMM_HOOKCHAIN_SIZE /* Somebody else is using the macro we want? */
#  undef DPCRTLMM_HOOKCHAIN_SIZE /* Get shot of that crap */
#endif /*DPCRTLMM_HOOKCHAIN_SIZE*/

#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD) /* Standard boring version */
#  define DPCRTLMM_HOOKCHAIN_SIZE (0) /* Debug hooks are not supported */
#endif
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_PRO) /* Professional version */
#  define DPCRTLMM_HOOKCHAIN_SIZE (16) /* Limited debug hook support */
#endif
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_MENTAL) /* Absolutely mental version */
#  define DPCRTLMM_HOOKCHAIN_SIZE (32) /* Quite large hook chain sizes, can be altered because users have source */
#endif

/* Definition of DPCRTLMM_SAFETYLIST_MAXSIZE, the maximum number of modules
which may have their own array of blocks looked after byte this library is
determined by the maxmimum number of entries which may be stored in the
safety list.  The safety list is a fixed size structure in the data segment.
The standard flavor of the library allows 64 entries (enough for 64 different
sections or modules of the program to have different arrays for their blocks.
The professional version has enough space for 512 which allows a much larger
program.  The absolutely mental version is unlimited but 512 by default, it is
unlimited by virtue of the number being able to be changed below because they
would have the source code: */

#if DPCRTLMM_SAFETYLIST_MAXSIZE /* Somebody else is using the macro we want? */
#  undef DPCRTLMM_SAFETYLIST_MAXSIZE /* Again, get shot of that crap, they were just trying to disrupt the library! */
#endif /*DPCRTLMM_SAFETYLIST_MAXSIZE*/

#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD) /* Bog-standard boring version? */
#  define DPCRTLMM_SAFETYLIST_MAXSIZE (64) /* A good size for a free project I should hope */
#endif
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_PRO) /* Professional version */
#  define DPCRTLMM_SAFETYLIST_MAXSIZE (512) /* A very large size I'd say! */
#endif
#if (DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_MENTAL) /* Absolutely metal version */
#  define DPCRTLMM_SAFETYLIST_MAXSIZE (512) /* Programmers change this value if you run out of space */
#endif

/* In standard builds I limit the number of blocks which may be allocated
per program to 50 by default, that can be changed here, it has no effect
in professional or absolutely mental builds of the library even if one
removes the ifdef */
#if DPCRTLMM_RELEASETYPE == DPCRTLMM_RELEASETYPE_STD
#  define DPCRTLMM_STDBLOCKLIMIT (50)
#endif /*DPCRTLMM_RELEASETYPE_STD*/

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
#define DPCRTLMM_VERSION_MINOR (1)
/* Comment out the next line before releasing the library */
/*#define DPCRTLMM_VERSION_TEST*/
/*-------------------------------------------------------------------------*/
#endif /*!__INC_DPCRTLMM_BUILD_H*/
