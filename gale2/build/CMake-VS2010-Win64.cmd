@echo off

call opengl-parse.cmd

: (Re-)create an empty output directory.
rmdir /s /q VS2010-Win64 2> nul
mkdir VS2010-Win64 2> nul

: Generate the project files in the output directory.
pushd VS2010-Win64
cmake -G "Visual Studio 10 Win64" ..
if errorlevel 1 pause
popd
