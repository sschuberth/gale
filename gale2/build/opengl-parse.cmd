@echo off

setlocal enabledelayedexpansion

rem Check if Bash is in PATH.
for /f "delims=" %%i in ('where bash.exe 2^> nul') do (
    set BASH=%%i
)
if exist "%BASH%" goto FOUND

echo *** Searching for a Bash executable ...

rem Read the Git for Windows installation path from the registry.
for %%k in (HKCU HKLM) do (
    for %%w in (\ \Wow6432Node\) do (
        for /f "skip=2 tokens=3*" %%i in ('reg query "%%k\SOFTWARE%%wMicrosoft\Windows\CurrentVersion\Uninstall\Git_is1" /v InstallLocation 2^> nul') do (
            set BASH=%%i %%jbin\bash.exe
            goto FOUND
        )
    )
)

echo *** Unable to find a Git for Windows installation directory in the registry.
goto QUIT

:FOUND

echo *** Found a Bash executable at "%BASH%".
"%BASH%" --version

pushd "%~dp0..\glex"

echo *** Deleting existing OpenGL API initialization files ...
del GLEX_*.*

echo *** Updating OpenGL specification files ...
"%BASH%" -lc "../../glex2/update_spec.sh"

echo *** Parsing required OpenGL APIs ...
"%BASH%" -lc "../../glex2/glex.sh es=../../glex2/spec/enumext.spec fs=../../glex2/spec/gl.spec tm=../../glex2/spec/gl.tm api=@../build/opengl-apis.txt"
"%BASH%" -lc "../../glex2/glex.sh es=../../glex2/spec/wglenumext.spec fs=../../glex2/spec/wglext.spec tm=../../glex2/spec/wgl.tm api=@../build/opengl-apis.txt"

popd

:QUIT

exit /b %errorlevel%
