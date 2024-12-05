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

rem --- locate the Vulkan latest SDK installer and specify installation directory
set "sdk_url=https://sdk.lunarg.com/sdk/download/latest/windows/vulkan_sdk.exe"
set "curr_dir=%~dp0"

rem -------------------- installation directory --------------------
set "install_dir=%curr_dir%..\..\engine\external"
if not exist %install_dir% mkdir %install_dir%

rem --- convert to absolute path 
pushd %install_dir%
set "install_dir=%CD%"
popd

rem -------------------- installer filepath --------------------
set "sdk_installer=%install_dir%\vulkan_installer.exe"

rem --- download and force extract vulkan executable
echo %sdk_installer%
curl -L %sdk_url% > %sdk_installer%
echo "Vulkan SDK Installer download completed."

rem --- run the installer
set "extract_dir=%install_dir%\vulkan"
if not exist %extract_dir% mkdir %extract_dir%
start /b /wait "" %sdk_installer% --root %extract_dir% --accept-licenses --default-answer --confirm-command install com.lunarg.vulkan.volk
echo "Vulkan SDK installation completed."

rem -- set environment variables
setx /m VK_SDK_PATH %extract_dir%
setx /m VK_INSTANCE_LAYERS VK_LAYER_KHRONOS_validation
setx /m VK_LAYER_PATH %extract_dir%\Bin
echo "Environment variables set successfully."

rem --- cleanup
del %sdk_installer%
echo "Cleanup completed."

echo "Vulkan SDK installation completed."
pause

