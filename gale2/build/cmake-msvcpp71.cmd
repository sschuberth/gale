@echo off

call common.cmd %1

: (Re-)create an empty output directory.
rmdir /s /q msvcpp71 2> nul
mkdir msvcpp71 2> nul

: Generate the project files in the output directory.
pushd msvcpp71
cmake -G "Visual Studio 7 .NET 2003" ..
popd
