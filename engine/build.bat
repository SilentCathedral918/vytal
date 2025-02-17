@echo off
setlocal EnableDelayedExpansion

rem --- get the list of all the c. files in this codebase
set "c_filenames="
for /r %%f in (*.c) do (
    set "c_filenames=!c_filenames! %%f"
)

set "codebase=engine"

rem --- compiler flags
set "compiler_flags=-g -mavx2 -mfma -shared -Wall -Werror -Wvarargs -Wno-unused-function -Wno-discarded-qualifiers"

rem --- include flags
set "include_flags=-Isrc"

rem --- linker flags
set "linker_flags=-luser32 -lgdi32 -lopengl32"

rem --- defines
set "defines=-DVYTAL_DEBUG -DVYTAL_ENABLE_ASSERTIONS -DVYTAL_EXPORT_DLL -D_CRT_SECURE_NO_WARNINGS -DLINE_BUFFER_MAX_SIZE=8192 -DCVAR_HASHMAP_SIZE=1024 -DMAX_EXCEPTION_DEPTH=10 "

rem --- build command
echo Building '%codebase%'...
gcc %c_filenames% %compiler_flags% %include_flags% %defines% %linker_flags% -o ../bin/%codebase%.dll

echo '%codebase%' build completed.
endlocal


