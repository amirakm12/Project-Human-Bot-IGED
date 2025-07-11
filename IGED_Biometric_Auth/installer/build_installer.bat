@echo off
REM IGED Biometric Authentication Installer Builder
REM Builds executable installer for Windows

echo.
echo ========================================
echo   IGED Biometric Authentication Builder
echo ========================================
echo.

REM Set variables
set PROJECT_NAME=IGED_Biometric_Auth
set VERSION=1.0.0
set BUILD_DIR=build
set DIST_DIR=dist
set SPEC_FILE=iged_biometric.spec

REM Clean previous builds
echo [1/8] Cleaning previous builds...
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
if exist %DIST_DIR% rmdir /s /q %DIST_DIR%
if exist *.spec del *.spec

REM Check Python installation
echo [2/8] Checking Python installation...
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python not found in PATH
    echo Please install Python 3.8+ and add to PATH
    pause
    exit /b 1
)

REM Check PyInstaller
echo [3/8] Checking PyInstaller...
python -c "import PyInstaller" >nul 2>&1
if errorlevel 1 (
    echo Installing PyInstaller...
    pip install pyinstaller
)

REM Install dependencies
echo [4/8] Installing dependencies...
pip install -r requirements.txt
if errorlevel 1 (
    echo ERROR: Failed to install dependencies
    pause
    exit /b 1
)

REM Create PyInstaller spec file
echo [5/8] Creating PyInstaller spec file...
(
echo # -*- mode: python ; coding: utf-8 -*-
echo.
echo block_cipher = None
echo.
echo a = Analysis(
echo     ['iged_launcher.py'],
echo     pathex=[],
echo     binaries=[],
echo     datas=[
echo         ('windows_hello.py', '.'),
echo         ('biometric_manager_gui.py', '.'),
echo         ('webauthn_server.py', '.'),
echo         ('utils/*.py', 'utils'),
echo         ('config', 'config'),
echo         ('logs', 'logs'),
echo         ('README.md', '.'),
echo         ('requirements.txt', '.'),
echo     ],
echo     hiddenimports=[
echo         'winrt.windows.security.credentials.ui',
echo         'winrt.windows.foundation',
echo         'PySide6.QtWidgets',
echo         'PySide6.QtCore',
echo         'PySide6.QtGui',
echo         'flask',
echo         'flask_cors',
echo         'fido2',
echo         'cryptography',
echo         'colorama',
echo     ],
echo     hookspath=[],
echo     hooksconfig={},
echo     runtime_hooks=[],
echo     excludes=[],
echo     win_no_prefer_redirects=False,
echo     win_private_assemblies=False,
echo     cipher=block_cipher,
echo     noarchive=False,
echo ^)
echo.
echo pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher^)
echo.
echo exe = EXE(
echo     pyz,
echo     a.scripts,
echo     a.binaries,
echo     a.zipfiles,
echo     a.datas,
echo     [],
echo     name='IGED_Biometric_Launcher',
echo     debug=False,
echo     bootloader_ignore_signals=False,
echo     strip=False,
echo     upx=True,
echo     upx_exclude=[],
echo     runtime_tmpdir=None,
echo     console=False,
echo     disable_windowed_traceback=False,
echo     argv_emulation=False,
echo     target_arch=None,
echo     codesign_identity=None,
echo     entitlements_file=None,
echo     icon='icon.ico' if os.path.exists('icon.ico'^) else None,
echo ^)
echo.
echo # Create additional executables
echo exe_manager = EXE(
echo     pyz,
echo     ['biometric_manager_gui.py'],
echo     a.binaries,
echo     a.zipfiles,
echo     a.datas,
echo     [],
echo     name='IGED_Biometric_Manager',
echo     debug=False,
echo     bootloader_ignore_signals=False,
echo     strip=False,
echo     upx=True,
echo     upx_exclude=[],
echo     runtime_tmpdir=None,
echo     console=False,
echo     disable_windowed_traceback=False,
echo     argv_emulation=False,
echo     target_arch=None,
echo     codesign_identity=None,
echo     entitlements_file=None,
echo     icon='icon.ico' if os.path.exists('icon.ico'^) else None,
echo ^)
echo.
echo exe_server = EXE(
echo     pyz,
echo     ['webauthn_server.py'],
echo     a.binaries,
echo     a.zipfiles,
echo     a.datas,
echo     [],
echo     name='IGED_WebAuthn_Server',
echo     debug=False,
echo     bootloader_ignore_signals=False,
echo     strip=False,
echo     upx=True,
echo     upx_exclude=[],
echo     runtime_tmpdir=None,
echo     console=True,
echo     disable_windowed_traceback=False,
echo     argv_emulation=False,
echo     target_arch=None,
echo     codesign_identity=None,
echo     entitlements_file=None,
echo     icon='icon.ico' if os.path.exists('icon.ico'^) else None,
echo ^)
) > %SPEC_FILE%

REM Build executables
echo [6/8] Building executables with PyInstaller...
pyinstaller --clean --noconfirm %SPEC_FILE%
if errorlevel 1 (
    echo ERROR: PyInstaller build failed
    pause
    exit /b 1
)

REM Create installer directory structure
echo [7/8] Creating installer structure...
set INSTALLER_DIR=%DIST_DIR%\IGED_Biometric_Installer
mkdir "%INSTALLER_DIR%"
mkdir "%INSTALLER_DIR%\bin"
mkdir "%INSTALLER_DIR%\config"
mkdir "%INSTALLER_DIR%\logs"
mkdir "%INSTALLER_DIR%\docs"

REM Copy executables
copy "%DIST_DIR%\IGED_Biometric_Launcher.exe" "%INSTALLER_DIR%\bin\"
copy "%DIST_DIR%\IGED_Biometric_Manager.exe" "%INSTALLER_DIR%\bin\"
copy "%DIST_DIR%\IGED_WebAuthn_Server.exe" "%INSTALLER_DIR%\bin\"

REM Copy configuration files
if exist config\*.* copy config\*.* "%INSTALLER_DIR%\config\"

REM Copy documentation
copy README.md "%INSTALLER_DIR%\docs\"
copy requirements.txt "%INSTALLER_DIR%\"

REM Create batch files for easy execution
echo @echo off > "%INSTALLER_DIR%\Launch_IGED.bat"
echo echo Starting IGED with Biometric Authentication... >> "%INSTALLER_DIR%\Launch_IGED.bat"
echo cd /d "%%~dp0bin" >> "%INSTALLER_DIR%\Launch_IGED.bat"
echo IGED_Biometric_Launcher.exe >> "%INSTALLER_DIR%\Launch_IGED.bat"
echo pause >> "%INSTALLER_DIR%\Launch_IGED.bat"

echo @echo off > "%INSTALLER_DIR%\Biometric_Manager.bat"
echo echo Starting Biometric Manager... >> "%INSTALLER_DIR%\Biometric_Manager.bat"
echo cd /d "%%~dp0bin" >> "%INSTALLER_DIR%\Biometric_Manager.bat"
echo IGED_Biometric_Manager.exe >> "%INSTALLER_DIR%\Biometric_Manager.bat"
echo pause >> "%INSTALLER_DIR%\Biometric_Manager.bat"

echo @echo off > "%INSTALLER_DIR%\WebAuthn_Server.bat"
echo echo Starting WebAuthn Server... >> "%INSTALLER_DIR%\WebAuthn_Server.bat"
echo cd /d "%%~dp0bin" >> "%INSTALLER_DIR%\WebAuthn_Server.bat"
echo IGED_WebAuthn_Server.exe >> "%INSTALLER_DIR%\WebAuthn_Server.bat"
echo pause >> "%INSTALLER_DIR%\WebAuthn_Server.bat"

REM Create installation script
echo @echo off > "%INSTALLER_DIR%\install.bat"
echo echo Installing IGED Biometric Authentication... >> "%INSTALLER_DIR%\install.bat"
echo echo. >> "%INSTALLER_DIR%\install.bat"
echo echo This will install IGED Biometric Authentication to your system. >> "%INSTALLER_DIR%\install.bat"
echo echo. >> "%INSTALLER_DIR%\install.bat"
echo set /p INSTALL_DIR="Enter installation directory (default: C:\Program Files\IGED Biometric): " >> "%INSTALLER_DIR%\install.bat"
echo if "%%INSTALL_DIR%%"=="" set INSTALL_DIR=C:\Program Files\IGED Biometric >> "%INSTALLER_DIR%\install.bat"
echo echo. >> "%INSTALLER_DIR%\install.bat"
echo echo Installing to: %%INSTALL_DIR%% >> "%INSTALLER_DIR%\install.bat"
echo echo. >> "%INSTALLER_DIR%\install.bat"
echo mkdir "%%INSTALL_DIR%%" 2^>nul >> "%INSTALLER_DIR%\install.bat"
echo xcopy /s /e /y "%%~dp0*" "%%INSTALL_DIR%%\" >> "%INSTALLER_DIR%\install.bat"
echo echo. >> "%INSTALLER_DIR%\install.bat"
echo echo Installation complete! >> "%INSTALLER_DIR%\install.bat"
echo echo You can now run: "%%INSTALL_DIR%%\Launch_IGED.bat" >> "%INSTALLER_DIR%\install.bat"
echo pause >> "%INSTALLER_DIR%\install.bat"

REM Create uninstall script
echo @echo off > "%INSTALLER_DIR%\uninstall.bat"
echo echo Uninstalling IGED Biometric Authentication... >> "%INSTALLER_DIR%\uninstall.bat"
echo echo. >> "%INSTALLER_DIR%\uninstall.bat"
echo set /p INSTALL_DIR="Enter installation directory to remove: " >> "%INSTALLER_DIR%\uninstall.bat"
echo echo. >> "%INSTALLER_DIR%\uninstall.bat"
echo echo Removing: %%INSTALL_DIR%% >> "%INSTALLER_DIR%\uninstall.bat"
echo echo. >> "%INSTALLER_DIR%\uninstall.bat"
echo rmdir /s /q "%%INSTALL_DIR%%" >> "%INSTALLER_DIR%\uninstall.bat"
echo echo Uninstallation complete! >> "%INSTALLER_DIR%\uninstall.bat"
echo pause >> "%INSTALLER_DIR%\uninstall.bat"

REM Create README for installer
echo IGED Biometric Authentication v%VERSION% > "%INSTALLER_DIR%\INSTALLER_README.txt"
echo ========================================== >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo. >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo Installation Instructions: >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo 1. Run install.bat to install to your system >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo 2. Run Launch_IGED.bat to start IGED with biometric authentication >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo 3. Run Biometric_Manager.bat to manage biometric credentials >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo 4. Run WebAuthn_Server.bat to start the WebAuthn server >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo. >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo System Requirements: >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo - Windows 10+ with biometric hardware >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo - Windows Hello configured >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo - Internet connection for WebAuthn features >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo. >> "%INSTALLER_DIR%\INSTALLER_README.txt"
echo For more information, see docs\README.md >> "%INSTALLER_DIR%\INSTALLER_README.txt"

REM Create ZIP archive
echo [8/8] Creating installer archive...
cd %DIST_DIR%
powershell -command "Compress-Archive -Path 'IGED_Biometric_Installer' -DestinationPath 'IGED_Biometric_Auth_v%VERSION%.zip' -Force"
cd ..

REM Cleanup
echo Cleaning up build files...
rmdir /s /q %BUILD_DIR%
del %SPEC_FILE%

echo.
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo Executables created in: %DIST_DIR%\IGED_Biometric_Installer\bin\
echo Installer package: %DIST_DIR%\IGED_Biometric_Auth_v%VERSION%.zip
echo.
echo To install:
echo 1. Extract IGED_Biometric_Auth_v%VERSION%.zip
echo 2. Run install.bat
echo 3. Launch IGED with biometric authentication
echo.
pause 