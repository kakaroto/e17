@echo off

set Target=%cd%
set e17=%cd%\..\..\..\..
set Libs=%e17%
set Proto=%e17%\proto
set Extern=%e17%\..\..\extern

rem Copy externals
cd %Extern%\bin
xcopy freetype6.dll %Target%\ /Q /Y
xcopy jpeg62.dll %Target%\ /Q /Y
xcopy zlib1.dll %Target%\ /Q /Y
xcopy glew32.dll %Target%\ /Q /Y

rem Copy proto
cd %Proto%\evil\win32\vs8\out
xcopy libdl.dll %Target%\ /Q /Y
xcopy libevil.dll %Target%\ /Q /Y
xcopy libmman.dll %Target%\ /Q /Y

rem Copy libs
cd %Libs%\eina\win32\vs8\out
xcopy eina.dll %Target%\ /Q /Y

cd %Libs%\eet\win32\vs8\out
xcopy libeet.dll %Target%\ /Q /Y

cd %Libs%\evas\win32\vs8\out
xcopy libevas.dll %Target%\ /Q /Y

set Modules=%Libs%\evas\win32\vs8\out\modules
cd %Modules%\engines\direct3d
xcopy module.dll %Target%\evas\modules\engines\direct3d\mingw32-i686\ /Q /Y
cd %Modules%\engines\software_ddraw
xcopy module.dll %Target%\evas\modules\engines\software_ddraw\mingw32-i686\ /Q /Y
cd %Modules%\engines\software_generic
xcopy module.dll %Target%\evas\modules\engines\software_generic\mingw32-i686\ /Q /Y
cd %Modules%\engines\gl_glew
xcopy module.dll %Target%\evas\modules\engines\gl_glew\mingw32-i686\ /Q /Y
cd %Modules%\loaders\png
xcopy module.dll %Target%\evas\modules\loaders\png\mingw32-i686\ /Q /Y
