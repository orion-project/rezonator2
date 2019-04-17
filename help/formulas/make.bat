@echo off
setlocal

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..\..

set SOURCE_DIR=.\help\formulas
set TARGET_DIR=.\out\help_formulas

python -m sphinx -b html %SOURCE_DIR% %TARGET_DIR%
