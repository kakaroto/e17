@echo off

rem Set external libraries directory.
set EXT_DIR=%cd%\..\extern

if not exist %EXT_DIR% (
	set EXT_DIR=
	echo ERROR: External libs dir is not set.
	pause
	goto END
)

if "%DXSDK_DIR%" == "" (
	echo ERROR: Microsoft DirectX SDK is not installed.
	pause
	goto END
)

rem Check for basic requirements for Visual Studio 2010
if "%VS100COMNTOOLS%" == "" (
	echo WARNING: Microsoft Visual Studio 2010 is not installed.
	echo WARNING: Trying to launch Microsoft Visual Studio 2008.
	pause
	goto NOVS10
)

set PROJECT_TYPE=vs10
set VSCOMMONTOOLS=%VS100COMNTOOLS%vsvars32.bat

goto STARTVS

:NOVS10

rem Check for basic requirements for Visual Studio 2008
if "%VS90COMNTOOLS%" == "" (
	echo ERROR: Microsoft Visual Studio 2008 is not installed.
	pause
	goto END
)

set PROJECT_TYPE=vs9
set VSCOMMONTOOLS=%VS90COMNTOOLS%vsvars32.bat

:STARTVS

rem Setup common Win32 environment variables

rem Get Version numbers

call:getvmajFunc "evil"
set evilVMAJ=%VersionMajor%
set evilVMIN=%VersionMinor%
set evilVMIC=%VersionMicro%

call:getvmajFunc "eina"
set einaVMAJ=%VersionMajor%
set einaVMIN=%VersionMinor%
set einaVMIC=%VersionMicro%

call:getvmajFunc "eet"
set eetVMAJ=%VersionMajor%
set eetVMIN=%VersionMinor%
set eetVMIC=%VersionMicro%

call:getvmajFunc "evas"
set evasVMAJ=%VersionMajor%
set evasVMIN=%VersionMinor%
set evasVMIC=%VersionMicro%

call:getvmajFunc "ecore"
set ecoreVMAJ=%VersionMajor%
set ecoreVMIN=%VersionMinor%
set ecoreVMIC=%VersionMicro%

call:getvmajFunc "embryo"
set embryoVMAJ=%VersionMajor%
set embryoVMIN=%VersionMinor%
set embryoVMIC=%VersionMicro%

rem Add installation directory pathes.

set INCLUDE=%EXT_DIR%\include;%INCLUDE%
set LIB=%EXT_DIR%\lib;%LIB%

set INCLUDE_EVIL=%cd%\..\e17\evil\src\lib;%cd%\..\e17\evil\src\lib\sys;%cd%\evil\common;%INCLUDE_EVIL%

set INCLUDE_EINA=%EXT_DIR%\include\evil-%evilVMAJ%;%INCLUDE_EINA%
set INCLUDE_EINA=%cd%\..\e17\eina\src\include;%cd%\..\e17\eina\src\lib;%cd%\eina\common;%INCLUDE_EINA%

set INCLUDE_EET=%EXT_DIR%\include\evil-%evilVMAJ%;%EXT_DIR%\include\eina-%einaVMAJ%;%EXT_DIR%\include\eina-1\eina;%INCLUDE_EET%
set INCLUDE_EET=%cd%\..\e17\eet\src\lib;%cd%\eet\common;%INCLUDE_EET%

set INCLUDE_EVAS_DX=%DXSDK_DIR%Include;%INCLUDE_EVAS_DX%
set LIB_EVAS_DX=%DXSDK_DIR%Lib\x86;%LIB_EVAS_DX%

set INCLUDE_EVAS=%EXT_DIR%\include\evil-%evilVMAJ%;%EXT_DIR%\include\eina-%einaVMAJ%;%EXT_DIR%\include\eina-1\eina;%EXT_DIR%\include\eet-%eetVMAJ%;%EXT_DIR%\include\freetype2;%INCLUDE_EVAS%
set INCLUDE_EVAS=%cd%\..\e17\evas\src\lib;%cd%\..\e17\evas\src\lib\include;%cd%\..\e17\evas\src\lib\file;%cd%\..\e17\evas\src\lib\cserve;%cd%\evas\common;%INCLUDE_EVAS%

set INCLUDE_ECORE=%EXT_DIR%\include\evil-%evilVMAJ%;%EXT_DIR%\include\eina-%einaVMAJ%;%EXT_DIR%\include\eina-1\eina;%EXT_DIR%\include\evas-%evasVMAJ%;%INCLUDE_ECORE%
set INCLUDE_ECORE=%cd%\..\e17\ecore\src\lib\ecore;%cd%\..\e17\ecore\src\lib\ecore_evas;%cd%\..\e17\ecore\src\lib\ecore_input;%cd%\..\e17\ecore\src\lib\ecore_input_evas;%cd%\..\e17\ecore\src\lib\ecore_win32;%cd%\ecore\common;%INCLUDE_ECORE%

set INCLUDE_EMBRYO=%EXT_DIR%\include\evil-%evilVMAJ%;%INCLUDE_EMBRYO%
set INCLUDE_EMBRYO=%cd%\..\e17\embryo\src\lib;%cd%\embryo\common;%INCLUDE_EMBRYO%

set SolutionDirectory=%cd%\%PROJECT_TYPE%
set DebugOutputDirectory=%SolutionDirectory%\out_debug
set ReleaseOutputDirectory=%SolutionDirectory%\out
set DebugLibraryDirectory=%SolutionDirectory%\out_debug
set ReleaseLibraryDirectory=%SolutionDirectory%\out
set TemporaryDirectory=%SolutionDirectory%\temp

rem Setting environment for using Microsoft Visual Studio x86 tools.
call "%VSCOMMONTOOLS%"

efl.sln

goto END

:getvmajFunc

rem Extract version_major from configure.ac
set VersionMajor=0
for /F "TOKENS=*" %%a in ('findstr /R /C:"^m4_define.*v_maj]" ..\e17\%~1\configure.ac') do set VersionMajor=%%a
for /F "TOKENS=1,2 delims=, " %%a in ("%VersionMajor%") do set left=%%a&set right=%%b
set VersionMajor=%right:~0,-1%
set VersionMajor=%VersionMajor:[=;%
set VersionMajor=%VersionMajor:]=;%
for /F "TOKENS=1 delims=;" %%a in ("%VersionMajor%") do set VersionMajor=%%a

rem Extract version_minor from configure.ac
set VersionMinor=0
for /F "TOKENS=*" %%a in ('findstr /R /C:"^m4_define.*v_min]" ..\e17\%~1\configure.ac') do set VersionMinor=%%a
for /F "TOKENS=1,2 delims=, " %%a in ("%VersionMinor%") do set left=%%a&set right=%%b
set VersionMinor=%right:~0,-1%
set VersionMinor=%VersionMinor:[=;%
set VersionMinor=%VersionMinor:]=;%
for /F "TOKENS=1 delims=;" %%a in ("%VersionMinor%") do set VersionMinor=%%a

rem Extract version_micro from configure.ac
set VersionMicro=0
for /F "TOKENS=*" %%a in ('findstr /R /C:"^m4_define.*v_mic]" ..\e17\%~1\configure.ac') do set VersionMicro=%%a
for /F "TOKENS=1,2 delims=, " %%a in ("%VersionMicro%") do set left=%%a&set right=%%b
set VersionMicro=%right:~0,-1%
set VersionMicro=%VersionMicro:[=;%
set VersionMicro=%VersionMicro:]=;%
for /F "TOKENS=1 delims=;" %%a in ("%VersionMicro%") do set VersionMicro=%%a

goto:eof

:END
