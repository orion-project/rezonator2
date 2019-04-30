@echo off
setlocal

echo Build release version of the application.


echo.
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


echo.
echo ***** Create out dir if none
if not exist "out\" mkdir out
cd out


echo.
echo ***** Building...
if exist build rmdir /S /Q build
mkdir build
cd build

set START_TIME=%time%

qmake -config release ..\..\rezonator.pro
mingw32-make


:: Yeah, Windows console is very handy for measuring times too...
:: https://stackoverflow.com/questions/673523/how-do-i-measure-execution-time-of-a-command-on-the-windows-command-line
:: The output will be wrong if your command runs for 24 hours or more.
set END_TIME=%time%
set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%START_TIME%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%END_TIME%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100
set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%
set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
::echo Finished in %hours%:%mins%:%secs%.%ms% (%totalsecs%.%ms%s total)

echo.
echo Finished in %totalsecs% seconds
echo.

:eof
