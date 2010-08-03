@echo off

setlocal

: Add the Cygwin binary directory to the PATH.
if "%1" == "" (
    set CYGWIN="C:\cygwin\bin"
) else (
    set CYGWIN="%1"
)

echo *** Assuming Cygwin to be installed at %CYGWIN% ...

set PATH=%CYGWIN%;%PATH%

pushd "%~dp0..\glex"

echo *** Deleting existing OpenGL API initialization files ...
del GLEX_*.*

: Download the OpenGL specification files if needed.
echo *** Updating OpenGL specification files ...
bash -c "export PATH=/bin && ../../glex2/update_spec.sh"
echo *** Done

: Read the OpenGL APIs used in the project.
echo *** Parsing required OpenGL APIs ...
bash -c "export PATH=/bin && ../../glex2/glex.sh es=../../glex2/spec/enumext.spec fs=../../glex2/spec/gl.spec tm=../../glex2/spec/gl.tm api=@`cygpath '%~dp0opengl-apis.txt'`"
bash -c "export PATH=/bin && ../../glex2/glex.sh es=../../glex2/spec/wglenumext.spec fs=../../glex2/spec/wglext.spec tm=../../glex2/spec/wgl.tm api=@`cygpath '%~dp0opengl-apis.txt'`"
echo *** Done

if exist *.stackdump pause

popd
