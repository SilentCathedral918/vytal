@echo off
rem --- Open existing Vytal Project CLI ---
rem this batch script helps open an existing Vytal game project

rem make sure that VYTAL_EDITOR_PATH is set
if "%VYTAL_EDITOR_PATH%"=="" (
    echo Error: VYTAL_EDITOR_PATH is not set.
    exit /b 1
)

rem command line arguments
set PROJECT_PATH=
set PROJECT_FILE=
set VERBOSE=false

rem ----------------------------------------------------------------------------------------------

rem process argument
:process_args

rem no more argument, proceed to project construction
if "%1"=="" goto check_project

rem parse project filepath
if "%1"=="--project_path" (
	set PROJECT_PATH=%2
	shift
	shift
	goto process_args
)

rem parse verbose setting
if "%1"=="--verbose" (
	set VERBOSE=true
	shift
	goto process_args
)

rem ----------------------------------------------------------------------------------------------

:check_project

rem check if project filepath is provided
if "%PROJECT_PATH%"=="" (
    echo Usage: open_existing_project.bat --project-path C:\path\to\project
    exit /b 1
)

rem check if project folder exists
if not exist "%PROJECT_PATH%" (
    echo Error: Project folder does not exist at path: %PROJECT_PATH%.
    exit /b 1
)

rem locate the .vtproj file
for /r "%PROJECT_PATH%" %%f in (*.vtproj) do (
    set PROJECT_FILE=%%f
    goto found_project
)

rem no project file was found
echo Error: No .vtproj file found at project path: %PROJECT_PATH%.
exit /b 1

rem ----------------------------------------------------------------------------------------------

:found_project

rem found the project file, report status (if verbose)
if "%VERBOSE%"=="true" (
    echo Found project file: %PROJECT_FILE%
    echo Opening Vytal Editor with %PROJECT_FILE%...
)

rem launch Vytal Editor
"%VYTAL_EDITOR_PATH%\bin\vytal_editor.exe" "%PROJECT_FILE%"
exit /b 0

