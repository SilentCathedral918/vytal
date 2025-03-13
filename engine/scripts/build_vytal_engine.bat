@echo off
setlocal EnableDelayedExpansion

set "CODEBASE=vytal_engine"

rem make sure that VYTAL_ENGINE_PATH is set
if "%VYTAL_ENGINE_PATH%"=="" (
    echo Error: VYTAL_ENGINE_PATH is not set.
    exit /b 1
)

rem make sure that output directory exists
if not exist "%VYTAL_ENGINE_PATH%\bin" mkdir "%VYTAL_ENGINE_PATH%\bin"

rem collect all .c files
set "c_filenames="
for /r %%f in (*.c) do (
    set "c_filenames=!c_filenames! %%f"
)

rem compiler settings
set "compiler_flags=-g -mavx2 -mfma -shared -Wall -Werror -Wvarargs -Wno-unused-function -Wno-discarded-qualifiers"
set "include_flags=-Isrc -I%VYTAL_EXTERNAL_CGLTF% -I%VYTAL_EXTERNAL_GLFW%/include -I%VYTAL_EXTERNAL_VULKAN%/Include -I%VYTAL_EXTERNAL_CGLM%/include -I%VYTAL_EXTERNAL_STB%"
set "linker_flags=-L%VYTAL_EXTERNAL_GLFW%/lib -lglfw3 -luser32 -lgdi32 -lopengl32 -L%VYTAL_EXTERNAL_VULKAN%/Lib -lvulkan-1"
set "defines=-DVYTAL_DEBUG -DVYTAL_ENABLE_ASSERTIONS -DVYTAL_EXPORT_DLL -DVYTAL_VULKAN_VALIDATION_LAYERS_ENABLED -D_CRT_SECURE_NO_WARNINGS -DLINE_BUFFER_MAX_SIZE=512 -DSTRING_BUFFER_MAX_SIZE=8192 -DFILENAME_BUFFER_MAX_SIZE=64 -DCVAR_HASHMAP_SIZE=1024 -DMAX_EXCEPTION_DEPTH=10 -DMEMORY_ALIGNMENT_SIZE=16 -DCONTAINER_DEFAULT_CAPACITY=10 -DCONTAINER_RESIZE_FACTOR=2 -DMAX_COMPUTE_DESCRIPTOR_SETS=64 -DMAX_COMPUTE_PIPELINES=16 -DDEFAULT_TEXTURE_WIDTH=512 -DDEFAULT_TEXTURE_HEIGHT=512 -DDEFAULT_TEXTURE_SQUARE_SIZE=64 "

rem build command
echo Building '%CODEBASE%'...
gcc %c_filenames% %compiler_flags% %include_flags% %defines% %linker_flags% -o %VYTAL_ENGINE_PATH%\bin\%CODEBASE%.dll

rem check compilation status
if %errorlevel% neq 0 (
    echo '%CODEBASE%' build failed!
    exit /b 1
) else (
    echo '%CODEBASE%' build completed.
)

endlocal


