@echo off

echo Building all codebases (engine + sandbox)...

rem --- generates a directory to store binaries if yet to exist
set "bin_dir=./bin"
if not exist %bin_dir% mkdir %bin_dir%

rem --- 1. engine code
pushd engine
call build.bat
popd
if %ERRORLEVEL% neq 0 (echo Error:%ERRORLEVEL% && exit)

rem --- 2. sandbox code
pushd sandbox
call build.bat
popd
if %ERRORLEVEL% neq 0 (echo Error:%ERRORLEVEL% && exit)

echo All codebases (engine + sandbox) build completed.
