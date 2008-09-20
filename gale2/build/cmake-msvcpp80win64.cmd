@echo off

call common.cmd %1

: (Re-)create an empty output directory.
rmdir /s /q msvcpp80win64 2> nul
mkdir msvcpp80win64 2> nul

: Generate the project files in the output directory.
pushd msvcpp80win64
cmake -G "Visual Studio 8 2005 Win64" ..
popd
