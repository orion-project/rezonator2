::
:: Build application manual.
::

@echo off
setlocal

where /Q qhelpgenerator
if %ERRORLEVEL% neq 0 (
	echo.
	echo ERROR: qhelpgenerator is not found in PATH
	echo Find Qt installation and update your PATH like:
    echo set PATH=c:\Qt\5.12.0\mingw73_64\bin;%%PATH%%
	goto :eof
)
qhelpgenerator -v

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

set SOURCE_DIR=.\help
set TARGET_DIR=.\out\help
set BIN_DIR=.\bin

::python -m sphinx -b html %SOURCE_DIR% %TARGET_DIR%
::goto :eof

python -m sphinx -b qthelp %SOURCE_DIR% %TARGET_DIR%

copy /Y %SOURCE_DIR%\rezonator.qhcp %TARGET_DIR%

qhelpgenerator %TARGET_DIR%\rezonator.qhcp

move %TARGET_DIR%\rezonator.qch %BIN_DIR%
move %TARGET_DIR%\rezonator.qhc %BIN_DIR%

assistant -collectionFile %BIN_DIR%\rezonator.qhc -style fusion

:eof
