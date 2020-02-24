@echo off

echo Build application manual (HTML version).

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

set SOURCE_DIR=.\help
set TARGET_DIR=.\out\help_html

echo.
echo ***** Building html files...
python -m sphinx -b html %SOURCE_DIR% %TARGET_DIR%
if %ERRORLEVEL% neq 0 goto :eof

echo.
echo ***** Done
echo.

:eof
