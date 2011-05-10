# Make file for DPCRTLMM
# Written by Overlord David Duncan Ross Palmer
# Overlord@DayboLogic.co.uk
# 24th August 2000
# Last modified 29th August 2000

# This Makefile designed for Borland C++, you'll have to change the tools
# at the top of the file in the variables section to use Micro$oft or Colelus
# linker.  I have made everything easy to modify by putting it at the top
# of the file but if you have problems, email me.

# Paths & filenames - If this file is not called Makefile remember to
# change the variable 'THISFILE'
THISFILE=Makefile
LIBTITLE=dpcrtlmm
LIBNAME=$(LIBTITLE).lib
# Master dependancies ALWAYS cause a rebuild
MASTERDEP=build.h dpcrtlmm.h $(THISFILE)

# Tools
LIBRARY=tlib
# -A : ANSI C only
# -c : Compile with no link
COMPILE=bcc32 -A -c

# File control commands
FILEEXISTS=if exist
ERASE=erase
PRINT=@echo

$(LIBNAME) : alloc.obj blkarray.obj calloc.obj free.obj isbad.obj stats.obj dbghooks.obj locktrap.obj safelist.obj dpcrtlmm.obj log.obj vptrap.obj trap.obj strtstop.obj realloc.obj intdata.obj iblkptr.obj getblksz.obj bloclock.obj bdflags.obj
  $(LIBRARY) $(LIBNAME) -+alloc.obj
  $(LIBRARY) $(LIBNAME) -+blkarray.obj
  $(LIBRARY) $(LIBNAME) -+calloc.obj
  $(LIBRARY) $(LIBNAME) -+free.obj
  $(LIBRARY) $(LIBNAME) -+isbad.obj
  $(LIBRARY) $(LIBNAME) -+stats.obj
  $(LIBRARY) $(LIBNAME) -+dbghooks.obj
  $(LIBRARY) $(LIBNAME) -+locktrap.obj
  $(LIBRARY) $(LIBNAME) -+safelist.obj
  $(LIBRARY) $(LIBNAME) -+dpcrtlmm.obj
  $(LIBRARY) $(LIBNAME) -+log.obj
  $(LIBRARY) $(LIBNAME) -+vptrap.obj
  $(LIBRARY) $(LIBNAME) -+trap.obj
  $(LIBRARY) $(LIBNAME) -+strtstop.obj
  $(LIBRARY) $(LIBNAME) -+realloc.obj
  $(LIBRARY) $(LIBNAME) -+intdata.obj
  $(LIBRARY) $(LIBNAME) -+iblkptr.obj
  $(LIBRARY) $(LIBNAME) -+getblksz.obj
  $(LIBRARY) $(LIBNAME) -+bloclock.obj
  $(LIBRARY) $(LIBNAME) -+bdflags.obj
  $(PRINT).
  $(PRINT) (C)2000, Overlord DDRP, Daybo Logic.
  $(PRINT) Please read the license agreement before proceeding
  $(PRINT) http://daybologic.com/Dev/dpcrtlmm

alloc.obj : $(MASTERDEP) intdata.h log.h vptrap.h dbghooks.h
  $(COMPILE) alloc.c

blkarray.obj : $(MASTERDEP) intdata.h log.h trap.h safelist.h dbghooks.h
  $(COMPILE) blkarray.c

calloc.obj : $(MASTERDEP) intdata.h log.h iblkptr.h dbghooks.h
  $(COMPILE) calloc.c

free.obj : $(MASTERDEP) intdata.h vptrap.h locktrap.h log.h trap.h dbghooks.h
  $(COMPILE) free.c

isbad.obj : $(MASTERDEP) intdata.h trap.h safelist.h
  $(COMPILE) isbad.c

stats.obj : $(MASTERDEP) intdata.h
  $(COMPILE) stats.c

dbghooks.obj : $(MASTERDEP) intdata.h log.h dbghooks.h
  $(COMPILE) dbghooks.c

locktrap.obj : $(MASTERDEP) intdata.h trap.h locktrap.h
  $(COMPILE) locktrap.c

safelist.obj : $(MASTERDEP) intdata.h safelist.h
  $(COMPILE) safelist.c

dpcrtlmm.obj : $(MASTERDEP)
  $(COMPILE) dpcrtlmm.c

log.obj : $(MASTERDEP) intdata.h log.h
  $(COMPILE) log.c

vptrap.obj : $(MASTERDEP) intdata.h trap.h log.h safelist.h vptrap.h
  $(COMPILE) vptrap.c

trap.obj : $(MASTERDEP) intdata.h log.h trap.h dbghooks.h
  $(COMPILE) trap.c

strtstop.obj : $(MASTERDEP) intdata.h trap.h log.h safelist.h dbghooks.h
  $(COMPILE) strtstop.c

realloc.obj : $(MASTERDEP) intdata.h vptrap.h locktrap.h iblkptr.h dbghooks.h
  $(COMPILE) realloc.c

intdata.obj : $(MASTERDEP) intdata.h
  $(COMPILE) intdata.c

iblkptr.obj : $(MASTERDEP) intdata.h trap.h vptrap.h iblkptr.h
  $(COMPILE) iblkptr.c

getblksz.obj : $(MASTERDEP) intdata.h trap.h vptrap.h iblkptr.h
  $(COMPILE) getblksz.c

bloclock.obj : $(MASTERDEP)
  $(COMPILE) bloclock.c

bdflags.obj : $(MASTERDEP) intdata.h vptrap.h iblkptr.h dbghooks.h
  $(COMPILE) bdflags.c

clean:
  $(FILEEXISTS) alloc.obj $(ERASE) alloc.obj
  $(FILEEXISTS) blkarray.obj $(ERASE) blkarray.obj
  $(FILEEXISTS) calloc.obj $(ERASE) calloc.obj
  $(FILEEXISTS) free.obj $(ERASE) free.obj
  $(FILEEXISTS) isbad.obj $(ERASE) isbad.obj
  $(FILEEXISTS) stats.obj $(ERASE) stats.obj
  $(FILEEXISTS) dbghooks.obj $(ERASE) dbghooks.obj
  $(FILEEXISTS) locktrap.obj $(ERASE) locktrap.obj
  $(FILEEXISTS) safelist.obj $(ERASE) safelist.obj
  $(FILEEXISTS) dpcrtlmm.obj $(ERASE) dpcrtlmm.obj
  $(FILEEXISTS) log.obj $(ERASE) log.obj
  $(FILEEXISTS) vptrap.obj $(ERASE) vptrap.obj
  $(FILEEXISTS) trap.obj $(ERASE) trap.obj
  $(FILEEXISTS) strtstop.obj $(ERASE) strtstop.obj
  $(FILEEXISTS) realloc.obj $(ERASE) realloc.obj
  $(FILEEXISTS) intdata.obj $(ERASE) intdata.obj
  $(FILEEXISTS) iblkptr.obj $(ERASE) iblkptr.obj
  $(FILEEXISTS) getblksz.obj $(ERASE) getblksz.obj
  $(FILEEXISTS) bloclock.obj $(ERASE) bloclock.obj
  $(FILEEXISTS) bdflags.obj $(ERASE) bdflags.obj
  $(FILEEXISTS) $(LIBTITLE).bak $(ERASE) $(LIBTITLE).bak
