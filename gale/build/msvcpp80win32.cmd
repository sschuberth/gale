@echo off

call common.cmd %1

: (Re-)create an empty output directory.
rmdir /s /q "%~dpn0" 2> nul
mkdir "%~dpn0" 2> nul

: Generate the project files in the output directory.
pushd "%~dpn0"
cmake -G "Visual Studio 8 2005" ..
popd
