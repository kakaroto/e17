REM THIS SCRIPT IS USED TO CREATE A HEADER FILE CONATINING THE VERSION NUMBERS
REM Date: 2010/12/15, Author: Vincent Torri
REM ---------------    %1       %2       %3    -----
REM call %POSTBUILD $(VMAJ)  $(VMIN)  $(VMIC)
REM ---------------    %1       %2       %3    -----
REM

@echo off

set VMAJ=%~1
set VMIN=%~2
set VMIC=%~3

rem Remove common\config_version.h

if exist common\config_version.h (
	DEL /F common\config_version.h
)

rem Fill common\config_version.h

echo #ifndef VMAJ >> common\config_version.h
echo # define VMAJ %VMAJ% >> common\config_version.h
echo #endif >> common\config_version.h

echo #ifndef VMIN >> common\config_version.h
echo # define VMIN %VMIN% >> common\config_version.h
echo #endif >> common\config_version.h

echo #ifndef VMIC >> common\config_version.h
echo # define VMIC %VMIC% >> common\config_version.h
echo #endif >> common\config_version.h

echo #ifndef VREV >> common\config_version.h
echo # define VREV 0 >> common\config_version.h
echo #endif >> common\config_version.h

rem Patching evas

echo Patching Evas...
..\patch.exe -d ..\..\e17\evas -p0 < evas_vs.diff
