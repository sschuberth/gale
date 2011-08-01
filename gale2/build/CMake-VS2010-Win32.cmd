@echo off

call opengl-parse.cmd

rem (Re-)create an empty output directory.
rmdir /s /q VS2010-Win32 2> nul
mkdir VS2010-Win32 2> nul

rem Generate the project files in the output directory.
pushd VS2010-Win32
cmake -G "Visual Studio 10" ..
if errorlevel 1 pause
popd
