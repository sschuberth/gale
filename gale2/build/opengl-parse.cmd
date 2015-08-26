@echo off

setlocal

rem Read the Git for Windows installation path from the Registry.
:REG_QUERY
for /f "skip=2 delims=: tokens=1*" %%a in ('reg query "HKLM\SOFTWARE%WOW%\Microsoft\Windows\CurrentVersion\Uninstall\Git_is1" /v InstallLocation 2^> nul') do (
    for /f "tokens=3" %%z in ("%%a") do (
        set GIT=%%z:%%b
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
set PATH=%GIT%bin;%PATH%

where /q bash.exe
if errorlevel 1 (
    echo Error: Unable to find "bash.exe" in PATH.
    exit /b
)

pushd "%~dp0..\glex"

echo *** Deleting existing OpenGL API initialization files ...
del GLEX_*.*

echo *** Updating OpenGL specification files ...
bash -lc "../../glex2/update_spec.sh"

echo *** Parsing required OpenGL APIs ...
bash -lc "../../glex2/glex.sh es=../../glex2/spec/enumext.spec fs=../../glex2/spec/gl.spec tm=../../glex2/spec/gl.tm api=@../build/opengl-apis.txt"
bash -lc "../../glex2/glex.sh es=../../glex2/spec/wglenumext.spec fs=../../glex2/spec/wglext.spec tm=../../glex2/spec/wgl.tm api=@../build/opengl-apis.txt"

popd
