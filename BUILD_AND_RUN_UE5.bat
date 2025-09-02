@echo off
REM IGED Unreal Engine 5 Build and Launch Script
echo ========================================
echo IGED UE5 - Enterprise AI System
echo ========================================
echo.

REM Set UE5 installation path (adjust this to your UE5 location)
set UE5_PATH=C:\Program Files\Epic Games\UE_5.3
set PROJECT_PATH=%~dp0IGED_UE5
set PROJECT_FILE=%PROJECT_PATH%\IGED_UE5.uproject

echo Checking Unreal Engine 5 installation...
if not exist "%UE5_PATH%" (
    echo ERROR: Unreal Engine 5 not found at %UE5_PATH%
    echo Please install UE5 or update the path in this script
    pause
    exit /b 1
)

echo.
echo Building IGED UE5 Project...
echo ========================================

REM Generate project files
echo Generating project files...
"%UE5_PATH%\Engine\Build\BatchFiles\Build.bat" -projectfiles -project="%PROJECT_FILE%" -game -rocket -progress

REM Build the project
echo Building project...
"%UE5_PATH%\Engine\Build\BatchFiles\Build.bat" IGED Win64 Development "%PROJECT_FILE%" -waitmutex

REM Cook content
echo Cooking content...
"%UE5_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="%PROJECT_FILE%" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="%PROJECT_PATH%\Packaged"

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.

REM Launch the editor
echo Launching IGED in Unreal Engine 5...
start "" "%UE5_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT_FILE%"

echo.
echo IGED UE5 is now running!
echo.
pause