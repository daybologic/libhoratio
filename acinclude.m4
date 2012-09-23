# Daybo Logic C RTL Memory Manager
# Copyright (c) 2000-2012, David Duncan Ross Palmer, Daybo Logic
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

AC_DEFUN([AC_CHECK_FORMAT_POINTER_PREFIX],
[AC_CACHE_CHECK([printf %p prefixes 0x],
  ac_cv_format_pointer_prefix,
[
  if test "$ac_cv_header_stdio_h" = "yes"; then
    AC_RUN_IFELSE(
    [
#include <stdio.h>
static char addr[[32]];
int main() {
  sprintf(addr, "%p", addr);
  if ( strncmp(addr, "0x", 2) == 0 )
    return 0;
  return 1;
}
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
