@echo off
echo ==========================================
echo    IGED - UNREAL ENGINE 5 AI SYSTEM
echo ==========================================
echo.
echo Starting IGED UE5...
echo.

REM Set your UE5 path here
set UE5_PATH=C:\Program Files\Epic Games\UE_5.3

if not exist "%UE5_PATH%" (
    echo ERROR: Unreal Engine 5 not found!
    echo Please install from: https://www.unrealengine.com
    pause
    exit /b 1
)

echo Launching IGED in Unreal Engine 5...
start "" "%UE5_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0IGED_UE5.uproject"

echo.
echo IGED UE5 is running!
pause