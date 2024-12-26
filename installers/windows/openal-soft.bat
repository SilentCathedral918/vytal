@echo off

:: BatchGotAdmin
:-------------------------------------
rem --- check for permissions
if "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
	>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) 
else (
	>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

rem --- if error flag set, we do not have admin
if '%errorlevel%' NEQ '0' (
	echo Requesting administrative privileges...
	goto UACPrompt
) 
else ( 
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

rem --- locate openal-soft 1.24.1 binaries zip and specify installation directory
set "bin_url=https://github.com/kcat/openal-soft/releases/download/1.24.1/openal-soft-1.24.1-bin.zip"
set "curr_dir=%~dp0"

rem -------------------- installation directory --------------------
set "dest_dir=%curr_dir%..\..\engine\external"
if not exist %dest_dir% mkdir %dest_dir%

rem --- convert to absolute path 
pushd %dest_dir%
set "dest_dir=%CD%"
popd

rem -------------------- filepath for extraction --------------------
set "extract_dir=%dest_dir%\openal-soft\"

rem -------------------- download openal-soft binaries --------------------
echo "Downloading openal-soft binaries..."
curl -L "%bin_url%" -o "%temp%\openal-soft.zip"
if %errorlevel% NEQ 0 (
    echo "Failed to download openal-soft binaries."
    exit /b 1
)

rem -------------------- extract openal-soft binaries zip --------------------
echo "Extracting openal-soft binaries..."
powershell -Command "Expand-Archive -Path '%temp%\openal-soft.zip' -DestinationPath '%extract_dir%' -Force"
if %errorlevel% NEQ 0 (
    echo "Failed to extract openal-soft binaries."
    exit /b 1
)

rem ---------------------- temp zip folder cleanup ----------------------
del "%temp%\openal-soft.zip"

rem --- base path where openal-soft is extracted
set "extract_base_dir=%dest_dir%\openal-soft\openal-soft-1.24.1-bin"

rem --- move all contents from openal-soft-1.24.1-bin to its parent directory for clean aesthetic purpose
robocopy %extract_base_dir% "%dest_dir%\openal-soft" /move /s

rem --- architecture-specific folder mapping
set "bin32_folder=bin\Win32"
set "bin64_folder=bin\Win64"
set "lib32_folder=libs\Win32"
set "lib64_folder=libs\Win64"
set "rt32_folder=router\Win32"
set "rt64_folder=router\Win64"

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" ( goto handle_64bit ) else ( goto handle_32bit )

:handle_32bit
echo "The architecture is 32-bit. Removing 64-bit 'bin' and 'libs' folders"

rmdir /s /q "%dest_dir%\openal-soft\%bin64_folder%"
rmdir /s /q "%dest_dir%\openal-soft\%lib64_folder%"
rmdir /s /q "%dest_dir%\openal-soft\%rt64_folder%"

robocopy %dest_dir%\openal-soft\%bin32_folder% "%dest_dir%\openal-soft\bin" /move /s
robocopy %dest_dir%\openal-soft\%lib32_folder% "%dest_dir%\openal-soft\libs" /move /s
robocopy %dest_dir%\openal-soft\%rt32_folder% "%dest_dir%\openal-soft\router" /move /s

goto finalize

:handle_64bit
echo "The architecture is 64-bit. Removing 32-bit 'bin' and 'libs' folders"

rmdir /s /q "%dest_dir%\openal-soft\%bin32_folder%"
rmdir /s /q "%dest_dir%\openal-soft\%lib32_folder%"
rmdir /s /q "%dest_dir%\openal-soft\%rt32_folder%"

robocopy %dest_dir%\openal-soft\%bin64_folder% "%dest_dir%\openal-soft\bin" /move /s
robocopy %dest_dir%\openal-soft\%lib64_folder% "%dest_dir%\openal-soft\libs" /move /s
robocopy %dest_dir%\openal-soft\%rt64_folder% "%dest_dir%\openal-soft\router" /move /s

goto finalize

:finalize
rem --- set environment variables
setx OPENAL_SOFT_PATH "%dest_dir%\openal-soft" /m

echo "openal-soft installation completed."
pause
