@echo off
setlocal

echo Create redistributable package.


echo ***** Check if Qt is in PATH
where /Q windeployqt.exe
if %ERRORLEVEL% neq 0 (
	echo.
	echo ERROR: windeployqt.exe is not found in PATH
	echo Find Qt installation and update your PATH like:
    echo set PATH=c:\Qt\5.12.0\mingw73_64\bin;%%PATH%%
	goto :eof
)
echo ***** Find a dir where windeployqt is located
for %%G in ("%path:;=" "%") do (
    if exist %%G"\windeployqt.exe" (
	    set QT_DIR=%%G
		goto :qt_dir_found
	)
)
echo.
echo ERROR: unable to locate windeployqt.exe and get Qt path
goto :eof
 
:qt_dir_found
echo Qt dir is %QT_DIR%

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

echo.
echo ***** Create out dir if none
if not exist "out\" mkdir out
cd out

echo.
echo ***** Recreate target dir if already exists
if exist "redist\" rmdir /S /Q redist
mkdir redist
cd redist
if %ERRORLEVEL% neq 0 goto :eof


set BIN_DIR=..\..\bin


echo.
echo ***** Running windeployqt...
windeployqt %BIN_DIR%\rezonator.exe --dir . --no-translations --no-system-d3d-compiler --no-opengl-sw
windeployqt %QT_DIR%\assistant.exe --dir . --no-translations --no-system-d3d-compiler --no-opengl-sw
if %ERRORLEVEL% neq 0 goto :eof


echo.
echo ***** Copy additional files ignored by windeployqt...
copy %QT_DIR%\assistant.exe
copy %QT_DIR%\libgcc_s_seh-1.dll
copy %QT_DIR%\"libstdc++-6.dll"
copy %QT_DIR%\libwinpthread-1.dll
if %ERRORLEVEL% neq 0 goto :eof


echo.
echo ***** Clean some excessive files...
if exist libEGL.dll del libEGL.dll
if exist libGLESV2.dll del libGLESV2.dll
if exist sqldrivers\qsqlmysql.dll del sqldrivers\qsqlmysql.dll
if exist sqldrivers\qsqlodbc.dll del sqldrivers\qsqlodbc.dll
if exist sqldrivers\qsqlpsql.dll del sqldrivers\qsqlpsql.dll
if exist imageformats\qicns.dll del imageformats\qicns.dll
if exist imageformats\qtga.dll del imageformats\qtga.dll
if exist imageformats\qtiff.dll del imageformats\qtiff.dll
if exist imageformats\qwbmp.dll del imageformats\qwbmp.dll
if exist imageformats\qwebp.dll del imageformats\qwebp.dll
if %ERRORLEVEL% neq 0 goto :eof


echo.
echo ***** Copy project files...
copy %BIN_DIR%\rezonator.exe
copy %BIN_DIR%\rezonator_test.bat
copy %BIN_DIR%\rezonator_test_nogui.bat
mkdir examples
mkdir test_files
copy %BIN_DIR%\examples\*.* examples
copy %BIN_DIR%\test_files\*.* test_files
copy %BIN_DIR%\rezonator.qch
copy %BIN_DIR%\rezonator.qhc
if %ERRORLEVEL% neq 0 goto :eof


echo.
echo ***** Done
echo.

:eof
