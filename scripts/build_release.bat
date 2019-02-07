::
:: Build release version of the application.
::

@echo off
setlocal

echo ***** Check if Qt is in PATH
where /Q mingw32-make
if %ERRORLEVEL% neq 0 (
	echo.
	echo ERROR: mingw32-make is not found in PATH
	echo Ensure you have MinGW installed and update PATH, e.g.:
    echo set PATH=c:\Qt\Tools\mingw730_64\bin;%%PATH%%
	goto :eof
)
where /Q qmake
if %ERRORLEVEL% neq 0 (
	echo.
	echo ERROR: qmake is not found in PATH
	echo Find Qt installation and update your PATH like:
    echo set PATH=c:\Qt\5.12.0\mingw73_64\bin;%%PATH%%
	goto :eof
)
mingw32-make --version
qmake -v

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

echo ***** Create out dir if none
if not exist "out\" mkdir out
cd out

echo.
echo ***** Building...
if exist release rmdir /S /Q release
mkdir release
cd release
qmake -config release ..\..\rezonator.pro
mingw32-make
