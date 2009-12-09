@for %%a in (cl.exe) do @if EXIST %%~$PATH:a set vcvars_found=Y
@if "%vcvars_found%"=="Y" goto after_vcvars
@call "c:\Program Files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat"
@call "c:\Program Files (x86)\Microsoft Visual Studio 8\VC\bin\vcvars32.bat"
@call "c:/Microsoft Visual Studio 8/VC/bin/vcvars32.bat"

:after_vcvars

@IF /I "%1" == "RUN" goto run
@IF "%1" == "" (set VER=02) ELSE (set VER=%1)

@IF "%WINHPCDIR%" == "" set WINHPCDIR="c:\WinHPC"
@IF "%WINHPCDIR%" == "" goto err_mpi_not_found

@REM /RTC{s,c,u} : stack frame runtime checking, convert checks, unininitialized checks
@REM /Wall, /we : warning level 4, warning as error
@REM /J : unsigned char
@REM /ZI: debug info
@REM /O2: maximize speed
@REM /Og: global opt
@REM /D : define

@set LIBS= kernel32.lib msmpi.lib
@set LIBDIRS=/LIBPATH:%WINHPCDIR%\lib\i386

@set INPUTS=%LIBS% hellompi%VER%.c
@set INCDIRS=/I %WINHPCDIR%\include

@REM /link /NODEFAULTLIB:MSVCR80D
@REM /ALLOWISOLATION:NO : turn off manifest look up for some crt library
cl /MP /analyze:stacksize 38000 /analyze /J /W4 %FLAGS% %INCDIRS% %INPUTS% /link /ALLOWISOLATION:NO %LIBDIRS%
@if NOT "%ERRORLEVEL%"=="0" goto error

@echo "done"
@goto end

:run
%WINHPCDIR%\bin\mpiexec -n 4 hellompi%2%.exe
@goto end

:err_mpi_not_found
@echo "error: WINHPCDIR not set. Windows HPC directory not found"
@goto end

:error
@echo "something gone wrong"
@goto end

:end