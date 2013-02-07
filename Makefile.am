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

TESTS = example1 example2 example3 tests1
check_PROGRAMS = $(TESTS)
examples_LDADD = -ldpcrtlmm
tests_LDADD = -ldpcrtlmm -lcunit

if DPCRTLMM_THREADS_PTH
AM_LDFLAGS= -lpth
endif

example1_SOURCES = example1.c
example1_INCLUDES = dpcrtlmm.h
example1_LDADD = $(examples_LDADD)

example2_SOURCES = example2.cpp dpccap.cpp
example2_INCLUDES = dpcrtlmm.h dpccap.h
example2_LDADD = $(examples_LDADD)

example3_SOURCES = example3.c
example3_INCLUDES = dpcrtlmm.h
example3_LDADD = $(examples_LDADD)

tests1_SOURCES = tests1.c
tests1_INCLUDES = dpcrtlmm.h
tests1_LDADD = $(tests_LDADD)

INCLUDES = \
  -I.

lib_LTLIBRARIES = libdpcrtlmm.la
libdpcrtlmm_la_SOURCES  = dpcrtlmm.c
libdpcrtlmm_la_SOURCES += dpc_alloc.c
libdpcrtlmm_la_SOURCES += dpc_bdflags.c
libdpcrtlmm_la_SOURCES += dpc_biglock.c
libdpcrtlmm_la_SOURCES += dpc_blkarray.c
libdpcrtlmm_la_SOURCES += dpc_bloclock.c
libdpcrtlmm_la_SOURCES += dpc_calloc.c
libdpcrtlmm_la_SOURCES += dpc_dbghooks.c
libdpcrtlmm_la_SOURCES += dpc_free.c
libdpcrtlmm_la_SOURCES += dpc_getblksz.c
libdpcrtlmm_la_SOURCES += dpc_iblkptr.c
libdpcrtlmm_la_SOURCES += dpc_intdata.c
libdpcrtlmm_la_SOURCES += dpc_isbad.c
libdpcrtlmm_la_SOURCES += dpc_locktrap.c
libdpcrtlmm_la_SOURCES += dpc_log.c
libdpcrtlmm_la_SOURCES += dpc_realloc.c
libdpcrtlmm_la_SOURCES += dpc_safelist.c
libdpcrtlmm_la_SOURCES += dpc_stats.c
libdpcrtlmm_la_SOURCES += dpc_trap.c
libdpcrtlmm_la_SOURCES += dpc_vptrap.c

nobase_include_HEADERS = dpcrtlmm.h

