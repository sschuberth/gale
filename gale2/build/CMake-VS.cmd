@echo off

setlocal enabledelayedexpansion

call opengl-parse.cmd
if errorlevel 1 (
    echo Error: Failed to parse the OpenGL specification.
    exit /b
)

rem (Re-)create an empty output directory.
rmdir /s /q %1 2> nul
mkdir %1 2> nul

rem Read the CMake installation path from the Registry.
for %%k in (HKCU HKLM) do (
    for %%w in (\ \Wow6432Node\) do (
        set KEY=%%k\SOFTWARE%%wMicrosoft\Windows\CurrentVersion\Uninstall
        for /f "delims=\ tokens=8" %%u in ('reg query "!KEY!" 2^> nul') do (
            for /f "tokens=1" %%n in ("%%u") do (
                if "%%n"=="CMake" (
                    for /f "skip=2 delims=: tokens=1*" %%a in ('reg query "!KEY!\%%u" /v UninstallString 2^> nul') do (
                        for /f "tokens=3" %%z in ("%%a") do (
                            set CMAKE=%%z:%%~pbbin\cmake.exe
                            echo Found CMake at "!CMAKE!".
                            goto DONE
                        )
                    )
                )
            )
        )
    )
)

:DONE

rem Generate the project files in the output directory.
pushd %1
"%CMAKE%" --version
"%CMAKE%" -G %1 ..
popd
