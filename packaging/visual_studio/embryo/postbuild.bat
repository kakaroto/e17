REM THIS SCRIPT IS USED TO COPY EMBRYO FILES TO EXTRERN DIRECTORY
REM Date: 2010/12/15, Authors: forumer@smartmobili.com, Vincent Torri
REM ---------------       %1              %2          %3         %4    -----
REM call %POSTBUILD $(ProjectType)  $(TargetDir)  $(LibName)   $(VMAJ)
REM ---------------       %1              %2          %3         %4    -----
REM

@echo off

rem Set generic variables

set PROJECT_TYPE=%~1
set EXT_DIR=%~2
set LIB_NAME=%~3
set VMAJ=%~4

set INC_DIR_LIB=embryo-%VMAJ%

set EXT_DIR_BIN=%EXT_DIR%\bin
set EXT_DIR_LIB=%EXT_DIR%\lib
set EXT_DIR_INC=%EXT_DIR%\include
set EXT_DIR_INC_LIB=%EXT_DIR%\include\%INC_DIR_LIB%
set LIB_NAME_DLL=%LIB_NAME%-%VMAJ%.dll
set LIB_NAME_LIB=%LIB_NAME%.lib

echo %LIB_NAME_DLL%

if not exist %EXT_DIR_BIN% (
	MKDIR %EXT_DIR_BIN%
)

if not exist %EXT_DIR_LIB% (
	MKDIR %EXT_DIR_LIB%
)

if not exist %EXT_DIR_INC% (
	MKDIR %EXT_DIR_INC%
)

if not exist %EXT_DIR_INC_LIB% (
	MKDIR %EXT_DIR_INC_LIB%
)

echo Copying Embryo files...
COPY /Y "..\%PROJECT_TYPE%\out_debug\%LIB_NAME_DLL%" "%EXT_DIR_BIN%"
COPY /Y "..\%PROJECT_TYPE%\out_debug\embryo_cc.exe" "%EXT_DIR_BIN%"

COPY /Y "..\%PROJECT_TYPE%\out_debug\%LIB_NAME_LIB%" "%EXT_DIR_LIB%"

COPY /Y "..\..\e17\embryo\src\lib\Embryo.h" "%EXT_DIR_INC_LIB%"
