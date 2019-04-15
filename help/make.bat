::
:: Build application manual.
::

@echo off
setlocal

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

python3 -m sphinx -b html .\help .\out\help
