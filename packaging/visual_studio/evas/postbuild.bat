REM THIS SCRIPT IS USED TO COPY EVAS FILES TO EXTRERN DIRECTORY
REM Date: 2010/12/15, Authors: forumer@smartmobili.com, Vincent Torri
REM ---------------       %1              %2          %3         %4    -----
REM call %POSTBUILD $(ProjectType)  $(TargetDir)  $(LibName)   $(VMAJ)
REM ---------------       %1              %2          %3         %4    -----
REM

@echo off

rem Unpatching Evas

echo Unpatching Evas...
..\patch.exe -R -d ..\..\e17\evas -p0 < evas_vs.diff

rem Set generic variables

set PROJECT_TYPE=%~1
set EXT_DIR=%~2
set LIB_NAME=%~3
set VMAJ=%~4

set INC_DIR_LIB=evas-%VMAJ%

set EXT_DIR_BIN=%EXT_DIR%\bin
set EXT_DIR_LIB=%EXT_DIR%\lib
set EXT_DIR_INC=%EXT_DIR%\include
set EXT_DIR_INC_LIB=%EXT_DIR%\include\%INC_DIR_LIB%
set LIB_NAME_DLL=%LIB_NAME%-%VMAJ%.dll
set LIB_NAME_LIB=%LIB_NAME%.lib

if not exist %EXT_DIR_BIN% (
	MKDIR %EXT_DIR_BIN%
)

if not exist %EXT_DIR_LIB%\evas\modules\engines\buffer\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\engines\buffer\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\engines\software_ddraw\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\engines\software_ddraw\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\engines\software_gdi\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\engines\software_gdi\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\engines\software_generic\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\engines\software_generic\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\eet\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\eet\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\gif\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\gif\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\jpeg\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\jpeg\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\pmaps\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\pmaps\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\png\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\png\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\tiff\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\tiff\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\loaders\xpm\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\loaders\xpm\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\savers\eet\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\savers\eet\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\savers\png\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\savers\png\win32-msc-x86
)

if not exist %EXT_DIR_LIB%\evas\modules\savers\tiff\win32-msc-x86 (
	MKDIR %EXT_DIR_LIB%\evas\modules\savers\tiff\win32-msc-x86
)

if not exist %EXT_DIR_INC_LIB% (
	MKDIR %EXT_DIR_INC_LIB%
)

echo Copying Evas files...
COPY /Y "..\%PROJECT_TYPE%\out_debug\%LIB_NAME_DLL%" "%EXT_DIR_BIN%"

COPY /Y "..\%PROJECT_TYPE%\out_debug\%LIB_NAME_LIB%" "%EXT_DIR_LIB%"

COPY /Y "..\%PROJECT_TYPE%\out_debug\engines\buffer\module.dll" "%EXT_DIR_LIB%\evas\modules\engines\buffer\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\engines\software_gdi\module.dll" "%EXT_DIR_LIB%\evas\modules\engines\software_gdi\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\engines\software_generic\module.dll" "%EXT_DIR_LIB%\evas\modules\engines\software_generic\win32-msc-x86"

COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\eet\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\eet\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\gif\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\gif\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\jpeg\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\jpeg\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\pmaps\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\pmaps\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\png\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\png\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\tiff\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\tiff\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\loaders\xpm\module.dll" "%EXT_DIR_LIB%\evas\modules\loaders\xpm\win32-msc-x86"

COPY /Y "..\%PROJECT_TYPE%\out_debug\savers\eet\module.dll" "%EXT_DIR_LIB%\evas\modules\savers\eet\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\savers\png\module.dll" "%EXT_DIR_LIB%\evas\modules\savers\png\win32-msc-x86"
COPY /Y "..\%PROJECT_TYPE%\out_debug\savers\tiff\module.dll" "%EXT_DIR_LIB%\evas\modules\savers\tiff\win32-msc-x86"

COPY /Y "..\..\e17\evas\src\lib\Evas.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evas\src\modules\engines\buffer\Evas_Engine_Buffer.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evas\src\modules\engines\software_gdi\Evas_Engine_Software_Gdi.h" "%EXT_DIR_INC_LIB%"
COPY /Y "..\..\e17\evas\src\modules\engines\software_ddraw\Evas_Engine_Software_DDraw.h" "%EXT_DIR_INC_LIB%"
