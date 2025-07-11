@echo off
echo ========================================
echo IGED - Add ALL Files to GitHub Repository
echo ========================================
echo.

REM Check if git is initialized
if not exist ".git" (
    echo Initializing git repository...
    git init
    echo.
)

REM Add the remote repository if it doesn't exist
git remote -v | findstr "origin" >nul
if errorlevel 1 (
    echo Adding remote repository...
    git remote add origin https://github.com/amirakm12/Project-Human-Bot-IGED.git
    echo.
)

REM Add ALL files and folders recursively
echo Adding ALL files and folders to git...
git add .

REM Check if there are any changes to commit
git status --porcelain | findstr . >nul
if errorlevel 1 (
    echo No changes to commit.
    goto :end
)

REM Commit all changes
echo.
echo Committing all changes...
git commit -m "Add complete IGED project with all files and folders

- Core system components (voice pipeline, encryption, memory engine)
- Agent modules (codegen, secops, data miner, network intelligence)
- Plugin system with exploit developer and system info
- Windows GUI and web admin panel
- Android client integration
- Build scripts and installation tools
- Documentation and configuration files
- Biometric authentication integration suite
- Complete project structure with all dependencies"

REM Push to GitHub
echo.
echo Pushing to GitHub...
git push -u origin main

if errorlevel 1 (
    echo Trying to push to master branch...
    git push -u origin master
)

echo.
echo ========================================
echo Repository Update Complete!
echo ========================================
echo.
echo All files have been added to:
echo https://github.com/amirakm12/Project-Human-Bot-IGED
echo.
echo Files included:
echo - Core system files (core/)
echo - Agent modules (agents/)
echo - Plugin system (plugins/)
echo - User interfaces (ui/, admin_panel/)
echo - Android client (android_client/, android-client/)
echo - Build and installation scripts
echo - Documentation files
echo - Configuration and logs
echo - Biometric authentication suite
echo - All subdirectories and their contents
echo.
echo The repository now contains the complete IGED project!
echo.

:end
pause 