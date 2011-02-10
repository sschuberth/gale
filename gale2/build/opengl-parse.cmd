@echo off

setlocal

rem Read the msysGit installation path from the Registry.
:REG_QUERY
for /f "skip=2 delims=: tokens=1*" %%a in ('reg query HKLM\SOFTWARE%WOW%\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1 /v InstallLocation 2^> nul') do (
    for /f "tokens=3" %%i in ("%%a") do (
        set GIT=%%i:%%b
    )
)
if "%GIT%"=="" (
    if "%WOW%"=="" (
        rem Assume we are on 64-bit Windows, so explicitly read the 32-bit Registry.
        set WOW=\Wow6432Node
        goto REG_QUERY
    )
)

rem Make sure Bash is in PATH (for running scripts).
set PATH=%PATH%;%GIT%bin

pushd "%~dp0..\glex"

echo *** Deleting existing OpenGL API initialization files ...
del GLEX_*.*

: Download the OpenGL specification files if needed.
echo *** Updating OpenGL specification files ...
bash -c "export PATH=$PATH:/bin && ../../glex2/update_spec.sh"
echo *** Done

: Read the OpenGL APIs used in the project.
echo *** Parsing required OpenGL APIs ...
bash -c "export PATH=$PATH:/bin && ../../glex2/glex.sh es=../../glex2/spec/enumext.spec fs=../../glex2/spec/gl.spec tm=../../glex2/spec/gl.tm api=@../build/opengl-apis.txt"
bash -c "export PATH=$PATH:/bin && ../../glex2/glex.sh es=../../glex2/spec/wglenumext.spec fs=../../glex2/spec/wglext.spec tm=../../glex2/spec/wgl.tm api=@../build/opengl-apis.txt"
echo *** Done

if exist *.stackdump pause

popd
