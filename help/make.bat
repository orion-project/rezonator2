::
:: Build application manual.
::

@echo off
setlocal

set SCRIPT_DIR=%~dp0
cd %SCRIPT_DIR%\..

echo ***** Check if Sphinx is in PATH
sphinx-build >NUL 2>NUL
if errorlevel 9009 (
	echo.
	echo ERROR: sphinx-build command was not found. Make sure you have Sphinx installed
	exit /b 1
)

sphinx-build -b html .\help .\out\help
