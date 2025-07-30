@echo off
setlocal EnableDelayedExpansion

echo Build application manual.


set HELP_TOOL=qhelpgenerator.exe

echo.
echo ***** Check if Qt is in PATH
where /Q %HELP_TOOL%
if %ERRORLEVEL% neq 0 (
    set HELP_TOOL=qcollectiongenerator.exe
    echo %HELP_TOOL% not found, try !HELP_TOOL!...

    where /Q !HELP_TOOL!
    if !ERRORLEVEL! neq 0 (
        echo.
        echo ERROR: %HELP_TOOL% or !HELP_TOOL! not found in PATH
        echo Find Qt installation and update your PATH like:
        echo set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%%PATH%%
        goto :eof
    )
)
%HELP_TOOL% -v


set SCRIPT_DIR=%~dp0
set SOURCE_DIR=%SCRIPT_DIR%
set TARGET_DIR=%SCRIPT_DIR%\..\out\help
set BIN_DIR=%SCRIPT_DIR%\..\bin


echo.
echo ***** Building html files...
python -m sphinx -b qthelp %SOURCE_DIR% %TARGET_DIR%
if %ERRORLEVEL% neq 0 goto :eof



echo.
echo ***** Building qt-help files...
copy /Y %SOURCE_DIR%\rezonator.qhcp %TARGET_DIR%
%HELP_TOOL% %TARGET_DIR%\rezonator.qhcp
if %ERRORLEVEL% neq 0 goto :eof



echo.
echo ***** Copy built help files to bin dir...
copy %TARGET_DIR%\rezonator.qch %BIN_DIR%
copy %TARGET_DIR%\rezonator.qhc %BIN_DIR%
if %ERRORLEVEL% neq 0 goto :eof



echo.
echo ***** Checking Assistant app...
for %%G in ("%path:;=" "%") do (
    if exist %%G"\%HELP_TOOL%" (
        set HELP_TOOL_DIR=%%G
        goto :qt_dir_found
    )
)
echo ERROR: unable to locate %HELP_TOOL% and get Assistant path
goto :eof

:qt_dir_found
@REM set ASSISTANT_SOURCE=%HELP_TOOL_DIR%\assistant.exe
@REM echo Source Assistant path is %ASSISTANT_SOURCE%

@REM set ASSISTANT_TARGET=%BIN_DIR%\assistant.exe
@REM echo Target Assistant path is %ASSISTANT_TARGET%
@REM if not exist "%ASSISTANT_TARGET%" (
@REM     echo Copy Assistant app to bin dir...
@REM     copy %ASSISTANT_SOURCE% %BIN_DIR%
@REM ) else (
@REM     echo Already there
@REM )



echo.
echo ***** Running Assistant...
@REM %BIN_DIR%\assistant -collectionFile %BIN_DIR%\rezonator.qhc -style fusion
cd %HELP_TOOL_DIR%
assistant.exe -collectionFile %BIN_DIR%\rezonator.qhc -style fusion


echo.
echo ***** Done
echo.

:eof
