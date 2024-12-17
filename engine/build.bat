@echo off
setlocal EnableDelayedExpansion

rem --- get the list of all the c. files in this codebase
set "c_filenames="
for /r %%f in (*.c) do (
    set "c_filenames=!c_filenames! %%f"
)

set "codebase=engine"

rem --- compiler flags
set "compiler_flags=-g -shared -Wall -Werror -Wvarargs -Wno-unused-function"

rem --- include flags
set "include_flags=-Isrc -I%VULKAN_SDK%/Include -I%GLFW_PATH%/include"

rem --- linker flags
set "linker_flags=-luser32 -L%VULKAN_SDK%/Lib -lvulkan-1 -L%GLFW_PATH%/lib -lglfw3 -lgdi32 -lopengl32"

rem --- defines
set "defines=-DVT_DEBUG -DVT_ENABLE_ASSERTIONS -DVT_EXPORT_DLL -D_CRT_SECURE_NO_WARNINGS"

rem --- build command
echo "Building %codebase%..."
gcc %c_filenames% %compiler_flags% %include_flags% %defines% %linker_flags% -o ../bin/%codebase%.dll

echo "%codebase% built..."
endlocal
