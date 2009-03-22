@echo off

call common.cmd %1

: (Re-)create an empty output directory.
rmdir /s /q msvcpp90win32 2> nul
mkdir msvcpp90win32 2> nul

: Generate the project files in the output directory.
pushd msvcpp90win32
cmake -G "Visual Studio 9 2008" ..
popd
