@echo off

setlocal

call opengl-parse.cmd
if errorlevel 1 (
    echo Error: Failed to parse the OpenGL specification.
    exit /b
)

rem (Re-)create an empty output directory.
rmdir /s /q %1 2> nul
mkdir %1 2> nul

rem Read the CMake installation path from the Registry.
:REG_QUERY
for /f "delims=\ tokens=8" %%u in ('reg query "HKLM\SOFTWARE%WOW%\Microsoft\Windows\CurrentVersion\Uninstall" 2^> nul') do (
    for /f "tokens=1" %%n in ("%%u") do (
        if "%%n"=="CMake" (
            for /f "skip=2 delims=: tokens=1*" %%a in ('reg query "HKLM\SOFTWARE%WOW%\Microsoft\Windows\CurrentVersion\Uninstall\%%u" /v UninstallString 2^> nul') do (
                for /f "tokens=3" %%z in ("%%a") do (
                    set CMAKE=%%z:%%~pbbin\cmake.exe
                )
            )
        )
    )
)
if "%CMAKE%"=="" (
    if "%WOW%"=="" (
        rem Assume we are on 64-bit Windows, so explicitly read the 32-bit Registry.
        set WOW=\Wow6432Node
        goto REG_QUERY
    )
)

rem Generate the project files in the output directory.
pushd %1
"%CMAKE%" --version
"%CMAKE%" -G %1 ..
if errorlevel 1 pause
popd
