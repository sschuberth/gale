:: Batch File Command Reference for Windows 2000
:: http://labmice.techtarget.com/articles/batchcmds.htm

@echo off

: Add the Cygwin binary directory to the PATH.
if "%1" == "" (
    set CYGWIN="C:\cygwin\bin"
) else (
    set CYGWIN="%1"
)

echo *** Assuming Cygwin to be installed at %CYGWIN% ...

set OLD_PATH=%PATH%
set PATH=%CYGWIN%;%PATH%

pushd "%~dp0..\glex"

: Download and parse the OpenGL extension registry if needed.
if not exist "..\..\glex\registry\OpenGL.org\ARB\color_buffer_float.txt" (
    echo *** Generating local OpenGL registry ...
    bash -c "export PATH=/bin && ../../glex/registry/update_registry.sh ../../glex/registry/OpenGL.org"
)

: List the extensions used in the project.
echo *** Parsing required OpenGL extensions ...
for /f %%e in (..\build\extensions.txt) do (
    echo Parsing file "%%e" ...
    bash -c "export PATH=/bin && ../../glex/glex.sh spec=../../glex/registry/OpenGL.org/%%e"
)

popd

: Undo any changes to variables.
set PATH=%OLD_PATH%
set OLD_PATH=
set CYGWIN=
