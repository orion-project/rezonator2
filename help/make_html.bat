@echo off

echo Build application manual (HTML version).

set SCRIPT_DIR=%~dp0
set SOURCE_DIR=%SCRIPT_DIR%
set TARGET_DIR=%SCRIPT_DIR%\..\out\help_html

echo.
echo ***** Building html files...
python -m sphinx -b html %SOURCE_DIR% %TARGET_DIR%
if %ERRORLEVEL% neq 0 goto :eof

echo.
echo ***** Done
echo.

:eof
