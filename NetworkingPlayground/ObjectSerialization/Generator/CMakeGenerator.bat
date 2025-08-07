@echo off
setlocal

REM Compute paths
set SOURCE_DIR=%~dp0
set BUILD_DIR=%SOURCE_DIR%\..\Solution
set BUILD_TEST_DIR=%BUILD_DIR%\Test
set BUILD_TARGET=all

if not "%~1"=="" (
	set BUILD_TARGET=%~1
)


REM === MAIN BUILD CHECK ===
if /I "%BUILD_TARGET%"=="all" (
    goto build_main
) else if /I "%BUILD_TARGET%"=="main" (
    goto build_main
)

REM Skip MAIN if not matching
goto skip_main

:build_main
    if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
    cd /d "%BUILD_DIR%"
    cmake -B "%BUILD_DIR%" -S "%SOURCE_DIR%"
:skip_main

REM === TEST BUILD CHECK ===
if /I "%BUILD_TARGET%"=="all" (
    goto build_test
) else if /I "%BUILD_TARGET%"=="test" (
    goto build_test
)

REM Skip TEST if not matching
goto done

:build_test
    if not exist "%BUILD_TEST_DIR%" mkdir "%BUILD_TEST_DIR%"
    cd /d "%BUILD_TEST_DIR%"
    cmake "-DTEST=ON" -B "%BUILD_TEST_DIR%" -S "%SOURCE_DIR%"

:done