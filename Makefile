# Make file for DPCRTLMM
# Written by Overlord David Duncan Ross Palmer
# Overlord@DayboLogic.co.uk
# 15th September 2000

# This Makefile designed for GNU tools, gcc GNU C compiler, GNU's make
# not neccersarily on UNIX but that's what I'm building on.  If you have
# Borland C++ please use Makefile.bor instead.

# Paths & filenames - Name of the makefile can be overridden on the command line in the form
# make -MAKEFILE=newmakefilename
.if !defined(MAKEFILE)
THISFILE=Makefile
.endif

LIBTITLE=dpcrtlmm
LIBNAME=$(LIBTITLE).lib
# Master dependancies ALWAYS cause a rebuild
MASTERDEP=build.h dpcrtlmm.h $(THISFILE)
OBJECTS=alloc.o blkarray.o calloc.o free.o isbad.o stats.o dbghooks.o locktrap.o safelist.o dpcrtlmm.o log.o vptrap.o trap.o strtstop.o realloc.o intdata.o iblkptr.o getblksz.o bloclock.o bdflags.o

# Tools
LIBRARY=libtool
LIBOPTS=--mode=link all-static version-info 1:1 release InternalSnapshot2
# -ansi : Only accept ANSI C code
# -pedantic : Be very fussy about ANSI related problems
# -c : Compile with no link
COMPILE=gcc -c -ansi -pedantic

# File control commands
FILEEXISTS=if exist
ERASE=rm

$(LIBNAME) : $(MASTERDEP) $(OBJECTS)
	$(LIBRARY) $(LIBOPTS) -o $(LIBNAME) $(OBJECTS)

alloc.o : alloc.c $(MASTERDEP) intdata.h log.h vptrap.h dbghooks.h
	$(COMPILE) alloc.c

blkarray.o : blkarray.c $(MASTERDEP) intdata.h log.h trap.h safelist.h dbghooks.h
	$(COMPILE) blkarray.c

calloc.o : calloc.c $(MASTERDEP) intdata.h log.h iblkptr.h dbghooks.h
	$(COMPILE) calloc.c

free.o : free.c $(MASTERDEP) intdata.h vptrap.h locktrap.h log.h trap.h dbghooks.h
	$(COMPILE) free.c

isbad.o : isbad.c $(MASTERDEP) intdata.h trap.h safelist.h
	$(COMPILE) isbad.c

stats.o : stats.c $(MASTERDEP) intdata.h
	$(COMPILE) stats.c

dbghooks.o : dbghooks.c $(MASTERDEP) intdata.h log.h dbghooks.h
	$(COMPILE) dbghooks.c

locktrap.o : locktrap.c $(MASTERDEP) intdata.h trap.h locktrap.h
	$(COMPILE) locktrap.c

safelist.o : safelist.c $(MASTERDEP) intdata.h safelist.h
	$(COMPILE) safelist.c

dpcrtlmm.o : dpcrtlmm.c $(MASTERDEP)
	$(COMPILE) dpcrtlmm.c

log.o : log.c $(MASTERDEP) intdata.h log.h
	$(COMPILE) log.c

vptrap.o : vptrap.c $(MASTERDEP) intdata.h trap.h log.h safelist.h vptrap.h
	$(COMPILE) vptrap.c

trap.o : trap.c $(MASTERDEP) intdata.h log.h trap.h dbghooks.h
	$(COMPILE) trap.c

strtstop.o : strtstop.c $(MASTERDEP) intdata.h trap.h log.h safelist.h dbghooks.h
	$(COMPILE) strtstop.c

realloc.o : realloc.c $(MASTERDEP) intdata.h vptrap.h locktrap.h iblkptr.h dbghooks.h
	$(COMPILE) realloc.c

intdata.o : intdata.c $(MASTERDEP) intdata.h
	$(COMPILE) intdata.c

iblkptr.o : iblkptr.c $(MASTERDEP) intdata.h trap.h vptrap.h iblkptr.h
	$(COMPILE) iblkptr.c

getblksz.o : getblksz.c $(MASTERDEP) intdata.h trap.h vptrap.h iblkptr.h
	$(COMPILE) getblksz.c

bloclock.o : bloclock.c $(MASTERDEP)
	$(COMPILE) bloclock.c

bdflags.o : bdflags.c $(MASTERDEP) intdata.h vptrap.h iblkptr.h dbghooks.h
	$(COMPILE) bdflags.c

clean:
	-$(ERASE) $(OBJECTS) $(LIBNAME) example.o example


# Type make example to ceate this small example program

example:
example.o : $(MASTERDEP) example.c
	$(COMPILE) example.c
example : $(MASTERDEP) example.o $(LIBNAME)
	make
	ld -oexample $(LIBNAME) example.o
