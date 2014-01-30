# Daybo Logic C RTL Memory Manager
# Copyright (c) 2000-2013, David Duncan Ross Palmer, Daybo Logic
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

AUTOMAKE_OPTIONS=

#AM_CFLAGS= -g -Wall -W -Wcast-qual -Wcast-align -Waggregate-return -Wpointer-arith \
# -Wshadow -Wmissing-prototypes -Winline -Wstrict-prototypes \
# -Wredundant-decls -Wwrite-strings -Wformat=2 -Wformat-security \
# -Wformat-nonliteral -Wundef -Wredundant-decls -Wbad-function-cast \
# -Wnested-externs -Wmissing-declarations -Wunknown-pragmas

# Add any explicit flags here, or on the configure command line.
AM_CFLAGS=

TESTS = example1 example2 example3 newbrand
check_PROGRAMS = $(TESTS)
examples_LDADD = -lhoratio

if HORATIO_THREADS_PTH
AM_LDFLAGS= -lpth
endif

example1_SOURCES = example1.c
example1_INCLUDES = horatio.h
example1_LDADD = $(examples_LDADD)

newbrand_SOURCES = newbrand.c
newbrand_INCLUDES = horatio.h
newbrand_LDADD = $(examples_LDADD)

example2_SOURCES = example2.cpp dpccap.cpp
example2_INCLUDES = horatio.h dpccap.h
example2_LDADD = $(examples_LDADD)

example3_SOURCES = example3.c
example3_INCLUDES = horatio.h
example3_LDADD = $(examples_LDADD)

AM_CFLAGS = \
  -I.

lib_LTLIBRARIES = libhoratio.la
libhoratio_la_SOURCES  = horatio.c
libhoratio_la_SOURCES += halloc.c
libhoratio_la_SOURCES += hbdflags.c
libhoratio_la_SOURCES += hbiglock.c
libhoratio_la_SOURCES += hblocarr.c
libhoratio_la_SOURCES += hbloclck.c
libhoratio_la_SOURCES += hcalloc.c
libhoratio_la_SOURCES += hdbghook.c
libhoratio_la_SOURCES += hfree.c
libhoratio_la_SOURCES += hblocksz.c
libhoratio_la_SOURCES += hiblkptr.c
libhoratio_la_SOURCES += hintdata.c
libhoratio_la_SOURCES += hisbad.c
libhoratio_la_SOURCES += hlcktrap.c
libhoratio_la_SOURCES += hlog.c
libhoratio_la_SOURCES += hrealloc.c
libhoratio_la_SOURCES += hsafelst.c
libhoratio_la_SOURCES += hstats.c
libhoratio_la_SOURCES += htrap.c
libhoratio_la_SOURCES += hvptrap.c

nobase_include_HEADERS =  horatio.h
nobase_include_HEADERS += dpcrtlmm.h
