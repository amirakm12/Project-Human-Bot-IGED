@echo off
REM IGED Biometric Authentication - Update Existing GitHub Repository
REM Updates https://github.com/amirakm12/Project-Human-Bot-IGED

echo.
echo ========================================
echo   IGED Biometric Auth - Update Repository
echo ========================================
echo.
echo Repository: https://github.com/amirakm12/Project-Human-Bot-IGED
echo.

REM Set variables
set REPO_URL=https://github.com/amirakm12/Project-Human-Bot-IGED.git
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

REM Check if remote exists
echo [3/8] Checking remote repository...
git remote -v | findstr "amirakm12/Project-Human-Bot-IGED" >nul 2>&1
if errorlevel 1 (
    echo Adding remote repository...
    git remote add origin %REPO_URL%
    echo Remote added: %REPO_URL%
    echo.
)

REM Fetch latest changes
echo [4/8] Fetching latest changes from GitHub...
git fetch origin
if errorlevel 1 (
    echo WARNING: Could not fetch from remote. Continuing with local changes...
    echo.
)

REM Add all files to Git
echo [5/8] Adding files to Git...
git add .

REM Check if there are changes to commit
echo [6/8] Checking for changes...
git status --porcelain >nul 2>&1
if errorlevel 1 (
    echo No changes to commit.
    echo All files are up to date.
    goto :end
)

REM Show what will be committed
echo [7/8] Files to be committed:
git status --short
echo.

REM Commit changes
echo [8/8] Committing changes...
git commit -m %COMMIT_MESSAGE%
if errorlevel 1 (
    echo ERROR: Failed to commit changes
    pause
    exit /b 1
)

REM Push to GitHub
echo Pushing to GitHub repository...
git push -u origin %BRANCH_NAME%
if errorlevel 1 (
    echo ERROR: Failed to push to GitHub
    echo Please check your authentication and try again.
    echo.
    echo You may need to:
    echo 1. Set up GitHub authentication (Personal Access Token or SSH key)
    echo 2. Ensure you have write access to the repository
    echo 3. Check your internet connection
    pause
    exit /b 1
)

:end
echo.
echo ========================================
echo   Update Complete!
echo ========================================
echo.
echo Repository: https://github.com/amirakm12/Project-Human-Bot-IGED
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
echo Next steps:
echo 1. Visit: https://github.com/amirakm12/Project-Human-Bot-IGED
echo 2. Verify all files are uploaded
echo 3. Set up GitHub Pages for documentation
echo 4. Configure repository settings
echo.
pause 