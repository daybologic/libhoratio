# Microsoft Developer Studio Generated NMAKE File, Based on dpcrtlmm.dsp
!IF "$(CFG)" == ""
CFG=dpcrtlmm - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dpcrtlmm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dpcrtlmm - Win32 Release" && "$(CFG)" != "dpcrtlmm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dpcrtlmm.mak" CFG="dpcrtlmm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dpcrtlmm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dpcrtlmm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dpcrtlmm - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dpcrtlmm.lib"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\bdflags.obj"
	-@erase "$(INTDIR)\blkarray.obj"
	-@erase "$(INTDIR)\bloclock.obj"
	-@erase "$(INTDIR)\calloc.obj"
	-@erase "$(INTDIR)\dbghooks.obj"
	-@erase "$(INTDIR)\dpcrtlmm.obj"
	-@erase "$(INTDIR)\free.obj"
	-@erase "$(INTDIR)\getblksz.obj"
	-@erase "$(INTDIR)\iblkptr.obj"
	-@erase "$(INTDIR)\intdata.obj"
	-@erase "$(INTDIR)\isbad.obj"
	-@erase "$(INTDIR)\locktrap.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\realloc.obj"
	-@erase "$(INTDIR)\safelist.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\trap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vptrap.obj"
	-@erase "$(OUTDIR)\dpcrtlmm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\dpcrtlmm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dpcrtlmm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dpcrtlmm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\realloc.obj" \
	"$(INTDIR)\bdflags.obj" \
	"$(INTDIR)\blkarray.obj" \
	"$(INTDIR)\bloclock.obj" \
	"$(INTDIR)\calloc.obj" \
	"$(INTDIR)\dbghooks.obj" \
	"$(INTDIR)\dpcrtlmm.obj" \
	"$(INTDIR)\free.obj" \
	"$(INTDIR)\getblksz.obj" \
	"$(INTDIR)\iblkptr.obj" \
	"$(INTDIR)\intdata.obj" \
	"$(INTDIR)\isbad.obj" \
	"$(INTDIR)\locktrap.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\safelist.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\trap.obj" \
	"$(INTDIR)\vptrap.obj"

"$(OUTDIR)\dpcrtlmm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dpcrtlmm - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\dpcrtlmm.lib"


CLEAN :
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\bdflags.obj"
	-@erase "$(INTDIR)\blkarray.obj"
	-@erase "$(INTDIR)\bloclock.obj"
	-@erase "$(INTDIR)\calloc.obj"
	-@erase "$(INTDIR)\dbghooks.obj"
	-@erase "$(INTDIR)\dpcrtlmm.obj"
	-@erase "$(INTDIR)\free.obj"
	-@erase "$(INTDIR)\getblksz.obj"
	-@erase "$(INTDIR)\iblkptr.obj"
	-@erase "$(INTDIR)\intdata.obj"
	-@erase "$(INTDIR)\isbad.obj"
	-@erase "$(INTDIR)\locktrap.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\realloc.obj"
	-@erase "$(INTDIR)\safelist.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\trap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vptrap.obj"
	-@erase "$(OUTDIR)\dpcrtlmm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\dpcrtlmm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dpcrtlmm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dpcrtlmm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\realloc.obj" \
	"$(INTDIR)\bdflags.obj" \
	"$(INTDIR)\blkarray.obj" \
	"$(INTDIR)\bloclock.obj" \
	"$(INTDIR)\calloc.obj" \
	"$(INTDIR)\dbghooks.obj" \
	"$(INTDIR)\dpcrtlmm.obj" \
	"$(INTDIR)\free.obj" \
	"$(INTDIR)\getblksz.obj" \
	"$(INTDIR)\iblkptr.obj" \
	"$(INTDIR)\intdata.obj" \
	"$(INTDIR)\isbad.obj" \
	"$(INTDIR)\locktrap.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\safelist.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\trap.obj" \
	"$(INTDIR)\vptrap.obj"

"$(OUTDIR)\dpcrtlmm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dpcrtlmm.dep")
!INCLUDE "dpcrtlmm.dep"
!ELSE 
!MESSAGE Warning: cannot find "dpcrtlmm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dpcrtlmm - Win32 Release" || "$(CFG)" == "dpcrtlmm - Win32 Debug"
SOURCE=.\alloc.c

"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bdflags.c

"$(INTDIR)\bdflags.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\blkarray.c

"$(INTDIR)\blkarray.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bloclock.c

"$(INTDIR)\bloclock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\calloc.c

"$(INTDIR)\calloc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dbghooks.c

"$(INTDIR)\dbghooks.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dpcrtlmm.c

"$(INTDIR)\dpcrtlmm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\free.c

"$(INTDIR)\free.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\getblksz.c

"$(INTDIR)\getblksz.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\iblkptr.c

"$(INTDIR)\iblkptr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\intdata.c

"$(INTDIR)\intdata.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\isbad.c

"$(INTDIR)\isbad.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\locktrap.c

"$(INTDIR)\locktrap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\log.c

"$(INTDIR)\log.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\realloc.c

"$(INTDIR)\realloc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\safelist.c

"$(INTDIR)\safelist.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\stats.c

"$(INTDIR)\stats.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\trap.c

"$(INTDIR)\trap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vptrap.c

"$(INTDIR)\vptrap.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

