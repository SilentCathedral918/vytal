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

:begin
rem --- locate GLFW 3.4 binaries zip and specify installation directory
set "bin_url=https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip"
set "curr_dir=%~dp0"

rem -------------------- installation directory --------------------
set "dest_dir=%curr_dir%..\..\engine\external"
if not exist %dest_dir% mkdir %dest_dir%

rem --- convert to absolute path 
pushd %dest_dir%
set "dest_dir=%CD%"
popd

rem -------------------- filepath for extraction --------------------
set "extract_dir=%dest_dir%\glfw\"

rem -------------------- download GLFW binaries --------------------
echo "Downloading GLFW binaries..."
curl -L "%bin_url%" -o "%temp%\glfw.zip"
if %errorlevel% NEQ 0 (
    echo "Failed to download GLFW binaries."
    exit /b 1
)

rem -------------------- extract GLFW binaries zip --------------------
echo "Extracting GLFW binaries..."
powershell -Command "Expand-Archive -Path '%temp%\glfw.zip' -DestinationPath '%extract_dir%' -Force"
if %errorlevel% NEQ 0 (
    echo "Failed to extract GLFW binaries."
    exit /b 1
)

rem ---------------------- temp zip folder cleanup ----------------------
del "%temp%\glfw.zip"

rem --- compiler-specific folder mapping
set "mingw_folder=lib-mingw-w64"
set "vc2013_folder=lib-vc2013"
set "vc2015_folder=lib-vc2015"
set "vc2017_folder=lib-vc2017"
set "vc2019_folder=lib-vc2019"
set "vc2022_folder=lib-vc2022"
set "ucrt_folder=lib-static-ucrt"

rem --- base path where GLFW is extracted
set "extract_base_dir=%dest_dir%\glfw\glfw-3.4.bin.WIN64"

goto prompt_ide_selection

:prompt_ide_selection
echo =====================================
echo Select Mode:
echo -------------------------------------
echo 1. GCC/Clang (MinGW)
echo 2. MSVC 12.0 (Visual Studio 2013)
echo 3. MSVC 14.0 (Visual Studio 2015)
echo 4. MSVC 14.1 (Visual Studio 2017)
echo 5. MSVC 14.2 (Visual Studio 2019)
echo 6. MSVC 14.3 (Visual Studio 2022)
echo 7. Universal C Runtime
echo =====================================
set /p choice="Enter your choice (1-7): "
rem --- set environment variables based on choice
if "%choice%"=="1" ( set "selected_folder=%mingw_folder%"
) else if "%choice%"=="2" ( set "selected_folder=%vc2013_folder%"
) else if "%choice%"=="3" ( set "selected_folder=%vc2015_folder%"
) else if "%choice%"=="4" ( set "selected_folder=%vc2017_folder%"
) else if "%choice%"=="5" ( set "selected_folder=%vc2019_folder%"
) else if "%choice%"=="6" ( set "selected_folder=%vc2022_folder%"
) else if "%choice%"=="7" ( set "selected_folder=%ucrt_folder%"
) else ( 
 cls 
 goto begin 
)
goto filepath_manip

:filepath_manip
rem --- verify that the selected folder exists
set "selected_path=%extract_base_dir%\%selected_folder%"
if not exist "%selected_path%" (
    echo "Selected folder does not exist: %selected_path%"
    exit /b 1
)

rem --- move all contents from glfw-3.4.bin.WIN64 to its parent directory for clean aesthetic purpose
robocopy %extract_base_dir% "%dest_dir%\glfw" /move /s

rem --- remove unecessary folders
for %%F in (%mingw_folder% %vc2013_folder% %vc2015_folder% %vc2017_folder% %vc2019_folder% %vc2022_folder% %ucrt_folder%) do (
    if not "%%F"=="%selected_folder%" (
        rmdir /s /q "%dest_dir%\glfw\%%F"
    )
)

rem --- create folder called 'lib'
set "lib_dir=%dest_dir%\glfw\lib"
if not exist "%lib_dir%" mkdir "%lib_dir%"

rem --- move files from the selected folder to the lib directory
move "%dest_dir%\glfw\%selected_folder%\*" "%lib_dir%"

rem --- delete the selected folder
rmdir /s /q "%dest_dir%\glfw\%selected_folder%"

rem --- set environment variables
setx /m GLFW_PATH %dest_dir%\glfw

goto exit

:exit
echo "GLFW download completed."
pause


