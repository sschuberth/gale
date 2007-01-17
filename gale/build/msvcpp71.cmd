@echo off

: Add the Cygwin binary directory to the PATH.
if "%1" == "" (
    set cygwin="C:\cygwin\bin"
) else (
    set cygwin="%1"
)

set OLD_PATH=%PATH%
set PATH=%cygwin%;%PATH%

pushd "%~dp0..\glex"

: Download and parse the OpenGL extension registry.
bash -c "export PATH=/bin && ../../glex/registry/update_registry.sh ../../glex/registry/OpenGL.org"

: List the extensions used in the project.
bash -c "export PATH=/bin && ../../glex/glex.sh spec=../../glex/registry/OpenGL.org/ARB/wgl_pixel_format.txt"

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
cmake -G "Visual Studio 7 .NET 2003" ..
popd
