@echo off
setlocal EnableDelayedExpansion

set "CODEBASE=vytal_editor"

rem make sure that VYTAL_EDITOR_PATH is set
if "%VYTAL_EDITOR_PATH%"=="" (
    echo Error: VYTAL_EDITOR_PATH is not set.
    exit /b 1
)

rem make sure that VYTAL_ENGINE_PATH is set
if "%VYTAL_ENGINE_PATH%"=="" (
    echo Error: VYTAL_ENGINE_PATH is not set.
    exit /b 1
)

rem make sure that output directory exists
if not exist "%VYTAL_EDITOR_PATH%\bin" mkdir "%VYTAL_EDITOR_PATH%\bin"

rem copy engine dll to editor binaries folder
copy "%VYTAL_ENGINE_PATH%\bin\vytal_engine.dll" "%VYTAL_EDITOR_PATH%\bin\"

rem collect all .c files
set "c_filenames="
for /r %%f in (*.c) do (
    set "c_filenames=!c_filenames! %%f"
)

rem compiler settings
set "compiler_flags=-g"
set "include_flags=-Isrc -I%VYTAL_ENGINE_PATH%\src"
set "linker_flags=-L%VYTAL_EDITOR_PATH%\bin -lvytal_engine"
set "defines=-DVYTAL_DEBUG -DVYTAL_IMPORT_DLL -DLINE_BUFFER_MAX_SIZE=256 -DKEY_MAX_SIZE=64 -DVALUE_MAX_SIZE=256 -DWINDOW_REPORT_INTERVAL_MS=50 "

rem build command
echo Building '%CODEBASE%'...
gcc %c_filenames% %compiler_flags% %include_flags% %defines% %linker_flags% -o %VYTAL_EDITOR_PATH%\bin\%CODEBASE%.exe


rem check compilation statusze
if %errorlevel% neq 0 (
    echo '%CODEBASE%' build failed!
    exit /b 1
) else (
    echo '%CODEBASE%' build completed.
)

endlocal