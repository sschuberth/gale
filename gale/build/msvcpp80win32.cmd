@echo off

: Add the Cygwin binary directory to the PATH.
if "%1" == "" (
    set cygwin="C:\cygwin\bin"
) else (
    set cygwin="%1"
)

set OLD_PATH=%PATH%
set PATH=%cygwin%;%PATH%

: Download and parse the OpenGL extension registry.
pushd "%~dp0..\..\glex"
bash -c "export PATH=/bin && ./registry/update_registry.sh ./registry/OpenGL.org"
bash -c "export PATH=/bin && ./glex.sh spec=./registry/OpenGL.org/ARB/wgl_pixel_format.txt"
mv GLEX_*.* ..\gale\include\glex
popd

: Undo any changes to variables.
set PATH=%OLD_PATH%
set OLD_PATH=
set cygwin=

: (Re-)create an empty output directory.
rmdir /s /q "%~dpn0" 2> nul
mkdir "%~dpn0" 2> nul

: Generate the project files in the output directory.
pushd "%~dpn0"
cmake -G "Visual Studio 8 2005" ..
popd
