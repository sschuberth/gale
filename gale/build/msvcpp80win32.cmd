@echo off

: (Re-)create an empty output directory.
rmdir /s /q %~dpn0 2> nul
mkdir %~dpn0 2> nul

: Generate the project files in the output directory.
pushd %~dpn0
cmake.exe -G "Visual Studio 8 2005" ..\..\source
popd