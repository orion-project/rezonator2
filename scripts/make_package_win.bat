::
:: Create redistributable package.
::

@echo off
setlocal

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
echo ***** Qt dir is %QT_DIR%

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

echo ***** Create out dir if none
if not exist "out\" mkdir out
cd out

echo ***** Recreate target dir if already exists
if exist "redist\" rmdir /S /Q redist
mkdir redist
cd redist

echo ***** Running windeployqt...
windeployqt ..\..\bin\rezonator.exe --dir . --no-translations --no-system-d3d-compiler --no-opengl-sw

echo ***** Copy additional files which are ignored by windeployqt for some reason
copy /Y %QT_DIR%\libgcc_s_seh-1.dll
copy /Y %QT_DIR%\"libstdc++-6.dll"
copy /Y %QT_DIR%\libwinpthread-1.dll
if exist libEGL.dll del libEGL.dll
if exist libGLESV2.dll del libGLESV2.dll

echo ***** Copying project files...
copy /Y ..\..\bin\rezonator.exe
copy /Y ..\..\bin\rezonator_test.bat
copy /Y ..\..\bin\rezonator_test_nogui.bat
if not exist "examples\" mkdir examples
if not exist "test_files\" mkdir test_files
copy /Y ..\..\bin\examples\*.* examples
copy /Y ..\..\bin\test_files\*.* test_files
