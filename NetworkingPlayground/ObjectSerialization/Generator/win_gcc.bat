@echo off
setlocal

REM Compute paths
set SOURCE_DIR=%~dp0
set BUILD_DIR=%SOURCE_DIR%\..\Solution\MinGW

echo Source dir: %SOURCE_DIR%
echo Build dir: %BUILD_DIR%

REM Create build directory if it does not exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM Configure with CMake using MinGW
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ "%SOURCE_DIR%"

if %errorlevel% neq 0 (
    echo CMake configuration failed.
    exit /b %errorlevel%
)

REM Build
mingw32-make

if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)

echo Build completed successfully.

endlocal