# Make file for DPCRTLMM
# Written by Overlord David Duncan Ross Palmer
# Overlord@DayboLogic.co.uk
# 10th June 2001 - 11th January 2002

# This Makefile designed for GNU tools, gcc GNU C compiler, GNU's make
# not neccersarily on UNIX but that's what I'm building on.  If you have
# Borland C++ please use Makefile.bor instead.

# Paths & filenames - Name of the makefile can be overridden on the command line in the form
# make THISFILE=newmakefilename
#
# Normal make creates the library but not the examples
# make example creates all example programs
# make example1 builds the C example program only
# make example2 builds the C++ example program only 
# make example3 builds the C-stress test program only
# make clean deletes all files created by the build
# make clobber is supported for backwards compatibillty only
# make test builds all and then clobbers, it's for me to test the build before
#      distribution only.
# make all creates library and example programs
# make config creates the configuration program

# Tools
LIBRARY=ar
LIBOPTS=cru
# -ansi : Only accept ANSI C code
# -pedantic : Be very fussy about ANSI related problems
# -c : Compile with no link
# -g : Debug info
# -Wall : Every possible warning please
ANSI=-ansi -pedantic -Wall
COMPILE=gcc -g -c $(ANSI)
CPP_COMPILE=g++ -g -c $(ANSI)
LINK=gcc -g $(ANSI)
LINK_CPP=g++ -g $(ANSI)
RANLIB=ranlib $(LIBNAME)

# File control commands
ERASE=rm -f -v
COPY=cp -v

# File names and extensions
OBJ=.o
C=.c
H=.h
CPP=.cpp

# Main data for make rules
THISFILE=Makefile.gnu
LIBTITLE=dpcrtlmm
LIBNAME=lib$(LIBTITLE).a
# Master dependancies ALWAYS cause a rebuild
MASTERDEP=dpc_build$(H) dpcrtlmm$(H) $(THISFILE) config$(H) dpc_biglock$(H)
OBJECTS=dpc_alloc$(OBJ) dpc_blkarray$(OBJ) dpc_calloc$(OBJ) dpc_free$(OBJ) dpc_isbad$(OBJ) dpc_stats$(OBJ) dpc_dbghooks$(OBJ) dpc_locktrap$(OBJ) dpc_safelist$(OBJ) dpcrtlmm$(OBJ) dpc_log$(OBJ) dpc_vptrap$(OBJ) dpc_trap$(OBJ) dpc_realloc$(OBJ) dpc_intdata$(OBJ) dpc_iblkptr$(OBJ) dpc_getblksz$(OBJ) dpc_bloclock$(OBJ) dpc_bdflags$(OBJ) dpc_biglock$(OBJ)
LOGFILE=DPCRTLMM.LOG
COREDUMPS=example1.core example2.core example3.core core
BACKUPS=*~
LIBADD=$(LIBRARY) $(LIBOPTS) $(LIBNAME)

# Build rules

$(LIBNAME) : $(MASTERDEP) $(OBJECTS)
	$(LIBADD) $(OBJECTS)
	$(RANLIB)

dpc_alloc$(OBJ) : dpc_alloc$(C) $(MASTERDEP) dpc_intdata$(H) dpc_log$(H) dpc_vptrap$(H) dpc_dbghooks$(H) dpc_alloc$(H)
	$(COMPILE) dpc_alloc$(C)

dpc_blkarray$(OBJ) : dpc_blkarray$(C) $(MASTERDEP) dpc_intdata$(H) dpc_log$(H) dpc_trap$(H) dpc_safelist$(H) dpc_dbghooks$(H) dpc_biglock$(H) dpc_blkarray$(H)
	$(COMPILE) dpc_blkarray$(C)

dpc_calloc$(OBJ) : dpc_calloc$(C) dpc_alloc$(H) $(MASTERDEP) dpc_intdata$(H) dpc_log$(H) dpc_iblkptr$(H) dpc_dbghooks$(H)
	$(COMPILE) dpc_calloc$(C)

dpc_free$(OBJ) : dpc_free$(C) $(MASTERDEP) dpc_intdata$(H) dpc_vptrap$(H) dpc_locktrap$(H) dpc_log$(H) dpc_trap$(H) dpc_dbghooks$(H)
	$(COMPILE) dpc_free$(C)

dpc_isbad$(OBJ) : dpc_isbad$(C) $(MASTERDEP) dpc_intdata$(H) dpc_trap$(H) dpc_safelist$(H)
	$(COMPILE) dpc_isbad$(C)

dpc_stats$(OBJ) : dpc_stats$(C) $(MASTERDEP) dpc_intdata$(H)
	$(COMPILE) dpc_stats$(C)

dpc_dbghooks$(OBJ) : dpc_dbghooks$(C) $(MASTERDEP) dpc_intdata$(H) dpc_log$(H) dpc_dbghooks$(H)
	$(COMPILE) dpc_dbghooks$(C)

dpc_locktrap$(OBJ) : dpc_locktrap$(C) $(MASTERDEP) dpc_intdata$(H) dpc_trap$(H) dpc_locktrap$(H)
	$(COMPILE) dpc_locktrap$(C)

dpc_safelist$(OBJ) : dpc_safelist$(C) $(MASTERDEP) dpc_intdata$(H) dpc_safelist$(H)
	$(COMPILE) dpc_safelist$(C)

dpcrtlmm$(OBJ) : dpcrtlmm$(C) $(MASTERDEP) dpc_intdata$(H) dpc_trap$(H) dpc_log$(H) dpc_safelist$(H) dpc_dbghooks$(H) dpc_biglock$(H) dpc_blkarray$(H)
	$(COMPILE) dpcrtlmm$(C)

dpc_log$(OBJ) : dpc_log$(C) $(MASTERDEP) dpc_intdata$(H) dpc_log$(H)
	$(COMPILE) dpc_log$(C)

dpc_vptrap$(OBJ) : dpc_vptrap$(C) $(MASTERDEP) dpc_intdata$(H) dpc_trap$(H) dpc_log$(H) dpc_safelist$(H) dpc_vptrap$(H)
	$(COMPILE) dpc_vptrap$(C)

dpc_trap$(OBJ) : dpc_trap$(C) $(MASTERDEP) dpc_intdata$(H) dpc_log$(H) dpc_trap$(H) dpc_dbghooks$(H)
	$(COMPILE) dpc_trap$(C)

dpc_realloc$(OBJ) : dpc_realloc$(C) $(MASTERDEP) dpc_intdata$(H) dpc_vptrap$(H) dpc_locktrap$(H) dpc_iblkptr$(H) dpc_dbghooks$(H) dpc_log$(H)
	$(COMPILE) dpc_realloc$(C)

dpc_intdata$(OBJ) : dpc_intdata$(C) $(MASTERDEP) dpc_intdata$(H)
	$(COMPILE) dpc_intdata$(C)

dpc_iblkptr$(OBJ) : dpc_iblkptr$(C) $(MASTERDEP) dpc_intdata$(H) dpc_trap$(H) dpc_vptrap$(H) dpc_iblkptr$(H)
	$(COMPILE) dpc_iblkptr$(C)

dpc_getblksz$(OBJ) : dpc_getblksz$(C) $(MASTERDEP) dpc_intdata$(H) dpc_trap$(H) dpc_vptrap$(H) dpc_iblkptr$(H)
	$(COMPILE) dpc_getblksz$(C)

dpc_bloclock$(OBJ) : dpc_bloclock$(C) $(MASTERDEP) dpc_bdflags$(H)
	$(COMPILE) dpc_bloclock$(C)

dpc_bdflags$(OBJ) : dpc_bdflags$(C) $(MASTERDEP) dpc_intdata$(H) dpc_vptrap$(H) dpc_iblkptr$(H) dpc_dbghooks$(H) dpc_bdflags$(H)
	$(COMPILE) dpc_bdflags$(C)

dpc_biglock$(OBJ) : dpc_biglock$(C) dpc_biglock$(H) $(MASTERDEP)
	$(COMPILE) -D_RECURSIVE -D__UNIX__ dpc_biglock$(C)

clean : confclean
	@-$(ERASE) $(OBJECTS)
	@-$(ERASE) $(LOGFILE) $(COREDUMPS) $(BACKUPS)
	@-$(ERASE) example1$(OBJ) example2$(OBJ) example3$(OBJ)
	@-$(ERASE) dpccap$(OBJ)
	@-$(ERASE) example1 example2 example3
	@-$(ERASE) $(LIBNAME)

# Supported for backwards compatibillity
clobber : clean


example : example1 example2 example3  #Type make -f Makefile.gnu example to make all examples

# example1 program (C)

example1 : $(MASTERDEP) example1$(OBJ) $(LIBNAME)
	$(LINK) -o example1 example1$(OBJ) $(LIBNAME)

example1$(OBJ) : $(MASTERDEP) example1$(C)
	$(COMPILE) example1$(C)

# example2 program (C++)

example2 : $(MASTERDEP) example2$(OBJ) dpccap$(OBJ) $(LIBNAME)
	$(LINK_CPP) -o example2 example2$(OBJ) dpccap$(OBJ) $(LIBNAME)

example2$(OBJ) : $(MASTERDEP) example2$(CPP) dpccap$(CPP)
	$(CPP_COMPILE) example2$(CPP)

dpccap$(OBJ) : $(MASTERDEP) dpccap$(CPP) dpccap$(H)
	$(CPP_COMPILE) dpccap$(CPP)

# Stress test (example3)

example3 : $(MASTERDEP) example3$(OBJ) $(LIBNAME)
	$(LINK) -o example3 example3$(OBJ) $(LIBNAME)

example3$(OBJ) : $(MASTERDEP) example3$(C) dpc_intdata$(H) dpc_safelist$(H)
	$(COMPILE) example3$(C)


all:
	make -f $(THISFILE) config
	./config
	make -f $(THISFILE) $(LIBNAME)
	make -f $(THISFILE) example

test : # Maintainer thing only
	make -f $(THISFILE) clobber
	make -f $(THISFILE) all
	make -f $(THISFILE) clobber


# This section is for the configurator program

config : config$(OBJ)
	$(LINK) -o config config$(OBJ)

config$(OBJ) : config$(C) dpcrtlmm$(H)
	$(COMPILE) config$(C)

confclean:
	@-$(ERASE) config config$(OBJ) config$(H)
	@$(COPY) config$(H).default config$(H)
