# Daybo Logic C RTL Memory Manager
# Copyright (c) 2000-2014, David Duncan Ross Palmer, Daybo Logic
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
AUTOMAKE_OPTIONS=

TESTS = example1 example2 example3 newbrand dpctest1 htest1
check_PROGRAMS = $(TESTS)
test_LDADD = -lhoratio
cunit_test_LDADD = $(test_LDADD) -lcunit

if HORATIO_THREADS_PTH
AM_LDFLAGS= -lpth
endif


example1_SOURCES = example1.c
example1_INCLUDE = horatio.h
example1_LDADD = $(test_LDADD)

newbrand_SOURCES = newbrand.c
newbrand_AM_CFLAGS = horatio.h
newbrand_LDADD = $(test_LDADD)

example2_SOURCES = example2.cpp ../lib/dpccap.cpp
example2_AM_CPPFLAGS = horatio.h ../lib/dpccap.h
example2_LDADD = $(test_LDADD)

example3_SOURCES = example3.c
example3_AM_CFLAGS = horatio.h
example3_LDADD = $(test_LDADD)

dpctest1_SOURCES = dpctest1.c
dpctest1_INCLUDES = horatio.h
dpctest1_LDADD = $(cunit_test_LDADD)

htest1_SOURCES = htest1.c
htest1_INCLUDES = horatio.h
htest1_LDADD = $(cunit_test_LDADD)
