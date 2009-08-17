@echo off

call opengl-parse.cmd %1

: (Re-)create an empty output directory.
rmdir /s /q msvcpp90win64 2> nul
mkdir msvcpp90win64 2> nul

: Generate the project files in the output directory.
pushd msvcpp90win64
cmake -G "Visual Studio 9 2008 Win64" ..
popd
