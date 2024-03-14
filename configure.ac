#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Horatio's Memory Manager
# Copyright (c) 2000-2014, David Duncan Ross Palmer (M6KVM), Daybo Logic
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of the Daybo Logic nor the names of its contributors
#      may be used to endorse or promote products derived from this software
#      without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

AC_PREREQ(2.59)
AC_INIT([horatio],[2.0.2])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([lib/horatio.c])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for architecture/machine characteristics
AC_C_BIGENDIAN(
  AC_DEFINE([__B_ENDIAN__], [], [Big endian byte order]),
  AC_DEFINE([__L_ENDIAN__], [], [Little endian byte order])
)


# Checks for libraries.
# PThread checks
AC_CHECK_LIB(pthread, pthread_mutex_init, [pthread_mutex_init="yes"])
AC_CHECK_LIB(pthread, pthread_mutexattr_setkind_np, [pthread_mutexattr_setkind_np="yes"])
# PTH checks
AC_CHECK_LIB(pth, pth_init, [pth_init="yes"])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stddef.h stdlib.h string.h pth.h pthread.h windows.h])
AC_CHECK_HEADERS([alloc.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Custom function definitions
#----------------------------------------------------------------------------
AC_DEFUN([AC_CHECK_FORMAT_POINTER_PREFIX],
[AC_CACHE_CHECK([printf %p prefixes 0x],
  ac_cv_format_pointer_prefix,
[
  if test "$ac_cv_header_stdio_h" = "yes"; then
    AC_RUN_IFELSE(
    [
        AC_LANG_SOURCE(
            [
#include <stdio.h>
static char addr[[32]];
int main() {
  sprintf(addr, "%p", addr);
  if ( strncmp(addr, "0x", 2) == 0 )
    return 0;
  return 1;
}
            ]
        )
    ],[
      ac_cv_format_pointer_prefix=yes
    ],[
      ac_cv_format_pointer_prefix=no
    ],[
      dnl Where we don't know, assume yes, and don't modify address strings
      ac_cv_format_pointer_prefix=yes
    ])
  else
    ac_cv_format_pointer_prefix=no
  fi
])

if test "$ac_cv_format_pointer_prefix" = "yes"; then
  AC_DEFINE(HAVE_FORMAT_POINTER_PREFIX, 1, [printf prefixes %p 0x])
fi
dnl AC_MSG_RESULT($ac_cv_format_pointer_prefix)
])
#----------------------------------------------------------------------------
# Checks for standard library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset snprintf])

# Extra configuration
dnl pthread non-portable support
if test "$pthread_mutexattr_setkind_np" = "yes"; then
  AC_DEFINE([HORATIO_THREADS_PTHREAD_NP], [], [Non-portable POSIX thread extensions])
fi

dnl Test whether %p prefixes addresses with 0x
AC_CHECK_FORMAT_POINTER_PREFIX

# Optional arguments

dnl --enable-debug
AC_MSG_CHECKING(whether to build with extra debug messages)
AC_ARG_ENABLE(debug,
  [  --enable-debug          Enable debug messages],
  [debug_enabled=yes], [debug_enabled=no])
if test "$debug_enabled" = "no"; then
  AC_DEFINE([NDEBUG], [], [Disable debug messages])
else
  AC_DEFINE([DEBUG], [], [Enable debug messages])
fi
AC_MSG_RESULT($debug_enabled)

dnl --with-win32
AC_MSG_CHECKING(whether building for Win32)
AC_ARG_ENABLE(win32,
  [  --enable-win32          Enable Win32 code],
  [win32_enabled=yes], [win32_enabled=no])
if test "$win32_enabled" = "yes"; then
  AC_DEFINE([__WIN32__], [], [Enable Win32 code])
  real_thread_lib=nt
else
  AC_DEFINE([__UNIX__], [], [Enable UNIX code])
  real_thread_lib=pthread
fi
AC_MSG_RESULT($win32_enabled)

dnl --with-threads
AC_MSG_CHECKING([whether to enable thread support])
AC_ARG_ENABLE(
  [threads],
  [  --enable-threads        Enable thread support],
  [threads_enabled=yes],
  [threads_enabled=no]
)
if test "$threads_enabled" = "yes"; then
  AC_DEFINE([HORATIO_THREADS], [], [Thread support])
fi
AC_MSG_RESULT($threads_enabled)

AC_MSG_CHECKING([whether to use pragma hdrstop])
AC_ARG_ENABLE(
  [hdrstop],
  [  --enable-hdrstop        Enable pragma hdrstop],
  [hdrstop_enabled=yes],
  [hdrstop_enabled=no]
)
if test "$hdrstop_enabled" = "yes"; then
  AC_DEFINE([HORATIO_HDRSTOP], [], [Use pragma hdrstop])
fi
AC_MSG_RESULT($hdrstop_enabled)

dnl --with-gnu-pth
AC_MSG_CHECKING([which back-end threadlib to link against])
AC_ARG_WITH(
  [gnu-pth],
  [  --with-gnu-pth          Use GNU Portable Threads],
  [real_thread_lib=pth],
  [real_thread_lib="$real_thread_lib"]
)
AM_CONDITIONAL([HORATIO_THREADS_PTH], [test "$pth_init" = "yes"])
if test "$real_thread_lib" = "pth"; then
  if test "$threads_enabled" = "yes"; then
    AC_DEFINE([HORATIO_THREADS_PTH], [], [Use code for GNU Portable Threads])
  fi
  if test "$ac_cv_header_pth_h" != yes; then
    echo "ERROR: Specified --with-gnu-pth but pth.h was not found";
    exit 1
  fi
fi
if test "$real_thread_lib" = "pthread"; then
  if test "$threads_enabled" = "yes"; then
    if test "$ac_cv_header_pthread_h" != yes; then
      echo "ERROR: Specified --enable-threads but pthread.h was not found";
      echo "Did you want to use Win32's critical section objects?  If so,";
      echo "you must use --enable-win32 --enable-threads\n";
      exit 1
    fi
    AC_DEFINE([HORATIO_THREADS_PTHREAD], [], [Use code for POSIX threads])
  fi
fi
if test "$real_thread_lib" = "nt"; then
  if test "$threads_enabled" = "yes"; then
    AC_DEFINE([HORATIO_THREADS_NT], [], [Use code for NT threads])
  fi
fi
if test "$threads_enabled" = "no"; then
  real_thread_lib=none
fi
AC_MSG_RESULT($real_thread_lib)

dnl Display a warning if GNU PTH and Win32 mode have been enabled because at
dnl time of writing (20060221) GNU Portable Threads does not support being
dnl built natively for Win32.

if test "$real_thread_lib" = "pth"; then
  if test "$win32_enabled" = "yes"; then
    AC_MSG_WARN([GNU PTH is not known to be supported on Win32])
  fi
fi

dnl --with-safety-list-size=N
safety_list_size=512
AC_MSG_CHECKING([number of entries in safety list])
AC_ARG_WITH(
  safety-list-size,
  [  --with-safety-list-size=N
                          Maximum entries in the safety list (default 512)],
  [case $withval in
    [[0-9]]*)
    safety_list_size=$withval
    
    ;;
  *)
    echo "ERROR: Invalid --with-safety-list-size argument"
    exit 1
    ;;
  esac
])
AC_DEFINE_UNQUOTED([HORATIO_SAFETYLIST_MAXSIZE], $safety_list_size, [Number of entries in safety list])
AC_MSG_RESULT($safety_list_size)

dnl --with-hook-chain-size=N
hook_chain_size=32
AC_MSG_CHECKING([number of entries in debug hook chain])
AC_ARG_WITH(
  hook-chain-size,
  [  --with-hook-chain-size=N
                          Maximum number of entries in a hook chain (default 32)],
  [case $withval in
    [[0-9]]*)
    hook_chain_size=$withval
    
    ;;
  *)
    echo "ERROR: Invalid --with-hook-chain-size argument"
    exit 1
    ;;
  esac
])
hook_chains_enabled=no
if test "$hook_chain_size" != "0"; then
  AC_DEFINE([HORATIO_DEBUGHOOKS], [], [Use debug hooks])
  hook_chains_enabled=yes
fi
AC_DEFINE_UNQUOTED([HORATIO_HOOKCHAIN_SIZE], $hook_chain_size, [Number of entries in a debug hook chain])
AC_MSG_RESULT($hook_chain_size)
AC_MSG_CHECKING([whether to enable debug hook chains])
AC_MSG_RESULT($hook_chains_enabled)

dnl --enable-fardata
AC_MSG_CHECKING(whether you want far data)
AC_ARG_ENABLE(
  fardata,
  [  --enable-fardata        Enable far pointers for data (16-bit only)],
  [fardata_enabled=yes], [fardata_enabled=no])
if test "$fardata_enabled" = "yes"; then
  AC_DEFINE([HORATIO_WANTFARDATA], [], [Enable far data (16-bit)])
fi
AC_MSG_RESULT($fardata_enabled)

dnl --with-no-null-array
AC_MSG_CHECKING(whether you want the NULL array disabled)
AC_ARG_WITH(no-null-array,
  [  --with-no-null-array    Disable special NULL array for backward compat],
  [null_array_disabled=yes], [null_array_disabled=no])
if test "$null_array_disabled" = "yes"; then
  AC_DEFINE([HORATIO_NONULL_BLOCKDESCARRAY], [], [Disable special NULL array])
fi
AC_MSG_RESULT($null_array_disabled)

dnl --with-sqlite
AC_MSG_CHECKING([whether you want SQlite, my friend])
AC_ARG_WITH(sqlite,
  [  --with-sqlite           Link with SQlite],
  [sqlite=yes], [sqlite=no]
)
if test "$sqlite" = "yes"; then
  AC_DEFINE([USE_SQLITE], [], [Link with SQlite])
fi
AM_CONDITIONAL([USE_SQLITE], [test "$sqlite" = "yes"])
AC_MSG_RESULT($sqlite)

dnl --with-mysql
AC_MSG_CHECKING([whether you want MySQL, my friend])
AC_ARG_WITH(mysql,
  [  --with-mysql            Link with MySQL],
  [mysql=yes], [mysql=no]
)
if test "$mysql" = "yes"; then
  AC_DEFINE([USE_MYSQL], [], [Link with MySQL])
fi
AM_CONDITIONAL([USE_MYSQL], [test "$mysql" = "yes"])
AC_MSG_RESULT($mysql)

dnl --with-mongo
AC_MSG_CHECKING([whether you want MongoDB, my friend])
AC_ARG_WITH(mongo,
  [  --with-mongo            Link with MongoDB],
  [mongo=yes], [mongo=no]
)
if test "$mongo" = "yes"; then
  AC_DEFINE([USE_MONGO], [], [Link with MongoDB])
fi
AM_CONDITIONAL([USE_MONGO], [test "$mongo" = "yes"])
AC_MSG_RESULT($mongo)

dnl --enable-docs
AC_MSG_CHECKING([Whether to build documentation])
AC_ARG_ENABLE(
  docs,
  [  --enable-docs          Build the documentation],
  [docs_enabled=yes], [docs_enabled=no]
)
if test "$docs_enabled" = "yes"; then
  AC_DEFINE([HORATIO_DOCS], [], [Build the documentation])
fi
AM_CONDITIONAL([HORATIO_DOCS], [test "$docs_enabled" = "yes"])
AC_MSG_RESULT($docs_enabled)

# End of configuration
AC_OUTPUT(Makefile lib/Makefile include/Makefile t/Makefile)

