REM THIS SCRIPT IS USED TO COPY EVIL FILES TO EXTRERN DIRECTORY
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

set INC_DIR_LIB=evil-%VMAJ%

set EXT_DIR_BIN=%EXT_DIR%\bin
set EXT_DIR_LIB=%EXT_DIR%\lib
set EXT_DIR_INC=%EXT_DIR%\include
set EXT_DIR_INC_LIB=%EXT_DIR%\include\%INC_DIR_LIB%
set EXT_DIR_INC_SYS=%EXT_DIR%\include\sys
set LIB_NAME_DLL=%LIB_NAME%-%VMAJ%.dll
set LIB_NAME_LIB=%LIB_NAME%.lib

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

if not exist %EXT_DIR_INC%\sys (
	MKDIR %EXT_DIR_INC%\sys
)

echo Copying Evil files...
COPY /Y "..\%PROJECT_TYPE%\out_debug\%LIB_NAME_DLL%" "%EXT_DIR_BIN%"
COPY /Y "..\%PROJECT_TYPE%\out_debug\libdl.dll" "%EXT_DIR_BIN%"

COPY /Y "..\%PROJECT_TYPE%\out_debug\%LIB_NAME_LIB%" "%EXT_DIR_LIB%"
COPY /Y "..\%PROJECT_TYPE%\out_debug\libdl.lib" "%EXT_DIR_LIB%"

COPY /Y "..\..\e17\evil\src\lib\Evil.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_fcntl.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_langinfo.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_libgen.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_main.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_stdlib.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_stdio.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_string.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_time.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_unistd.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\evil_util.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evil\src\lib\dirent.h" "%EXT_DIR_INC%"
COPY /Y "..\..\e17\evil\src\lib\dlfcn\dlfcn.h" "%EXT_DIR_INC%"
COPY /Y "..\..\e17\evil\src\lib\fnmatch.h" "%EXT_DIR_INC%"
COPY /Y "..\..\e17\evil\src\lib\pwd.h" "%EXT_DIR_INC%"
COPY /Y "..\..\e17\evil\src\lib\sys\mman.h" "%EXT_DIR_INC_SYS%"
COPY /Y "common\sys\time.h" "%EXT_DIR_INC_SYS%"
