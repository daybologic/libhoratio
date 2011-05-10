# Make file for DPCRTLMM
# Written by Overlord David Duncan Ross Palmer
# Overlord@DayboLogic.co.uk
# 15th September 2000

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
# make clean deletes all temporary binaries used to create the library
# make clobber deletes target binaries (example programs and library).
# make test builds all and then clobbers, it's for me to test the build before
#      distribution only.
# make all creates library and example programs

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
ERASE=rm -f

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
MASTERDEP=build$(H) dpcrtlmm$(H) $(THISFILE)
OBJECTS=alloc$(OBJ) blkarray$(OBJ) calloc$(OBJ) free$(OBJ) isbad$(OBJ) stats$(OBJ) dbghooks$(OBJ) locktrap$(OBJ) safelist$(OBJ) dpcrtlmm$(OBJ) log$(OBJ) vptrap$(OBJ) trap$(OBJ) realloc$(OBJ) intdata$(OBJ) iblkptr$(OBJ) getblksz$(OBJ) bloclock$(OBJ) bdflags$(OBJ)
LOGFILE=DPCRTLMM.LOG
COREDUMPS=example1.core example2.core example3.core core
BACKUPS=*~
LIBADD=$(LIBRARY) $(LIBOPTS) $(LIBNAME)

# Build rules

$(LIBNAME) : $(MASTERDEP) $(OBJECTS)
	$(LIBADD) alloc$(OBJ)
	$(LIBADD) blkarray$(OBJ)
	$(LIBADD) calloc$(OBJ)
	$(LIBADD) free$(OBJ)
	$(LIBADD) isbad$(OBJ)
	$(LIBADD) stats$(OBJ)
	$(LIBADD) dbghooks$(OBJ)
	$(LIBADD) locktrap$(OBJ)
	$(LIBADD) safelist$(OBJ)
	$(LIBADD) dpcrtlmm$(OBJ)
	$(LIBADD) log$(OBJ)
	$(LIBADD) vptrap$(OBJ)
	$(LIBADD) trap$(OBJ)
	$(LIBADD) realloc$(OBJ)
	$(LIBADD) intdata$(OBJ)
	$(LIBADD) iblkptr$(OBJ)
	$(LIBADD) getblksz$(OBJ)
	$(LIBADD) bloclock$(OBJ)
	$(LIBADD) bdflags$(OBJ)
	$(RANLIB)

alloc$(OBJ) : alloc$(C) $(MASTERDEP) intdata$(H) log$(H) vptrap$(H) dbghooks$(H)
	$(COMPILE) alloc$(C)

blkarray$(OBJ) : blkarray$(C) $(MASTERDEP) intdata$(H) log$(H) trap$(H) safelist$(H) dbghooks$(H)
	$(COMPILE) blkarray$(C)

calloc$(OBJ) : calloc$(C) $(MASTERDEP) intdata$(H) log$(H) iblkptr$(H) dbghooks$(H)
	$(COMPILE) calloc$(C)

free$(OBJ) : free$(C) $(MASTERDEP) intdata$(H) vptrap$(H) locktrap$(H) log$(H) trap$(H) dbghooks$(H)
	$(COMPILE) free$(C)

isbad$(OBJ) : isbad$(C) $(MASTERDEP) intdata$(H) trap$(H) safelist$(H)
	$(COMPILE) isbad$(C)

stats$(OBJ) : stats$(C) $(MASTERDEP) intdata$(H)
	$(COMPILE) stats$(C)

dbghooks$(OBJ) : dbghooks$(C) $(MASTERDEP) intdata$(H) log$(H) dbghooks$(H)
	$(COMPILE) dbghooks$(C)

locktrap$(OBJ) : locktrap$(C) $(MASTERDEP) intdata$(H) trap$(H) locktrap$(H)
	$(COMPILE) locktrap$(C)

safelist$(OBJ) : safelist$(C) $(MASTERDEP) intdata$(H) safelist$(H)
	$(COMPILE) safelist$(C)

dpcrtlmm$(OBJ) : dpcrtlmm$(C) $(MASTERDEP) intdata$(H) trap$(H) log$(H) safelist$(H) dbghooks$(H)
	$(COMPILE) dpcrtlmm$(C)

log$(OBJ) : log$(C) $(MASTERDEP) intdata$(H) log$(H)
	$(COMPILE) log$(C)

vptrap$(OBJ) : vptrap$(C) $(MASTERDEP) intdata$(H) trap$(H) log$(H) safelist$(H) vptrap$(H)
	$(COMPILE) vptrap$(C)

trap$(OBJ) : trap$(C) $(MASTERDEP) intdata$(H) log$(H) trap$(H) dbghooks$(H)
	$(COMPILE) trap$(C)

realloc$(OBJ) : realloc$(C) $(MASTERDEP) intdata$(H) vptrap$(H) locktrap$(H) iblkptr$(H) dbghooks$(H) log$(H)
	$(COMPILE) realloc$(C)

intdata$(OBJ) : intdata$(C) $(MASTERDEP) intdata$(H)
	$(COMPILE) intdata$(C)

iblkptr$(OBJ) : iblkptr$(C) $(MASTERDEP) intdata$(H) trap$(H) vptrap$(H) iblkptr$(H)
	$(COMPILE) iblkptr$(C)

getblksz$(OBJ) : getblksz$(C) $(MASTERDEP) intdata$(H) trap$(H) vptrap$(H) iblkptr$(H)
	$(COMPILE) getblksz$(C)

bloclock$(OBJ) : bloclock$(C) $(MASTERDEP)
	$(COMPILE) bloclock$(C)

bdflags$(OBJ) : bdflags$(C) $(MASTERDEP) intdata$(H) vptrap$(H) iblkptr$(H) dbghooks$(H)
	$(COMPILE) bdflags$(C)

critical$(OBJ) : critical$(C) $(MASTERDEP) critical$(H)
	$(COMPILE) critical$(C)

clean:
	@-$(ERASE) $(OBJECTS)
	@-$(ERASE) $(LOGFILE) $(COREDUMPS) $(BACKUPS)
	@-$(ERASE) example1$(OBJ) example2$(OBJ) example3$(OBJ)
	@-$(ERASE) dpccap$(OBJ)

# Destroy everything including final executables and library
clobber: clean
	@-$(ERASE) $(LIBNAME)
	@-$(ERASE) example1 example2 example3


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

example3$(OBJ) : $(MASTERDEP) example3$(C) intdata$(H) safelist$(H)
	$(COMPILE) example3$(C)


all : $(LIBNAME) example

test : # Maintainer thing only
	make -f $(THISFILE) clobber
	make -f $(THISFILE) all
	make -f $(THISFILE) clobber
