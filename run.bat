@Echo off
echo Relies on Borland C++ installation, otherwise you'll have to play with
echo my Makefile or build your own
echo.
if not exist Makefile.bor goto nomake

make -f Makefile.bor %1 %2 %3 %4 %5 %6 %7 %8 %9
if not exist dpcrtlmm.lib goto nolib
if not exist example1.exe goto noexe
example
echo Press any key to read log of memory activities
pause > nul
if exist dpcrtlmm.log more < dpcrtlmm.log
if not exist dpcrtlmm.log echo No log, assume library was never started or disk full
goto doneit

:nomake
echo ERROR: No makefile for Borland (Makefile.bor), obtain distribution set
echo again.  http://daybologic.com/Dev/dpcrtlmm or email
echo Overlord@DayboLogic.co.uk for help.
pause
goto doneit

:nolib
echo ERROR: Something failed creating the library (dpcrtlmm.lib), I can't tell
echo you what, please post the entire output of this make to
echo Overlord@DayboLogic.co.uk for assistance
pause
goto doneit

:noexe
echo ERROR: Could not generate example1 program but library built successfully.
echo Link dpcrtlmm.lib with your own apps.  I'd appreciate the output of this
echo session to determine why I couldn't build example1.exe
pause

:doneit
