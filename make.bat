@REM ====================
@REM find compiler
@REM ====================

@REM using vcvars32.bat instead of cl.exe because SN Systems uses a cl.exe for an exe name as well for VS-PS3 
@set vcvars_found=N
@for %%a in (vcvars32.bat) do @if EXIST %%~$PATH:a set vcvars_found=Y
@if "%vcvars_found%"=="Y" goto after_vcvars
@for %%a in ("c:\Program Files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat" 
	 "c:\Program Files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat" 
	 "c:\Program Files (x86)\Microsoft Visual Studio 8\VC\bin\vcvars32.bat") do @(
	@if EXIST %%a (
	   @call %%a
	   @goto after_vcvars
	)
)

:after_vcvars

@IF /I "%1" == "RUN" (
	set VER=%2
	goto run
	)
@IF "%1" == "" (set VER=02) ELSE (set VER=%1)

@IF "%WINHPCDIR%" == "" set WINHPCDIR="c:\WinHPC"
@IF "%WINHPCDIR%" == "" goto err_mpi_not_found


@set LIBS= kernel32.lib msmpi.lib
@set LIBDIRS=/LIBPATH:%WINHPCDIR%\lib\i386

@set INPUTS=%LIBS% hellompi%VER%.c
@set INCDIRS=/I %WINHPCDIR%\include

@REM /link /NODEFAULTLIB:MSVCR80D
@REM /ALLOWISOLATION:NO : turn off manifest look up for some crt library
cl /MP /analyze:stacksize 38000 /analyze /J /W4 %FLAGS% %INCDIRS% %INPUTS% /link /ALLOWISOLATION:NO %LIBDIRS%
@if NOT "%ERRORLEVEL%"=="0" goto error

REM @echo "done"
REM @goto end

:run

set NCPUS=4
@IF "%VER%" == "05" set NCPUS=8

%WINHPCDIR%\bin\mpiexec -n %NCPUS% hellompi%VER%.exe
@goto end

:err_mpi_not_found
@echo "error: WINHPCDIR not set. Windows HPC directory not found"
@goto end

:error
@echo "something gone wrong"
@goto end

:end