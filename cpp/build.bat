@echo off
REM Build script for Windows with vcpkg and Visual Studio

echo Building Traductor Danes-Espanol C++ Native...

REM Check if vcpkg is available
if not exist "%VCPKG_ROOT%" (
    echo Warning: VCPKG_ROOT not set. Make sure vcpkg is installed and configured.
    set VCPKG_ROOT=C:\vcpkg
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

REM Build
echo Building...
cmake --build . --config Release --parallel

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!
echo.
echo Executables available in:
echo   bin/traductor_cli.exe      - Command line interface
echo   bin/traductor_gui.exe      - Qt GUI (if Qt available)
echo   bin/traductor_rest.exe     - REST API server (if Drogon available)

pause
