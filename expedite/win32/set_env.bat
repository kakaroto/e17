@echo off

rem Set external libraries directory.
set EXT_DIR=%cd%\..\..\..\..\extern

if not exist %EXT_DIR% (
	set EXT_DIR=
	echo ERROR: External libs dir is not set.
	pause
	goto END
)

rem Add installation directory pathes.
set INCLUDE=%EXT_DIR%\include;%INCLUDE%
set LIB=%EXT_DIR%\lib;%LIB%

rem Add Evil lib path
set EvilInclude=%cd%\..\..\proto\evil\src\lib
set EvilCommon=%cd%\..\..\proto\evil\win32\common
set EvilOut=%cd%\..\..\proto\evil\win32\%PROJECT_TYPE%\out

rem Add Evas lib path
set EvasOut=%cd%\..\..\evas\win32\%PROJECT_TYPE%\out
set EvasDirect3DInclude=%cd%\..\..\evas\src\modules\engines\direct3d
set EvasGLGlewInclude=%cd%\..\..\evas\src\modules\engines\gl_glew
set EvasDDrawInclude=%cd%\..\..\evas\src\modules\engines\software_ddraw
set EvasInclude=%EvasDDrawInclude%;%EvasGLGlewInclude%;%EvasDirect3DInclude%;%cd%\..\..\evas\src\lib

rem Add Eina lib path
set EinaInclude=%cd%\..\..\eina\src\include
set EinaCommon=%cd%\..\..\eina\win32\common
set EinaOut=%cd%\..\..\eina\win32\%PROJECT_TYPE%\out


set INCLUDE=%EvasInclude%;%EvilCommon%;%EvilInclude%;%EvilInclude%\dlfcn;%EvilInclude%\mman;%INCLUDE%
set INCLUDE=%EinaCommon%;%EinaInclude%;%INCLUDE%
set LIB=%EinaOut%;%EvasOut%;%EvilOut%;%LIB%

if "%DXSDK_DIR%" == "" (
	echo WARNING: Microsoft DirectX SDK is not installed.
	pause
	goto END
)

rem Add DirectX includes and libraries dirs.
set INCLUDE="%DXSDK_DIR%Include";%INCLUDE%
set LIB="%DXSDK_DIR%Lib\x86";%LIB%


:END
