@echo off
REM IGED UE5 - Windows Build Script
REM Copyright (c) 2024 Project Human Bot

echo ========================================
echo  IGED UE5 - Advanced Build System
echo  Version 1.0.0
echo ========================================
echo.

REM Set environment variables
set UE5_ROOT=C:\Program Files\Epic Games\UE_5.4
set PROJECT_ROOT=%~dp0
set BUILD_CONFIG=Development
set PLATFORM=Win64

REM Parse command line arguments
if "%1"=="shipping" set BUILD_CONFIG=Shipping
if "%1"=="debug" set BUILD_CONFIG=Debug
if "%1"=="test" set BUILD_CONFIG=Test
if "%2"=="clean" set CLEAN_BUILD=1

echo Configuration: %BUILD_CONFIG%
echo Platform: %PLATFORM%
echo.

REM Check if UE5 is installed
if not exist "%UE5_ROOT%" (
    echo ERROR: Unreal Engine 5.4 not found at %UE5_ROOT%
    echo Please install UE5.4 or update the UE5_ROOT path
    exit /b 1
)

REM Clean build if requested
if "%CLEAN_BUILD%"=="1" (
    echo Cleaning previous build...
    rmdir /s /q "%PROJECT_ROOT%Binaries" 2>nul
    rmdir /s /q "%PROJECT_ROOT%Intermediate" 2>nul
    rmdir /s /q "%PROJECT_ROOT%Saved" 2>nul
    echo Clean complete.
    echo.
)

REM Generate project files
echo Generating project files...
call "%UE5_ROOT%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "%PROJECT_ROOT%IGED_UE5.uproject" -Game -Engine
if errorlevel 1 (
    echo ERROR: Failed to generate project files
    exit /b 1
)

REM Build the project
echo Building IGED_UE5...
call "%UE5_ROOT%\Engine\Build\BatchFiles\Build.bat" IGED_UE5 %PLATFORM% %BUILD_CONFIG% "%PROJECT_ROOT%IGED_UE5.uproject" -WaitMutex
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

REM Cook content for shipping builds
if "%BUILD_CONFIG%"=="Shipping" (
    echo Cooking content for shipping...
    call "%UE5_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="%PROJECT_ROOT%IGED_UE5.uproject" -noP4 -platform=%PLATFORM% -clientconfig=%BUILD_CONFIG% -serverconfig=%BUILD_CONFIG% -cook -allmaps -build -stage -pak -archive -archivedirectory="%PROJECT_ROOT%Package"
    if errorlevel 1 (
        echo WARNING: Cooking failed but build succeeded
    )
)

echo.
echo ========================================
echo  Build Complete!
echo  Output: %PROJECT_ROOT%Binaries\%PLATFORM%
echo ========================================

REM Launch the editor if in development mode
if "%BUILD_CONFIG%"=="Development" (
    echo.
    echo Launch IGED_UE5 Editor? (Y/N)
    set /p LAUNCH=
    if /i "%LAUNCH%"=="Y" (
        echo Launching IGED_UE5...
        start "" "%UE5_ROOT%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT_ROOT%IGED_UE5.uproject"
    )
)

exit /b 0
