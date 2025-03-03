@echo off
setlocal

rem --- Vytal Engine ---
rem this batch script helps setting up Vytal Engine (bindings, etc.)

:: BatchGotAdmin
:-------------------------------------
rem check for permissions
if "%PROCESSOR_ARCHITECTURE%" equ "amd64" (
	>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) else (
	>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

rem if error flag set, we do not have admin
if '%errorlevel%' neq '0' (
	echo Requesting administrative privileges...
	goto UACPrompt
) else ( 
	goto gotAdmin 
)

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "%~s0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"
    "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    if exist "%temp%\getadmin.vbs" ( del "%temp%\getadmin.vbs" )
:-------------------------------------

set "VYTAL_MAIN_PATH=%~dp0"
if "%VYTAL_MAIN_PATH:~-1%"=="\" set "VYTAL_MAIN_PATH=%VYTAL_MAIN_PATH:~0,-1%"

rem set environment variable for Vytal Engine workspace
setx /m VYTAL_ENGINE_PATH "%VYTAL_MAIN_PATH%\engine"
echo Environment variable for workspace 'Vytal Engine' is set.

rem set environment variable for Vytal Editor workspace
setx /m VYTAL_EDITOR_PATH "%VYTAL_MAIN_PATH%\editor"
echo Environment variable for workspace 'Vytal Editor' is set.

rem force reloading the variables in the same script
for /f "tokens=2,* delims= " %%A in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v VYTAL_EDITOR_PATH') do set "VYTAL_EDITOR_PATH=%%B"

rem check if the variable is set properly
if "%VYTAL_EDITOR_PATH%"=="" (
    echo Error: VYTAL_EDITOR_PATH is not set.
    echo Please restart your command prompt and try again.
    exit /b 1
) else (
    echo VYTAL_EDITOR_PATH is now available: %VYTAL_EDITOR_PATH%
)

endlocal
pause