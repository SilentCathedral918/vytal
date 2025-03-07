@echo off
rem --- Create new Vytal Project CLI ---
rem this batch script helps create a new Vytal game project

rem command line arguments
set PROJECT_NAME=
set PROJECT_PATH=
set VERBOSE=false
set FORCE=false

rem tab-indent
set "TAB=    "

rem ----------------------------------------------------------------------------------------------

rem process argument
:process_args

rem no more argument, proceed to project construction
if "%1"=="" goto construct_project

rem parse project name
if "%1"=="--project_name" (
	set PROJECT_NAME=%2
	shift
	shift
	goto process_args 
)

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

rem parse force-overwrite setting
if "%1"=="--force" (
	set FORCE=true
	shift
	goto process_args
)

rem ----------------------------------------------------------------------------------------------

rem make sure that VYTAL_EDITOR_PATH is set
if "%VYTAL_EDITOR_PATH%"=="" (
    echo Error: VYTAL_EDITOR_PATH is not set.
    exit /b 1
)

rem ----------------------------------------------------------------------------------------------

rem construct new Vytal project
:construct_project

rem validate project name
if "%PROJECT_NAME%"=="" (
	echo Error: Project name is required.
	exit /b 1  
)

rem validate project path
if "%PROJECT_PATH%"=="" (
	echo Error: Filepath for Vytal project is required.
	exit /b 1  
)

rem output verbose log if enabled
if "%VERBOSE%"=="true" (
	echo Verbose mode enabled.
)

rem check if folder already exists, with handling --force flag
if exist "%PROJECT_PATH%\%PROJECT_NAME%" (
	if "%FORCE%"=="false" (
		echo Error: Project directory '%PROJECT_PATH%\%PROJECT_NAME%' already exists. Use --force to overwrite.
		exit /b 1 
	) else (
		echo Warning: overwriting existing project directory.
		rmdir /s /q "%PROJECT_PATH%\%PROJECT_NAME%"
	)
)

rem ----------------------------------------------------------------------------------------------

rem convert project base filepath to absolute format
for %%i in ("%PROJECT_PATH%") do set "PROJECT_PATH=%%~fi"

rem ----------------------------------------------------------------------------------------------

rem construct project folder structure
echo Constructing project folder structure...
mkdir "%PROJECT_PATH%\%PROJECT_NAME%\src"
mkdir "%PROJECT_PATH%\%PROJECT_NAME%\assets"
mkdir "%PROJECT_PATH%\%PROJECT_NAME%\configs"

rem construct editor output log folder structure if not exist
echo Constructing editor output log folder structure...
if not exist "%VYTAL_EDITOR_PATH%\saved\logs" mkdir "%VYTAL_EDITOR_PATH%\saved\logs"

rem generate a default .vtproj file
echo Constructing default project file: %PROJECT_NAME%.vtproj
echo # Project information > "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo project_name = "%PROJECT_NAME%" >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo version = "1.0" >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo modules = [] >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"

rem project base folder filepath
echo # Paths >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo base_path = "%PROJECT_PATH%\%PROJECT_NAME%" >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"

rem project sub-folder filepaths
echo [paths] >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo source = "src" >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo assets = "assets" >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo config = "configs" >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.vtproj"

if "%VERBOSE%"=="true" (
    echo %PROJECT_NAME%.vtproj construction completed.
)

rem ----------------------------------------------------------------------------------------------

rem setup editor configurations
echo Setting up default editor configurations...

echo # General settings > "%PROJECT_PATH%\%PROJECT_NAME%\configs\editor.cfg"
echo [general] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\editor.cfg"
echo title = "vytal_editor/%PROJECT_NAME%" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\editor.cfg"
echo auto_play_on_launch = true >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\editor.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\editor.cfg"

rem ----------------------------------------------------------------------------------------------

rem setup engine configurations
echo Setting up default engine configurations...

rem memory zones
echo # Engine memory zones configuration >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo [memory_zones] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo core = "4KB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo modules = "4KB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo containers = "8MB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo strings = "8MB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo delegates = "8KB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo input = "4KB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo platform = "2KB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo assets = "256MB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo renderer = "64MB" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"

echo # Loggers configuration >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # format: ^<logger_name^> = ^<log_flags^> -^> ^<output_log_filepath^> >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   0000 0001 -^> includes timestamp >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   0000 0010 -^> includes file and line >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   0000 0100 -^> includes function name >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo [loggers] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo VYTAL_ENGINE = 00000111 -^> "%VYTAL_EDITOR_PATH%\saved\logs\vytal_engine_log.txt" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo VYTAL_EDITOR = 00000111 -^> "%VYTAL_EDITOR_PATH%\saved\logs\vytal_editor_log.txt" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"

echo # Input general configuration >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # mouse_sensitivity: determines how fast the camera moves when mouse moves >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # invert_y_axis: (true = mouse up -^> camera up; false = mouse down -^> camera down) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # key_repeat_delay: delay before key repeat starts (ms) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # key_repeat_rate: key repeat rate after held down (ms per repeat) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo [input.general] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo mouse_sensitivity = 1.0 >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo invert_y_axis = false >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo key_repeat_delay = 250 >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo key_repeat_rate = 30 >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"

echo # Input bindings configuration >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo [input.bindings] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo move_forward = "w" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo move_backward = "s" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo move_left = "a" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo move_right = "d" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo jump = "space" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo crouch = "ctrl" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo sprint = "shift" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"

echo # Window properties configuration >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # x, y: position for the window (RANDOM_POSITION = let OS determine the position, otherwise place on specified position) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # width, height: resolution for the window (in pixels) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # window_mode: defines how the window is displayed >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "windowed": regular resizable window >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "fullscreen": takes up the entire screen >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "borderless": fullscreen window without borders >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # title_bar_flags: display flags for window title bar >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   0000 0001 -^> includes ^title >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   0000 0010 -^> includes frame-time (in ms) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   0000 0100 -^> includes draw-time (in ms) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # vsync: toggles vertical sync; enabled vsync helps reduce screen tearing >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # backend: selects window backend >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "glfw": GLFW backend (default, supported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "sdl2": SDL backend (unsupported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "win32": Win32 backend (unsupported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "vulkan": Vulkan WSI backend (unsupported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo [window] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo x = RANDOM_POSITION >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo y = RANDOM_POSITION >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo width = 1024 >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo height = 768 >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo window_mode = "windowed" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo title_bar_flags = 00000111 >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo vsync = on >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo backend = "glfw" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"

echo # Renderer properties configuration >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo # backend: selects renderer backend >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "vulkan": Vulkan backend (default, supported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "opengl": OpenGL backend (unsupported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo #   "directx": DirectX backend (unsupported) >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo [renderer] >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo backend = "vulkan" >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"
echo. >> "%PROJECT_PATH%\%PROJECT_NAME%\configs\engine.cfg"


if "%VERBOSE%"=="true" (
    echo Default engine configurations setup completed.
)

rem ----------------------------------------------------------------------------------------------

rem instructions
echo Project %PROJECT_NAME% constructed at path: %PROJECT_PATH%\%PROJECT_NAME%.
echo You may now open the project with '%PROJECT_NAME%.vtproj'.

if "%VERBOSE%"=="true" (
    echo Project construction completed successfully.
)

rem ----------------------------------------------------------------------------------------------
exit /b 0