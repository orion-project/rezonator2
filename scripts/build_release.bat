::
:: Build release version of the application.
::

@echo off
setlocal

where /Q mingw32-make
if %ERRORLEVEL% neq 0 (
	echo.
	echo ERROR: mingw32-make is not found in PATH
	echo Ensure you have MinGW installed and update PATH, e.g.:
    echo set PATH=C:\Qt\Tools\mingw530_32\bin;%%PATH%%
	goto :eof
)
where /Q qmake
if %ERRORLEVEL% neq 0 (
	echo.
	echo ERROR: qmake is not found in PATH
	echo Find Qt installation and update your PATH like:
    echo set PATH=c:\Qt\5.10.1\mingw53_32\bin;%%PATH%%
	goto :eof
)
mingw32-make --version
qmake -v

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..\out

echo.
echo Updating version number...
python ..\release\make_version.py

echo.
echo Building...
if exist BuildDir rmdir /S /Q BuildDir
mkdir BuildDir
cd BuildDir
qmake -config release ..\..\rezonator.pro
mingw32-make
