@echo off

setlocal enabledelayedexpansion

call opengl-parse.cmd
if not errorlevel 0 (
    echo Error: Failed to parse the OpenGL specification.
    goto QUIT
)

rem Check if CMake is in PATH.
for /f "delims=" %%i in ('where cmake.exe 2^> nul') do (
    set CMAKE=%%i
)
if exist "%CMAKE%" goto FOUND

echo *** Searching for a CMake ^>= 3.5 executable ...

rem Read a CMake >= 3.5 installation path from the registry.
for %%k in (HKCU HKLM) do (
    for %%w in (\ \Wow6432Node\) do (
        for /f "skip=1 delims=" %%u in ('reg query "%%k\SOFTWARE%%wMicrosoft\Windows\CurrentVersion\Uninstall" /f "{*" /k 2^> nul') do (
            for /f "skip=2 tokens=3*" %%d in ('reg query "%%u" /v DisplayName 2^> nul') do (
                if "%%d"=="CMake" (
                    for /f "skip=2 tokens=3*" %%i in ('reg query "%%u" /v InstallLocation 2^> nul') do (
                        set CMAKE=%%i %%jbin\cmake.exe
                        goto FOUND
                    )
                )
            )
        )
    )
)

echo *** Searching for a CMake ^< 3.5 executable ...

rem Read a CMake < 3.5 installation path from the registry.
for %%k in (HKCU HKLM) do (
    for %%w in (\ \Wow6432Node\) do (
        set KEY=%%k\SOFTWARE%%wMicrosoft\Windows\CurrentVersion\Uninstall
        for /f "delims=\ tokens=8" %%u in ('reg query "!KEY!" 2^> nul') do (
            for /f "tokens=1" %%n in ("%%u") do (
                if "%%n"=="CMake" (
                    for /f "skip=2 delims=: tokens=1*" %%a in ('reg query "!KEY!\%%u" /v UninstallString 2^> nul') do (
                        for /f "tokens=3" %%z in ("%%a") do (
                            set CMAKE=%%z:%%~pbbin\cmake.exe
                            goto FOUND
                        )
                    )
                )
            )
        )
    )
)

echo *** Unable to find a CMake installation directory in the registry.
goto QUIT

:FOUND

echo *** Found a CMake executable at "%CMAKE%".
"%CMAKE%" --version

rem (Re-)create an empty output directory to generate the project files in.
rmdir /s /q %1 2> nul
mkdir %1 2> nul

pushd %1
"%CMAKE%" -G %1 ..
popd

:QUIT

exit /b %errorlevel%
