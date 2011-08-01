@echo off

call opengl-parse.cmd

rem (Re-)create an empty output directory.
rmdir /s /q VS2008-Win32 2> nul
mkdir VS2008-Win32 2> nul

rem Generate the project files in the output directory.
pushd VS2008-Win32
cmake -G "Visual Studio 9 2008" ..
if errorlevel 1 pause
popd
