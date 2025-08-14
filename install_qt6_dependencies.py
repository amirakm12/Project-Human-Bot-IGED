#!/usr/bin/env python3
"""
IGED Qt6 Dependencies Installer
Comprehensive installer for all Qt6 and application dependencies
Ensures optimal configuration for peak performance
"""

import sys
import os
import subprocess
import platform
import json
import argparse
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import urllib.request
import tempfile
import shutil

class Qt6DependencyInstaller:
    """Manages Qt6 and related dependency installation"""
    
    def __init__(self, verbose: bool = False):
        self.verbose = verbose
        self.platform = platform.system()
        self.python_version = sys.version_info
        self.installed_packages = []
        self.failed_packages = []
        
    def log(self, message: str, level: str = "INFO"):
        """Log messages with formatting"""
        symbols = {
            "INFO": "ℹ️",
            "SUCCESS": "✅",
            "WARNING": "⚠️",
            "ERROR": "❌",
            "PROGRESS": "⏳"
        }
        symbol = symbols.get(level, "•")
        print(f"{symbol} {message}")
    
    def check_python_version(self) -> bool:
        """Verify Python version compatibility"""
        min_version = (3, 8)
        if self.python_version < min_version:
            self.log(f"Python {min_version[0]}.{min_version[1]}+ required, found {sys.version}", "ERROR")
            return False
        
        self.log(f"Python {sys.version} detected", "SUCCESS")
        return True
    
    def upgrade_pip(self) -> bool:
        """Upgrade pip to latest version"""
        try:
            self.log("Upgrading pip to latest version...", "PROGRESS")
            result = subprocess.run(
                [sys.executable, "-m", "pip", "install", "--upgrade", "pip"],
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                self.log("pip upgraded successfully", "SUCCESS")
                return True
            else:
                self.log(f"Failed to upgrade pip: {result.stderr}", "WARNING")
                return False
                
        except Exception as e:
            self.log(f"Error upgrading pip: {e}", "ERROR")
            return False
    
    def install_package(self, package: str, required: bool = True) -> bool:
        """Install a single package"""
        try:
            self.log(f"Installing {package}...", "PROGRESS")
            
            result = subprocess.run(
                [sys.executable, "-m", "pip", "install", package],
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                self.log(f"{package} installed successfully", "SUCCESS")
                self.installed_packages.append(package)
                return True
            else:
                if required:
                    self.log(f"Failed to install {package}: {result.stderr}", "ERROR")
                    self.failed_packages.append(package)
                else:
                    self.log(f"Optional package {package} could not be installed", "WARNING")
                return False
                
        except Exception as e:
            self.log(f"Error installing {package}: {e}", "ERROR")
            if required:
                self.failed_packages.append(package)
            return False
    
    def get_qt6_packages(self) -> Dict[str, List[str]]:
        """Get Qt6 packages based on platform"""
        packages = {
            "core": [
                "PySide6>=6.6.1",
                "PySide6-Essentials>=6.6.1",
                "PySide6-Addons>=6.6.1"
            ],
            "optional": [
                "qt6-tools",
                "qt6-applications"
            ],
            "development": [
                "pyside6-tools",
                "qt-material"
            ]
        }
        
        # Platform-specific packages
        if self.platform == "Windows":
            packages["platform"] = [
                "pywin32>=306"
            ]
        elif self.platform == "Linux":
            packages["platform"] = [
                "PyQt6-Qt6>=6.6.0"
            ]
        elif self.platform == "Darwin":  # macOS
            packages["platform"] = []
        
        return packages
    
    def get_app_dependencies(self) -> Dict[str, List[str]]:
        """Get application dependencies"""
        return {
            "core": [
                "cryptography>=3.4.8",
                "flask>=2.3.3",
                "flask-cors>=4.0.0",
                "requests>=2.31.0",
                "psutil>=5.9.0"
            ],
            "data": [
                "pandas>=2.0.0",
                "numpy>=1.24.0",
                "matplotlib>=3.7.0",
                "seaborn>=0.12.0"
            ],
            "voice": [
                "SpeechRecognition>=3.10.0",
                "PyAudio>=0.2.11"
            ],
            "build": [
                "pyinstaller>=5.13.0",
                "setuptools>=68.0.0",
                "wheel>=0.41.0"
            ]
        }
    
    def check_system_requirements(self) -> bool:
        """Check system requirements for Qt6"""
        self.log("Checking system requirements...", "INFO")
        
        requirements_met = True
        
        # Check display availability
        if self.platform == "Linux":
            if not os.environ.get('DISPLAY') and not os.environ.get('WAYLAND_DISPLAY'):
                self.log("No display detected. GUI applications may not work.", "WARNING")
        
        # Check OpenGL support
        try:
            if self.platform == "Windows":
                # Windows usually has OpenGL support
                self.log("OpenGL support assumed on Windows", "INFO")
            elif self.platform == "Linux":
                # Check for OpenGL libraries
                result = subprocess.run(
                    ["ldconfig", "-p"],
                    capture_output=True,
                    text=True
                )
                if "libGL" in result.stdout:
                    self.log("OpenGL libraries found", "SUCCESS")
                else:
                    self.log("OpenGL libraries may be missing", "WARNING")
        except Exception as e:
            self.log(f"Could not check OpenGL: {e}", "WARNING")
        
        # Check available disk space
        try:
            import shutil
            total, used, free = shutil.disk_usage("/")
            free_gb = free / (1024 ** 3)
            
            if free_gb < 1:
                self.log(f"Low disk space: {free_gb:.2f} GB available", "WARNING")
                requirements_met = False
            else:
                self.log(f"Disk space: {free_gb:.2f} GB available", "SUCCESS")
        except Exception as e:
            self.log(f"Could not check disk space: {e}", "WARNING")
        
        return requirements_met
    
    def install_qt6_dependencies(self) -> bool:
        """Install all Qt6 dependencies"""
        self.log("Installing Qt6 dependencies...", "INFO")
        
        packages = self.get_qt6_packages()
        success = True
        
        # Install core packages
        for package in packages["core"]:
            if not self.install_package(package, required=True):
                success = False
        
        # Install optional packages
        if "optional" in packages:
            for package in packages["optional"]:
                self.install_package(package, required=False)
        
        # Install platform-specific packages
        if "platform" in packages:
            for package in packages["platform"]:
                self.install_package(package, required=False)
        
        return success
    
    def install_app_dependencies(self) -> bool:
        """Install application dependencies"""
        self.log("Installing application dependencies...", "INFO")
        
        dependencies = self.get_app_dependencies()
        success = True
        
        # Install core dependencies
        for package in dependencies["core"]:
            if not self.install_package(package, required=True):
                success = False
        
        # Install optional dependencies
        for category in ["data", "voice", "build"]:
            if category in dependencies:
                for package in dependencies[category]:
                    self.install_package(package, required=False)
        
        return success
    
    def verify_installation(self) -> bool:
        """Verify Qt6 installation"""
        self.log("Verifying Qt6 installation...", "INFO")
        
        try:
            # Try importing PySide6
            import PySide6
            from PySide6.QtCore import QT_VERSION_STR
            from PySide6.QtWidgets import QApplication
            
            self.log(f"PySide6 version: {PySide6.__version__}", "SUCCESS")
            self.log(f"Qt version: {QT_VERSION_STR}", "SUCCESS")
            
            # Test creating application
            app = QApplication.instance()
            if app is None:
                test_app = QApplication([])
                test_app.quit()
            
            self.log("Qt6 installation verified successfully", "SUCCESS")
            return True
            
        except ImportError as e:
            self.log(f"Qt6 verification failed: {e}", "ERROR")
            return False
        except Exception as e:
            self.log(f"Qt6 test failed: {e}", "ERROR")
            return False
    
    def create_config_file(self):
        """Create configuration file for Qt6 settings"""
        config = {
            "qt6": {
                "version": "6.6.1",
                "backend": "PySide6",
                "installed": True,
                "verified": self.verify_installation()
            },
            "optimizations": {
                "hardware_acceleration": True,
                "opengl": True,
                "multithreading": True,
                "cache_enabled": True
            },
            "packages": {
                "installed": self.installed_packages,
                "failed": self.failed_packages
            }
        }
        
        config_path = Path("config/qt6_config.json")
        config_path.parent.mkdir(exist_ok=True)
        
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        self.log(f"Configuration saved to {config_path}", "SUCCESS")
    
    def run_full_installation(self) -> bool:
        """Run complete installation process"""
        print("\n" + "=" * 60)
        print("IGED Qt6 Dependencies Installer")
        print("=" * 60 + "\n")
        
        # Check Python version
        if not self.check_python_version():
            return False
        
        # Check system requirements
        self.check_system_requirements()
        
        # Upgrade pip
        self.upgrade_pip()
        
        # Install Qt6
        qt6_success = self.install_qt6_dependencies()
        
        # Install app dependencies
        app_success = self.install_app_dependencies()
        
        # Verify installation
        verified = self.verify_installation()
        
        # Create config file
        self.create_config_file()
        
        # Print summary
        print("\n" + "=" * 60)
        print("Installation Summary")
        print("=" * 60)
        
        if self.installed_packages:
            print(f"\n✅ Successfully installed ({len(self.installed_packages)} packages):")
            for pkg in self.installed_packages[:10]:  # Show first 10
                print(f"  • {pkg}")
            if len(self.installed_packages) > 10:
                print(f"  ... and {len(self.installed_packages) - 10} more")
        
        if self.failed_packages:
            print(f"\n❌ Failed to install ({len(self.failed_packages)} packages):")
            for pkg in self.failed_packages:
                print(f"  • {pkg}")
        
        if verified:
            print("\n✅ Qt6 installation verified and ready to use!")
        else:
            print("\n⚠️ Qt6 installation could not be verified")
        
        return qt6_success and app_success

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="Install Qt6 and dependencies for IGED"
    )
    
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--qt6-only', action='store_true',
                       help='Install only Qt6 dependencies')
    parser.add_argument('--app-only', action='store_true',
                       help='Install only application dependencies')
    parser.add_argument('--verify', action='store_true',
                       help='Only verify existing installation')
    
    args = parser.parse_args()
    
    installer = Qt6DependencyInstaller(verbose=args.verbose)
    
    if args.verify:
        success = installer.verify_installation()
    elif args.qt6_only:
        installer.upgrade_pip()
        success = installer.install_qt6_dependencies()
        installer.verify_installation()
    elif args.app_only:
        installer.upgrade_pip()
        success = installer.install_app_dependencies()
    else:
        success = installer.run_full_installation()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()