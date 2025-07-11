@echo off
REM IGED Biometric Authentication - GitHub Update Script
REM Updates all files to GitHub repository

echo.
echo ========================================
echo   IGED Biometric Auth - GitHub Update
echo ========================================
echo.

REM Set variables
set REPO_NAME=IGED_Biometric_Auth
set COMMIT_MESSAGE="Add IGED Biometric Authentication Integration Suite v1.0.0"
set BRANCH_NAME=main

REM Check if Git is installed
echo [1/8] Checking Git installation...
git --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Git not found in PATH
    echo Please install Git and add to PATH
    pause
    exit /b 1
)

REM Check if we're in a Git repository
echo [2/8] Checking Git repository...
if not exist .git (
    echo Initializing new Git repository...
    git init
    echo.
)

REM Add all files to Git
echo [3/8] Adding files to Git...
git add .

REM Check if there are changes to commit
echo [4/8] Checking for changes...
git status --porcelain >nul 2>&1
if errorlevel 1 (
    echo No changes to commit.
    echo All files are up to date.
    goto :end
)

REM Show what will be committed
echo [5/8] Files to be committed:
git status --short
echo.

REM Commit changes
echo [6/8] Committing changes...
git commit -m %COMMIT_MESSAGE%
if errorlevel 1 (
    echo ERROR: Failed to commit changes
    pause
    exit /b 1
)

REM Check if remote exists
echo [7/8] Checking remote repository...
git remote -v >nul 2>&1
if errorlevel 1 (
    echo No remote repository configured.
    echo Please add your GitHub remote:
    echo git remote add origin https://github.com/YOUR_USERNAME/IGED_Biometric_Auth.git
    echo.
    echo Then run: git push -u origin main
    goto :end
)

REM Push to GitHub
echo [8/8] Pushing to GitHub...
git push origin %BRANCH_NAME%
if errorlevel 1 (
    echo ERROR: Failed to push to GitHub
    echo Please check your remote configuration and try again.
    pause
    exit /b 1
)

:end
echo.
echo ========================================
echo   Update Complete!
echo ========================================
echo.
echo Repository: %REPO_NAME%
echo Branch: %BRANCH_NAME%
echo Commit: %COMMIT_MESSAGE%
echo.
echo Files updated:
echo - Complete biometric authentication suite
echo - Windows Hello integration
echo - WebAuthn server implementation
echo - PySide6 GUI manager
echo - Secure launcher with biometric protection
echo - Cryptographic utilities
echo - Unified logging system
echo - Android client framework
echo - Build scripts and documentation
echo.
pause 