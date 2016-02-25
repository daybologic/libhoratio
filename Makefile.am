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

ACLOCAL_AMFLAGS=-I m4
AUTOMAKE_OPTIONS=subdir-objects

C_ONLY_WFLAGS = \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wbad-function-cast \
	-Wmissing-declarations \
	-Wnested-externs

GENERAL_WFLAGS = \
	-Wall \
	-W \
	-Wcast-qual \
	-Wcast-align \
	-Waggregate-return \
	-Wpointer-arith \
	-Wshadow -Winline \
	-Wredundant-decls \
	-Wwrite-strings \
	-Wformat=2 \
	-Wformat-security \
	-Wformat-nonliteral \
	-Wundef \
	-Wredundant-decls \
	-Wunknown-pragmas

GENERAL_CC_FLAGS = \
	-g \
	-I$(top_srcdir)/../include \
	-I/usr/include/mongo-client \
	-I/usr/include/glib-2.0 \
	-I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
	-I/usr/lib/i386-linux-gnu/glib-2.0/include \
	-I/usr/local/mysql/include

AM_CFLAGS = \
	$(GENERAL_CC_FLAGS) \
	$(GENERAL_WFLAGS) \
	$(C_ONLY_WFLAGS)

AM_CPPFLAGS = \
	$(GENERAL_CC_FLAGS) \
	$(GENERAL_WFLAGS)

AM_LDFLAGS = \
	-L$(top_srcdir)/../lib \
	-L/usr/local/lib \
	-L/usr/local/mysql/lib

export AM_CPPFLAGS
export AM_CFLAGS
export AM_LDFLAGS

SUBDIRS = lib include t
if HORATIO_DOCS
SUBDIRS += docs
endif #HORATIO_DOCS
