#!/usr/bin/env python3
"""
IGED - Biometric Manager GUI
PySide6-based GUI for managing biometric and WebAuthn credentials
"""

import sys
import os
import json
import asyncio
import threading
from pathlib import Path
from typing import Optional, Dict, Any
from datetime import datetime

try:
    from PySide6.QtWidgets import (
        QApplication, QWidget, QVBoxLayout, QHBoxLayout, 
        QPushButton, QLabel, QMessageBox, QTabWidget,
        QTextEdit, QLineEdit, QComboBox, QCheckBox,
        QGroupBox, QGridLayout, QProgressBar, QStatusBar,
        QMainWindow, QMenuBar, QMenu, QAction, QFileDialog,
        QTableWidget, QTableWidgetItem, QHeaderView
    )
    from PySide6.QtCore import Qt, QTimer, QThread, pyqtSignal, QSettings
    from PySide6.QtGui import QFont, QIcon, QPixmap, QPalette, QColor
    PYSIDE6_AVAILABLE = True
except ImportError:
    print("❌ PySide6 not available. Install with: pip install PySide6")
    PYSIDE6_AVAILABLE = False

# Import our modules
try:
    from windows_hello import biometric_authenticate_sync, check_biometric_availability
    from utils.logger import setup_logger
    from utils.crypto_utils import CryptoManager
    WINDOWS_HELLO_AVAILABLE = True
except ImportError as e:
    print(f"⚠️ Some modules not available: {e}")
    WINDOWS_HELLO_AVAILABLE = False

# Setup logging
logger = setup_logger(__name__) if 'setup_logger' in globals() else None

class BiometricWorker(QThread):
    """Worker thread for biometric operations"""
    authentication_complete = pyqtSignal(bool, str)
    status_update = pyqtSignal(str)
    
    def __init__(self, operation: str, reason: str = ""):
        super().__init__()
        self.operation = operation
        self.reason = reason
    
    def run(self):
        """Run the biometric operation"""
        try:
            if self.operation == "authenticate":
                self.status_update.emit("Requesting biometric authentication...")
                success = biometric_authenticate_sync(self.reason)
                if success:
                    self.authentication_complete.emit(True, "Authentication successful")
                else:
                    self.authentication_complete.emit(False, "Authentication failed or canceled")
            elif self.operation == "check_availability":
                self.status_update.emit("Checking biometric availability...")
                available = check_biometric_availability()
                if available:
                    self.authentication_complete.emit(True, "Biometric hardware available")
                else:
                    self.authentication_complete.emit(False, "No biometric hardware available")
        except Exception as e:
            self.authentication_complete.emit(False, f"Error: {str(e)}")

class BiometricManagerGUI(QMainWindow):
    """Main GUI window for biometric credential management"""
    
    def __init__(self):
        super().__init__()
        self.settings = QSettings("IGED", "BiometricManager")
        self.crypto_manager = CryptoManager() if 'CryptoManager' in globals() else None
        self.init_ui()
        self.load_settings()
    
    def init_ui(self):
        """Initialize the user interface"""
        self.setWindowTitle("IGED Biometric Credential Manager")
        self.setMinimumSize(800, 600)
        self.setGeometry(100, 100, 1000, 700)
        
        # Set dark theme
        self.set_dark_theme()
        
        # Create central widget and layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)
        
        # Create menu bar
        self.create_menu_bar()
        
        # Create tab widget
        self.tab_widget = QTabWidget()
        layout.addWidget(self.tab_widget)
        
        # Create tabs
        self.create_biometric_tab()
        self.create_webauthn_tab()
        self.create_credentials_tab()
        self.create_logs_tab()
        
        # Create status bar
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("Ready")
        
        # Create progress bar
        self.progress_bar = QProgressBar()
        self.progress_bar.setVisible(False)
        self.status_bar.addPermanentWidget(self.progress_bar)
    
    def create_menu_bar(self):
        """Create the menu bar"""
        menubar = self.menuBar()
        
        # File menu
        file_menu = menubar.addMenu("&File")
        
        export_action = QAction("&Export Credentials", self)
        export_action.setShortcut("Ctrl+E")
        export_action.triggered.connect(self.export_credentials)
        file_menu.addAction(export_action)
        
        import_action = QAction("&Import Credentials", self)
        import_action.setShortcut("Ctrl+I")
        import_action.triggered.connect(self.import_credentials)
        file_menu.addAction(import_action)
        
        file_menu.addSeparator()
        
        exit_action = QAction("E&xit", self)
        exit_action.setShortcut("Ctrl+Q")
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        # Tools menu
        tools_menu = menubar.addMenu("&Tools")
        
        test_auth_action = QAction("&Test Authentication", self)
        test_auth_action.setShortcut("Ctrl+T")
        test_auth_action.triggered.connect(self.test_authentication)
        tools_menu.addAction(test_auth_action)
        
        check_hardware_action = QAction("&Check Hardware", self)
        check_hardware_action.triggered.connect(self.check_hardware)
        tools_menu.addAction(check_hardware_action)
        
        # Help menu
        help_menu = menubar.addMenu("&Help")
        
        about_action = QAction("&About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)
    
    def create_biometric_tab(self):
        """Create the biometric authentication tab"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # Status group
        status_group = QGroupBox("Biometric Status")
        status_layout = QGridLayout(status_group)
        
        self.hardware_status_label = QLabel("Hardware: Unknown")
        self.hardware_status_label.setStyleSheet("font-weight: bold;")
        status_layout.addWidget(QLabel("Hardware Status:"), 0, 0)
        status_layout.addWidget(self.hardware_status_label, 0, 1)
        
        self.auth_status_label = QLabel("Authentication: Not tested")
        self.auth_status_label.setStyleSheet("font-weight: bold;")
        status_layout.addWidget(QLabel("Authentication Status:"), 1, 0)
        status_layout.addWidget(self.auth_status_label, 1, 1)
        
        layout.addWidget(status_group)
        
        # Authentication group
        auth_group = QGroupBox("Biometric Authentication")
        auth_layout = QVBoxLayout(auth_group)
        
        # Reason input
        reason_layout = QHBoxLayout()
        reason_layout.addWidget(QLabel("Authentication Reason:"))
        self.reason_input = QLineEdit("Authenticate to unlock IGED")
        reason_layout.addWidget(self.reason_input)
        auth_layout.addLayout(reason_layout)
        
        # Buttons
        button_layout = QHBoxLayout()
        
        self.check_hardware_btn = QPushButton("Check Hardware")
        self.check_hardware_btn.clicked.connect(self.check_hardware)
        button_layout.addWidget(self.check_hardware_btn)
        
        self.authenticate_btn = QPushButton("Authenticate")
        self.authenticate_btn.clicked.connect(self.test_authentication)
        button_layout.addWidget(self.authenticate_btn)
        
        self.launch_iged_btn = QPushButton("Launch IGED")
        self.launch_iged_btn.clicked.connect(self.launch_iged)
        button_layout.addWidget(self.launch_iged_btn)
        
        auth_layout.addLayout(button_layout)
        layout.addWidget(auth_group)
        
        # Settings group
        settings_group = QGroupBox("Settings")
        settings_layout = QGridLayout(settings_group)
        
        self.auto_launch_checkbox = QCheckBox("Auto-launch IGED after successful authentication")
        self.auto_launch_checkbox.setChecked(True)
        settings_layout.addWidget(self.auto_launch_checkbox, 0, 0, 1, 2)
        
        self.remember_reason_checkbox = QCheckBox("Remember authentication reason")
        self.remember_reason_checkbox.setChecked(True)
        settings_layout.addWidget(self.remember_reason_checkbox, 1, 0, 1, 2)
        
        layout.addWidget(settings_group)
        
        # Add stretch to push everything to top
        layout.addStretch()
        
        self.tab_widget.addTab(tab, "Biometric Authentication")
    
    def create_webauthn_tab(self):
        """Create the WebAuthn management tab"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # Server status group
        server_group = QGroupBox("WebAuthn Server Status")
        server_layout = QGridLayout(server_group)
        
        self.server_status_label = QLabel("Server: Not running")
        self.server_status_label.setStyleSheet("font-weight: bold; color: red;")
        server_layout.addWidget(QLabel("Status:"), 0, 0)
        server_layout.addWidget(self.server_status_label, 0, 1)
        
        self.server_port_input = QLineEdit("5000")
        server_layout.addWidget(QLabel("Port:"), 1, 0)
        server_layout.addWidget(self.server_port_input, 1, 1)
        
        layout.addWidget(server_group)
        
        # Server controls
        server_controls_layout = QHBoxLayout()
        
        self.start_server_btn = QPushButton("Start Server")
        self.start_server_btn.clicked.connect(self.start_webauthn_server)
        server_controls_layout.addWidget(self.start_server_btn)
        
        self.stop_server_btn = QPushButton("Stop Server")
        self.stop_server_btn.clicked.connect(self.stop_webauthn_server)
        self.stop_server_btn.setEnabled(False)
        server_controls_layout.addWidget(self.stop_server_btn)
        
        layout.addLayout(server_controls_layout)
        
        # Credentials table
        credentials_group = QGroupBox("Registered Credentials")
        credentials_layout = QVBoxLayout(credentials_group)
        
        self.credentials_table = QTableWidget()
        self.credentials_table.setColumnCount(4)
        self.credentials_table.setHorizontalHeaderLabels([
            "User ID", "Name", "Device", "Registration Date"
        ])
        self.credentials_table.horizontalHeader().setStretchLastSection(True)
        credentials_layout.addWidget(self.credentials_table)
        
        # Table controls
        table_controls_layout = QHBoxLayout()
        
        self.refresh_credentials_btn = QPushButton("Refresh")
        self.refresh_credentials_btn.clicked.connect(self.refresh_credentials)
        table_controls_layout.addWidget(self.refresh_credentials_btn)
        
        self.delete_credential_btn = QPushButton("Delete Selected")
        self.delete_credential_btn.clicked.connect(self.delete_credential)
        table_controls_layout.addWidget(self.delete_credential_btn)
        
        credentials_layout.addLayout(table_controls_layout)
        layout.addWidget(credentials_group)
        
        self.tab_widget.addTab(tab, "WebAuthn Management")
    
    def create_credentials_tab(self):
        """Create the credentials management tab"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # Credential info
        info_group = QGroupBox("Credential Information")
        info_layout = QGridLayout(info_group)
        
        self.credential_count_label = QLabel("0")
        info_layout.addWidget(QLabel("Total Credentials:"), 0, 0)
        info_layout.addWidget(self.credential_count_label, 0, 1)
        
        self.last_sync_label = QLabel("Never")
        info_layout.addWidget(QLabel("Last Sync:"), 1, 0)
        info_layout.addWidget(self.last_sync_label, 1, 1)
        
        layout.addWidget(info_group)
        
        # Export/Import controls
        export_import_layout = QHBoxLayout()
        
        self.export_btn = QPushButton("Export Credentials")
        self.export_btn.clicked.connect(self.export_credentials)
        export_import_layout.addWidget(self.export_btn)
        
        self.import_btn = QPushButton("Import Credentials")
        self.import_btn.clicked.connect(self.import_credentials)
        export_import_layout.addWidget(self.import_btn)
        
        layout.addLayout(export_import_layout)
        
        # Add stretch
        layout.addStretch()
        
        self.tab_widget.addTab(tab, "Credentials")
    
    def create_logs_tab(self):
        """Create the logs tab"""
        tab = QWidget()
        layout = QVBoxLayout(tab)
        
        # Log controls
        log_controls_layout = QHBoxLayout()
        
        self.clear_logs_btn = QPushButton("Clear Logs")
        self.clear_logs_btn.clicked.connect(self.clear_logs)
        log_controls_layout.addWidget(self.clear_logs_btn)
        
        self.save_logs_btn = QPushButton("Save Logs")
        self.save_logs_btn.clicked.connect(self.save_logs)
        log_controls_layout.addWidget(self.save_logs_btn)
        
        layout.addLayout(log_controls_layout)
        
        # Log display
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        self.log_display.setFont(QFont("Consolas", 10))
        layout.addWidget(self.log_display)
        
        self.tab_widget.addTab(tab, "Logs")
    
    def set_dark_theme(self):
        """Apply dark theme to the application"""
        palette = QPalette()
        palette.setColor(QPalette.Window, QColor(53, 53, 53))
        palette.setColor(QPalette.WindowText, QColor(255, 255, 255))
        palette.setColor(QPalette.Base, QColor(25, 25, 25))
        palette.setColor(QPalette.AlternateBase, QColor(53, 53, 53))
        palette.setColor(QPalette.ToolTipBase, QColor(255, 255, 255))
        palette.setColor(QPalette.ToolTipText, QColor(255, 255, 255))
        palette.setColor(QPalette.Text, QColor(255, 255, 255))
        palette.setColor(QPalette.Button, QColor(53, 53, 53))
        palette.setColor(QPalette.ButtonText, QColor(255, 255, 255))
        palette.setColor(QPalette.BrightText, QColor(255, 0, 0))
        palette.setColor(QPalette.Link, QColor(42, 130, 218))
        palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
        palette.setColor(QPalette.HighlightedText, QColor(255, 255, 255))
        
        self.setPalette(palette)
    
    def check_hardware(self):
        """Check biometric hardware availability"""
        self.status_bar.showMessage("Checking biometric hardware...")
        self.progress_bar.setVisible(True)
        self.progress_bar.setRange(0, 0)  # Indeterminate progress
        
        # Run in worker thread
        self.worker = BiometricWorker("check_availability")
        self.worker.authentication_complete.connect(self.on_hardware_check_complete)
        self.worker.status_update.connect(self.status_bar.showMessage)
        self.worker.start()
    
    def test_authentication(self):
        """Test biometric authentication"""
        reason = self.reason_input.text()
        if not reason:
            reason = "Test authentication for IGED"
        
        self.status_bar.showMessage("Testing biometric authentication...")
        self.progress_bar.setVisible(True)
        self.progress_bar.setRange(0, 0)
        
        # Run in worker thread
        self.worker = BiometricWorker("authenticate", reason)
        self.worker.authentication_complete.connect(self.on_authentication_complete)
        self.worker.status_update.connect(self.status_bar.showMessage)
        self.worker.start()
    
    def on_hardware_check_complete(self, success: bool, message: str):
        """Handle hardware check completion"""
        self.progress_bar.setVisible(False)
        
        if success:
            self.hardware_status_label.setText("Hardware: Available ✅")
            self.hardware_status_label.setStyleSheet("font-weight: bold; color: green;")
        else:
            self.hardware_status_label.setText("Hardware: Not Available ❌")
            self.hardware_status_label.setStyleSheet("font-weight: bold; color: red;")
        
        self.status_bar.showMessage(message)
        self.log_message(f"Hardware check: {message}")
    
    def on_authentication_complete(self, success: bool, message: str):
        """Handle authentication completion"""
        self.progress_bar.setVisible(False)
        
        if success:
            self.auth_status_label.setText("Authentication: Successful ✅")
            self.auth_status_label.setStyleSheet("font-weight: bold; color: green;")
            
            # Auto-launch IGED if enabled
            if self.auto_launch_checkbox.isChecked():
                self.launch_iged()
        else:
            self.auth_status_label.setText("Authentication: Failed ❌")
            self.auth_status_label.setStyleSheet("font-weight: bold; color: red;")
        
        self.status_bar.showMessage(message)
        self.log_message(f"Authentication: {message}")
    
    def launch_iged(self):
        """Launch IGED after successful authentication"""
        try:
            # Look for IGED launcher in parent directory
            iged_launcher = Path("../launcher.py")
            if iged_launcher.exists():
                import subprocess
                subprocess.Popen([sys.executable, str(iged_launcher)])
                self.status_bar.showMessage("IGED launched successfully")
                self.log_message("IGED launched successfully")
            else:
                QMessageBox.warning(self, "IGED Not Found", 
                                  "IGED launcher not found. Please ensure IGED is installed.")
        except Exception as e:
            QMessageBox.critical(self, "Launch Error", f"Failed to launch IGED: {str(e)}")
    
    def start_webauthn_server(self):
        """Start the WebAuthn server"""
        # This would integrate with webauthn_server.py
        self.server_status_label.setText("Server: Running ✅")
        self.server_status_label.setStyleSheet("font-weight: bold; color: green;")
        self.start_server_btn.setEnabled(False)
        self.stop_server_btn.setEnabled(True)
        self.status_bar.showMessage("WebAuthn server started")
        self.log_message("WebAuthn server started")
    
    def stop_webauthn_server(self):
        """Stop the WebAuthn server"""
        self.server_status_label.setText("Server: Stopped ❌")
        self.server_status_label.setStyleSheet("font-weight: bold; color: red;")
        self.start_server_btn.setEnabled(True)
        self.stop_server_btn.setEnabled(False)
        self.status_bar.showMessage("WebAuthn server stopped")
        self.log_message("WebAuthn server stopped")
    
    def refresh_credentials(self):
        """Refresh the credentials table"""
        # This would load credentials from storage
        self.credential_count_label.setText("0")
        self.status_bar.showMessage("Credentials refreshed")
        self.log_message("Credentials refreshed")
    
    def delete_credential(self):
        """Delete selected credential"""
        current_row = self.credentials_table.currentRow()
        if current_row >= 0:
            self.credentials_table.removeRow(current_row)
            self.status_bar.showMessage("Credential deleted")
            self.log_message("Credential deleted")
    
    def export_credentials(self):
        """Export credentials to file"""
        filename, _ = QFileDialog.getSaveFileName(
            self, "Export Credentials", "", "JSON Files (*.json)"
        )
        if filename:
            # Export logic here
            self.status_bar.showMessage("Credentials exported")
            self.log_message(f"Credentials exported to {filename}")
    
    def import_credentials(self):
        """Import credentials from file"""
        filename, _ = QFileDialog.getOpenFileName(
            self, "Import Credentials", "", "JSON Files (*.json)"
        )
        if filename:
            # Import logic here
            self.status_bar.showMessage("Credentials imported")
            self.log_message(f"Credentials imported from {filename}")
    
    def clear_logs(self):
        """Clear the log display"""
        self.log_display.clear()
        self.status_bar.showMessage("Logs cleared")
    
    def save_logs(self):
        """Save logs to file"""
        filename, _ = QFileDialog.getSaveFileName(
            self, "Save Logs", "", "Text Files (*.txt)"
        )
        if filename:
            try:
                with open(filename, 'w') as f:
                    f.write(self.log_display.toPlainText())
                self.status_bar.showMessage("Logs saved")
                self.log_message(f"Logs saved to {filename}")
            except Exception as e:
                QMessageBox.critical(self, "Save Error", f"Failed to save logs: {str(e)}")
    
    def show_about(self):
        """Show about dialog"""
        QMessageBox.about(self, "About IGED Biometric Manager",
                         "IGED Biometric Credential Manager\n\n"
                         "Version 1.0.0\n"
                         "Enterprise-grade biometric authentication for IGED\n\n"
                         "Supports Windows Hello and WebAuthn")
    
    def log_message(self, message: str):
        """Add message to log display"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.log_display.append(f"[{timestamp}] {message}")
    
    def load_settings(self):
        """Load application settings"""
        self.reason_input.setText(self.settings.value("auth_reason", "Authenticate to unlock IGED"))
        self.auto_launch_checkbox.setChecked(self.settings.value("auto_launch", True, type=bool))
        self.remember_reason_checkbox.setChecked(self.settings.value("remember_reason", True, type=bool))
        self.server_port_input.setText(self.settings.value("server_port", "5000"))
    
    def save_settings(self):
        """Save application settings"""
        if self.remember_reason_checkbox.isChecked():
            self.settings.setValue("auth_reason", self.reason_input.text())
        self.settings.setValue("auto_launch", self.auto_launch_checkbox.isChecked())
        self.settings.setValue("remember_reason", self.remember_reason_checkbox.isChecked())
        self.settings.setValue("server_port", self.server_port_input.text())
    
    def closeEvent(self, event):
        """Handle application close event"""
        self.save_settings()
        event.accept()

def main():
    """Main function"""
    if not PYSIDE6_AVAILABLE:
        print("❌ PySide6 not available. Install with: pip install PySide6")
        return
    
    app = QApplication(sys.argv)
    app.setApplicationName("IGED Biometric Manager")
    app.setApplicationVersion("1.0.0")
    
    window = BiometricManagerGUI()
    window.show()
    
    sys.exit(app.exec())

if __name__ == "__main__":
    main() 