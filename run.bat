@echo off
title IGED - Project Human Bot

echo ========================================
echo IGED - Project Human Bot Launcher
echo ========================================
echo.

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.8+ from https://python.org
    pause
    exit /b 1
)

REM Check if main files exist
if not exist "launcher.py" (
    echo ERROR: launcher.py not found
    echo Please run this from the IGED project directory
    pause
    exit /b 1
)

REM Run IGED
echo Starting IGED...
echo.
python main.py

REM Keep window open on error
if errorlevel 1 (
    echo.
    echo IGED exited with an error.
    pause
)