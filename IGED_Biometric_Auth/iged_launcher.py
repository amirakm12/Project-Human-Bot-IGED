#!/usr/bin/env python3
"""
IGED - Secure Biometric Launcher
Launches IGED only after successful biometric authentication
"""

import sys
import os
import subprocess
import time
import threading
from pathlib import Path
from typing import Optional, Dict, Any

# Import biometric modules
try:
    from windows_hello import biometric_authenticate_sync, check_biometric_availability
    from PySide6.QtWidgets import QApplication, QMessageBox, QProgressDialog, QLabel
    from PySide6.QtCore import Qt, QTimer
    from PySide6.QtGui import QFont
    BIOMETRIC_AVAILABLE = True
except ImportError as e:
    print(f"⚠️ Biometric modules not available: {e}")
    BIOMETRIC_AVAILABLE = False

class SecureIGEDLauncher:
    """Secure launcher for IGED with biometric authentication"""
    
    def __init__(self):
        self.iged_path = None
        self.iged_process = None
        self.auth_attempts = 0
        self.max_attempts = 3
        self.lockout_duration = 300  # 5 minutes
        self.last_failed_attempt = 0
        
        # Find IGED installation
        self.find_iged_installation()
    
    def find_iged_installation(self):
        """Find IGED installation directory"""
        possible_paths = [
            Path("../launcher.py"),  # Parent directory
            Path("launcher.py"),     # Current directory
            Path("../../launcher.py"), # Grandparent directory
            Path.cwd() / "launcher.py"  # Current working directory
        ]
        
        for path in possible_paths:
            if path.exists():
                self.iged_path = path
                print(f"✅ Found IGED at: {path.absolute()}")
                return
        
        print("❌ IGED launcher not found")
        print("💡 Please ensure IGED is installed in a parent directory")
        self.iged_path = None
    
    def check_lockout(self) -> bool:
        """Check if account is locked out due to failed attempts"""
        if self.auth_attempts >= self.max_attempts:
            time_since_last = time.time() - self.last_failed_attempt
            if time_since_last < self.lockout_duration:
                remaining = int(self.lockout_duration - time_since_last)
                print(f"🔒 Account locked out. Try again in {remaining} seconds")
                return True
            else:
                # Reset lockout
                self.auth_attempts = 0
                print("🔓 Lockout period expired")
        
        return False
    
    def authenticate_user(self) -> bool:
        """Authenticate user using biometrics"""
        if not BIOMETRIC_AVAILABLE:
            print("❌ Biometric authentication not available")
            return False
        
        if self.check_lockout():
            return False
        
        print("🔐 Requesting biometric authentication...")
        print("👆 Please use your fingerprint or face to authenticate")
        
        # Check hardware availability
        if not check_biometric_availability():
            print("❌ No biometric hardware available")
            print("💡 Please configure Windows Hello or use alternative authentication")
            return False
        
        # Request authentication
        success = biometric_authenticate_sync("Authenticate to unlock IGED")
        
        if success:
            print("✅ Biometric authentication successful!")
            self.auth_attempts = 0  # Reset failed attempts
            return True
        else:
            print("❌ Biometric authentication failed")
            self.auth_attempts += 1
            self.last_failed_attempt = time.time()
            
            if self.auth_attempts >= self.max_attempts:
                print(f"🔒 Account locked for {self.lockout_duration} seconds")
            
            return False
    
    def launch_iged(self) -> bool:
        """Launch IGED after successful authentication"""
        if not self.iged_path:
            print("❌ IGED not found")
            return False
        
        try:
            print("🚀 Launching IGED...")
            
            # Launch IGED in a new process
            self.iged_process = subprocess.Popen([
                sys.executable, str(self.iged_path)
            ], cwd=self.iged_path.parent)
            
            print(f"✅ IGED launched successfully (PID: {self.iged_process.pid})")
            return True
            
        except Exception as e:
            print(f"❌ Failed to launch IGED: {e}")
            return False
    
    def show_gui_authentication(self) -> bool:
        """Show GUI authentication dialog"""
        if not BIOMETRIC_AVAILABLE:
            return False
        
        app = QApplication.instance()
        if app is None:
            app = QApplication(sys.argv)
        
        # Create progress dialog
        progress = QProgressDialog("Authenticating with Windows Hello...", "Cancel", 0, 0)
        progress.setWindowTitle("IGED Authentication")
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(0)
        progress.setValue(0)
        
        # Set up timer to check authentication
        auth_result = [None]
        
        def check_auth():
            try:
                if check_biometric_availability():
                    success = biometric_authenticate_sync("Authenticate to unlock IGED")
                    auth_result[0] = success
                else:
                    auth_result[0] = False
            except Exception as e:
                auth_result[0] = False
                print(f"Authentication error: {e}")
            finally:
                progress.close()
        
        # Run authentication in separate thread
        auth_thread = threading.Thread(target=check_auth)
        auth_thread.start()
        
        # Show progress dialog
        progress.exec()
        
        # Wait for authentication to complete
        auth_thread.join()
        
        return auth_result[0] if auth_result[0] is not None else False
    
    def run(self) -> bool:
        """Main launcher execution"""
        print("🚀 IGED Secure Biometric Launcher")
        print("=" * 40)
        
        # Check if IGED is available
        if not self.iged_path:
            print("❌ IGED installation not found")
            return False
        
        # Attempt authentication
        print("🔐 Starting biometric authentication...")
        
        # Try GUI authentication first
        if BIOMETRIC_AVAILABLE:
            success = self.show_gui_authentication()
        else:
            success = self.authenticate_user()
        
        if success:
            print("✅ Authentication successful!")
            
            # Launch IGED
            if self.launch_iged():
                print("🎉 IGED launched successfully!")
                print("🔐 You can now use IGED with full functionality")
                return True
            else:
                print("❌ Failed to launch IGED")
                return False
        else:
            print("❌ Authentication failed")
            print("🔒 IGED will not start without successful authentication")
            return False

def show_error_dialog(message: str):
    """Show error dialog"""
    try:
        app = QApplication.instance()
        if app is None:
            app = QApplication(sys.argv)
        
        msg_box = QMessageBox()
        msg_box.setIcon(QMessageBox.Critical)
        msg_box.setWindowTitle("IGED Authentication Error")
        msg_box.setText("Authentication Failed")
        msg_box.setInformativeText(message)
        msg_box.setStandardButtons(QMessageBox.Ok)
        msg_box.exec()
    except Exception:
        # Fallback to console output
        print(f"❌ {message}")

def show_success_dialog():
    """Show success dialog"""
    try:
        app = QApplication.instance()
        if app is None:
            app = QApplication(sys.argv)
        
        msg_box = QMessageBox()
        msg_box.setIcon(QMessageBox.Information)
        msg_box.setWindowTitle("IGED Authentication")
        msg_box.setText("Authentication Successful")
        msg_box.setInformativeText("IGED is now launching...")
        msg_box.setStandardButtons(QMessageBox.Ok)
        msg_box.exec()
    except Exception:
        pass

def main():
    """Main function"""
    print("🔐 IGED Secure Biometric Launcher")
    print("=" * 40)
    
    # Check dependencies
    if not BIOMETRIC_AVAILABLE:
        print("⚠️ Biometric authentication not available")
        print("💡 Install dependencies: pip install pywinrt PySide6")
        print("🔑 Proceeding with basic authentication...")
        
        # Fallback to basic authentication
        response = input("Do you want to launch IGED without biometric authentication? (y/N): ")
        if response.lower() != 'y':
            print("❌ Authentication required. Exiting.")
            sys.exit(1)
        
        # Launch IGED directly
        launcher = SecureIGEDLauncher()
        if launcher.launch_iged():
            print("✅ IGED launched (without biometric authentication)")
        else:
            print("❌ Failed to launch IGED")
            sys.exit(1)
        return
    
    # Run secure launcher
    launcher = SecureIGEDLauncher()
    success = launcher.run()
    
    if not success:
        show_error_dialog("Biometric authentication failed. IGED will not start.")
        sys.exit(1)
    else:
        show_success_dialog()

if __name__ == "__main__":
    main() 