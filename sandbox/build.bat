@echo off
setlocal EnableDelayedExpansion

rem --- get the list of all the c. files in this codebase
set "c_filenames="
for /r %%f in (*.c) do (
    set "c_filenames=!c_filenames! %%f"
)

set "codebase=sandbox"

rem --- compiler flags
set "compiler_flags=-g"

rem --- include flags
set "include_flags=-Isrc -I../engine/src/"

rem --- linker flags
set "linker_flags=-L../bin/ -lengine"

rem --- defines
set "defines=-DVYTAL_DEBUG -DVYTAL_IMPORT_DLL -DLINE_BUFFER_MAX_SIZE=8192 "

rem --- build command
echo Building '%codebase%'...
gcc %c_filenames% %compiler_flags% %include_flags% %defines% %linker_flags% -o ../bin/%codebase%.exe

echo '%codebase%' build completed.
endlocal


