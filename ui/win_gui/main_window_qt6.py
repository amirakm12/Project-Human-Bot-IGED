"""
IGED GUI - Qt6 Interface
Main window for the IGED assistant using PySide6
"""

import sys
import threading
import queue
import time
from datetime import datetime
import logging
import json
from pathlib import Path

from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QLabel, QLineEdit, QTextEdit, QTabWidget,
    QTreeWidget, QTreeWidgetItem, QSplitter, QGroupBox,
    QSlider, QCheckBox, QMessageBox, QHeaderView,
    QScrollArea, QFrame, QGridLayout, QFileDialog
)
from PySide6.QtCore import Qt, QTimer, QThread, Signal, QSettings
from PySide6.QtGui import QFont, QPalette, QColor, QTextCursor

logger = logging.getLogger(__name__)

class CommandWorker(QThread):
    """Worker thread for command execution"""
    output_signal = Signal(str)
    status_signal = Signal(str)
    
    def __init__(self, command, components):
        super().__init__()
        self.command = command
        self.components = components
    
    def run(self):
        """Execute command in background thread"""
        try:
            # Process command through voice pipeline
            if 'voice' in self.components:
                self.components['voice'].process_text_command(self.command)
                self.output_signal.emit(f"✅ Command executed: {self.command}")
            else:
                self.output_signal.emit("❌ Voice pipeline not available")
        except Exception as e:
            self.output_signal.emit(f"❌ Command execution error: {e}")

class IGEDGUI(QMainWindow):
    """Qt6-based GUI for IGED assistant"""
    
    def __init__(self, components):
        super().__init__()
        self.components = components
        self.settings = QSettings("IGED", "MainWindow")
        self.message_queue = queue.Queue()
        self.running = True
        self.voice_active = False
        
        self.init_ui()
        self.load_settings()
        
        # Setup timer for message processing
        self.message_timer = QTimer()
        self.message_timer.timeout.connect(self._process_messages)
        self.message_timer.start(100)  # Check every 100ms
    
    def init_ui(self):
        """Initialize the user interface"""
        self.setWindowTitle("IGED - Project Human Bot")
        self.setGeometry(100, 100, 1000, 700)
        
        # Apply dark theme
        self.set_dark_theme()
        
        # Create central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Title
        title_label = QLabel("🤖 IGED - Sovereign AI Assistant")
        title_label.setAlignment(Qt.AlignCenter)
        title_font = QFont("Arial", 16, QFont.Bold)
        title_label.setFont(title_font)
        title_label.setStyleSheet("color: #00ff00; padding: 10px;")
        main_layout.addWidget(title_label)
        
        # Create tab widget
        self.tab_widget = QTabWidget()
        main_layout.addWidget(self.tab_widget)
        
        # Create tabs
        self.create_main_tab()
        self.create_memory_tab()
        self.create_status_tab()
        self.create_settings_tab()
        
        # Status bar
        self.status_label = QLabel("Ready")
        self.statusBar().addWidget(self.status_label)
    
    def create_main_tab(self):
        """Create the main interface tab"""
        main_tab = QWidget()
        layout = QVBoxLayout(main_tab)
        
        # Command input section
        input_group = QGroupBox("Command Input")
        input_layout = QVBoxLayout(input_group)
        
        # Command entry
        command_layout = QHBoxLayout()
        command_layout.addWidget(QLabel("Enter Command:"))
        self.command_entry = QLineEdit()
        self.command_entry.setPlaceholderText("Type your command here...")
        self.command_entry.returnPressed.connect(self.execute_command)
        command_layout.addWidget(self.command_entry)
        input_layout.addLayout(command_layout)
        
        # Buttons
        button_layout = QHBoxLayout()
        
        self.execute_btn = QPushButton("🚀 Execute")
        self.execute_btn.clicked.connect(self.execute_command)
        self.execute_btn.setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;")
        button_layout.addWidget(self.execute_btn)
        
        self.voice_btn = QPushButton("🎤 Start Voice")
        self.voice_btn.clicked.connect(self.toggle_voice)
        self.voice_btn.setStyleSheet("background-color: #2196F3; color: white; font-weight: bold;")
        button_layout.addWidget(self.voice_btn)
        
        self.clear_btn = QPushButton("🗑️ Clear")
        self.clear_btn.clicked.connect(self.clear_output)
        self.clear_btn.setStyleSheet("background-color: #f44336; color: white; font-weight: bold;")
        button_layout.addWidget(self.clear_btn)
        
        input_layout.addLayout(button_layout)
        layout.addWidget(input_group)
        
        # Output section
        output_group = QGroupBox("Output")
        output_layout = QVBoxLayout(output_group)
        
        self.output_text = QTextEdit()
        self.output_text.setReadOnly(True)
        self.output_text.setFont(QFont("Consolas", 10))
        self.output_text.setStyleSheet("background-color: #1e1e1e; color: #00ff00;")
        output_layout.addWidget(self.output_text)
        
        layout.addWidget(output_group)
        
        self.tab_widget.addTab(main_tab, "🎯 Main Interface")
    
    def create_memory_tab(self):
        """Create the memory tab"""
        memory_tab = QWidget()
        layout = QVBoxLayout(memory_tab)
        
        # Search section
        search_layout = QHBoxLayout()
        search_layout.addWidget(QLabel("Search Memory:"))
        self.search_entry = QLineEdit()
        self.search_entry.setPlaceholderText("Enter search query...")
        search_layout.addWidget(self.search_entry)
        
        self.search_btn = QPushButton("🔍 Search")
        self.search_btn.clicked.connect(self.search_memory)
        search_layout.addWidget(self.search_btn)
        layout.addLayout(search_layout)
        
        # Memory list
        self.memory_tree = QTreeWidget()
        self.memory_tree.setHeaderLabels(['Time', 'Command', 'Agent', 'Status'])
        self.memory_tree.itemSelectionChanged.connect(self.on_memory_select)
        layout.addWidget(self.memory_tree)
        
        # Memory details
        details_group = QGroupBox("Memory Details")
        details_layout = QVBoxLayout(details_group)
        
        self.memory_details = QTextEdit()
        self.memory_details.setReadOnly(True)
        self.memory_details.setMaximumHeight(150)
        self.memory_details.setFont(QFont("Consolas", 9))
        details_layout.addWidget(self.memory_details)
        
        layout.addWidget(details_group)
        
        self.tab_widget.addTab(memory_tab, "🧠 Memory")
        
        # Load initial memory
        self.load_memory()
    
    def create_status_tab(self):
        """Create the system status tab"""
        status_tab = QWidget()
        layout = QVBoxLayout(status_tab)
        
        # System info
        system_group = QGroupBox("System Information")
        system_layout = QVBoxLayout(system_group)
        
        self.system_info = QTextEdit()
        self.system_info.setReadOnly(True)
        self.system_info.setMaximumHeight(200)
        self.system_info.setFont(QFont("Consolas", 9))
        system_layout.addWidget(self.system_info)
        layout.addWidget(system_group)
        
        # Agents status
        agents_group = QGroupBox("Agents Status")
        agents_layout = QVBoxLayout(agents_group)
        
        self.agents_info = QTextEdit()
        self.agents_info.setReadOnly(True)
        self.agents_info.setMaximumHeight(150)
        self.agents_info.setFont(QFont("Consolas", 9))
        agents_layout.addWidget(self.agents_info)
        layout.addWidget(agents_group)
        
        # Refresh button
        self.refresh_btn = QPushButton("🔄 Refresh Status")
        self.refresh_btn.clicked.connect(self.refresh_status)
        self.refresh_btn.setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;")
        layout.addWidget(self.refresh_btn)
        
        layout.addStretch()
        
        self.tab_widget.addTab(status_tab, "📊 System Status")
        
        # Load initial status
        self.refresh_status()
    
    def create_settings_tab(self):
        """Create the settings tab"""
        settings_tab = QWidget()
        layout = QVBoxLayout(settings_tab)
        
        # Voice settings
        voice_group = QGroupBox("Voice Settings")
        voice_layout = QVBoxLayout(voice_group)
        
        sensitivity_layout = QHBoxLayout()
        sensitivity_layout.addWidget(QLabel("Voice Sensitivity:"))
        self.voice_sensitivity = QSlider(Qt.Horizontal)
        self.voice_sensitivity.setRange(1, 10)
        self.voice_sensitivity.setValue(5)
        sensitivity_layout.addWidget(self.voice_sensitivity)
        self.sensitivity_label = QLabel("0.5")
        sensitivity_layout.addWidget(self.sensitivity_label)
        voice_layout.addLayout(sensitivity_layout)
        
        self.voice_sensitivity.valueChanged.connect(
            lambda v: self.sensitivity_label.setText(str(v/10))
        )
        
        layout.addWidget(voice_group)
        
        # Memory settings
        memory_group = QGroupBox("Memory Settings")
        memory_layout = QVBoxLayout(memory_group)
        
        self.clear_memory_btn = QPushButton("🗑️ Clear All Memory")
        self.clear_memory_btn.clicked.connect(self.clear_all_memory)
        self.clear_memory_btn.setStyleSheet("background-color: #f44336; color: white;")
        memory_layout.addWidget(self.clear_memory_btn)
        
        self.export_memory_btn = QPushButton("📤 Export Memory")
        self.export_memory_btn.clicked.connect(self.export_memory)
        self.export_memory_btn.setStyleSheet("background-color: #2196F3; color: white;")
        memory_layout.addWidget(self.export_memory_btn)
        
        layout.addWidget(memory_group)
        
        # System settings
        system_group = QGroupBox("System Settings")
        system_layout = QVBoxLayout(system_group)
        
        self.offline_mode_check = QCheckBox("Offline Mode")
        system_layout.addWidget(self.offline_mode_check)
        
        layout.addWidget(system_group)
        
        layout.addStretch()
        
        self.tab_widget.addTab(settings_tab, "⚙️ Settings")
    
    def set_dark_theme(self):
        """Apply dark theme to the application"""
        palette = QPalette()
        palette.setColor(QPalette.Window, QColor(43, 43, 43))
        palette.setColor(QPalette.WindowText, QColor(255, 255, 255))
        palette.setColor(QPalette.Base, QColor(30, 30, 30))
        palette.setColor(QPalette.AlternateBase, QColor(43, 43, 43))
        palette.setColor(QPalette.ToolTipBase, QColor(255, 255, 255))
        palette.setColor(QPalette.ToolTipText, QColor(255, 255, 255))
        palette.setColor(QPalette.Text, QColor(255, 255, 255))
        palette.setColor(QPalette.Button, QColor(74, 74, 74))
        palette.setColor(QPalette.ButtonText, QColor(255, 255, 255))
        palette.setColor(QPalette.BrightText, QColor(255, 0, 0))
        palette.setColor(QPalette.Link, QColor(42, 130, 218))
        palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
        palette.setColor(QPalette.HighlightedText, QColor(255, 255, 255))
        
        self.setPalette(palette)
    
    def execute_command(self):
        """Execute a command"""
        command = self.command_entry.text().strip()
        if not command:
            return
        
        self.log_output(f"🎯 Executing: {command}")
        self.command_entry.clear()
        
        # Execute in worker thread
        self.worker = CommandWorker(command, self.components)
        self.worker.output_signal.connect(self.log_output)
        self.worker.start()
    
    def toggle_voice(self):
        """Toggle voice listening"""
        try:
            if 'voice' in self.components:
                voice = self.components['voice']
                if self.voice_active:
                    voice.stop_listening()
                    self.voice_btn.setText("🎤 Start Voice")
                    self.voice_btn.setStyleSheet("background-color: #2196F3; color: white; font-weight: bold;")
                    self.log_output("🛑 Voice listening stopped")
                    self.voice_active = False
                else:
                    voice.start_listening()
                    self.voice_btn.setText("🛑 Stop Voice")
                    self.voice_btn.setStyleSheet("background-color: #f44336; color: white; font-weight: bold;")
                    self.log_output("🎤 Voice listening started")
                    self.voice_active = True
            else:
                self.log_output("❌ Voice pipeline not available")
        except Exception as e:
            self.log_output(f"❌ Voice toggle error: {e}")
    
    def clear_output(self):
        """Clear the output text area"""
        self.output_text.clear()
    
    def log_output(self, message):
        """Add message to output"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted_message = f"[{timestamp}] {message}"
        
        # Add to queue for thread-safe update
        self.message_queue.put(formatted_message)
    
    def _process_messages(self):
        """Process messages from queue"""
        try:
            while not self.message_queue.empty():
                message = self.message_queue.get_nowait()
                self.output_text.append(message)
                # Auto-scroll to bottom
                cursor = self.output_text.textCursor()
                cursor.movePosition(QTextCursor.End)
                self.output_text.setTextCursor(cursor)
        except queue.Empty:
            pass
        except Exception as e:
            logger.error(f"Message processing error: {e}")
    
    def load_memory(self):
        """Load memory entries into tree view"""
        try:
            self.memory_tree.clear()
            
            if 'memory' in self.components:
                memory = self.components['memory']
                entries = memory.get_recent_entries(50)
                
                for entry in entries:
                    timestamp = entry.get('timestamp', '')[:19]
                    command = entry.get('command', '')[:50]
                    agent = entry.get('agent', 'unknown')
                    success = "✅" if entry.get('success', False) else "❌"
                    
                    item = QTreeWidgetItem([timestamp, command, agent, success])
                    self.memory_tree.addTopLevelItem(item)
        except Exception as e:
            logger.error(f"Failed to load memory: {e}")
    
    def search_memory(self):
        """Search memory entries"""
        query = self.search_entry.text().strip()
        if not query:
            self.load_memory()
            return
        
        try:
            self.memory_tree.clear()
            
            if 'memory' in self.components:
                memory = self.components['memory']
                entries = memory.search_entries(query, 20)
                
                for entry in entries:
                    timestamp = entry.get('timestamp', '')[:19]
                    command = entry.get('command', '')[:50]
                    agent = entry.get('agent', 'unknown')
                    success = "✅" if entry.get('success', False) else "❌"
                    
                    item = QTreeWidgetItem([timestamp, command, agent, success])
                    self.memory_tree.addTopLevelItem(item)
        except Exception as e:
            logger.error(f"Failed to search memory: {e}")
    
    def on_memory_select(self):
        """Handle memory item selection"""
        selected_items = self.memory_tree.selectedItems()
        if not selected_items:
            return
        
        try:
            item = selected_items[0]
            timestamp = item.text(0)
            
            if 'memory' in self.components:
                memory = self.components['memory']
                entries = memory.get_recent_entries(100)
                
                for entry in entries:
                    if entry.get('timestamp', '')[:19] == timestamp:
                        details = f"Time: {entry.get('timestamp', '')}\n"
                        details += f"Command: {entry.get('command', '')}\n"
                        details += f"Agent: {entry.get('agent', '')}\n"
                        details += f"Success: {entry.get('success', '')}\n"
                        details += f"Result: {entry.get('result', '')}\n"
                        
                        self.memory_details.setPlainText(details)
                        break
        except Exception as e:
            logger.error(f"Failed to load memory details: {e}")
    
    def refresh_status(self):
        """Refresh system status"""
        try:
            # System info
            system_info = "IGED System Status\n"
            system_info += "=" * 50 + "\n"
            
            if 'voice' in self.components:
                voice_status = self.components['voice'].get_status()
                system_info += f"Voice Pipeline: {'Active' if voice_status.get('is_listening', False) else 'Inactive'}\n"
                system_info += f"Whisper Model: {'Loaded' if voice_status.get('whisper_loaded', False) else 'Not Loaded'}\n"
            
            if 'orchestrator' in self.components:
                orch_status = self.components['orchestrator'].get_system_status()
                system_info += f"Active Agents: {orch_status.get('total_agents', 0)}\n"
                system_info += f"Active Plugins: {orch_status.get('total_plugins', 0)}\n"
            
            self.system_info.setPlainText(system_info)
            
            # Agents info
            agents_info = "Agent Status\n"
            agents_info += "=" * 30 + "\n"
            
            if 'orchestrator' in self.components:
                orch_status = self.components['orchestrator'].get_system_status()
                agents = orch_status.get('agents', {})
                for agent_name, status in agents.items():
                    agents_info += f"{agent_name}: {status.get('status', 'unknown')}\n"
            
            self.agents_info.setPlainText(agents_info)
        except Exception as e:
            logger.error(f"Failed to refresh status: {e}")
    
    def clear_all_memory(self):
        """Clear all memory entries"""
        reply = QMessageBox.question(self, "Confirm", 
                                    "Are you sure you want to clear all memory?",
                                    QMessageBox.Yes | QMessageBox.No)
        
        if reply == QMessageBox.Yes:
            try:
                if 'memory' in self.components:
                    self.components['memory'].clear_memory()
                    self.load_memory()
                    self.log_output("🧹 All memory cleared")
            except Exception as e:
                self.log_output(f"❌ Failed to clear memory: {e}")
    
    def export_memory(self):
        """Export memory to file"""
        try:
            filename, _ = QFileDialog.getSaveFileName(
                self, "Export Memory", 
                f"memory_export_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json",
                "JSON Files (*.json)"
            )
            
            if filename and 'memory' in self.components:
                memory = self.components['memory']
                if memory.export_memory(filename):
                    self.log_output(f"📤 Memory exported to {filename}")
                else:
                    self.log_output("❌ Failed to export memory")
        except Exception as e:
            self.log_output(f"❌ Export error: {e}")
    
    def load_settings(self):
        """Load application settings"""
        # Load window geometry
        geometry = self.settings.value("geometry")
        if geometry:
            self.restoreGeometry(geometry)
        
        # Load other settings
        self.voice_sensitivity.setValue(self.settings.value("voice_sensitivity", 5, type=int))
        self.offline_mode_check.setChecked(self.settings.value("offline_mode", False, type=bool))
    
    def save_settings(self):
        """Save application settings"""
        self.settings.setValue("geometry", self.saveGeometry())
        self.settings.setValue("voice_sensitivity", self.voice_sensitivity.value())
        self.settings.setValue("offline_mode", self.offline_mode_check.isChecked())
    
    def closeEvent(self, event):
        """Handle window close event"""
        self.running = False
        self.save_settings()
        
        # Stop message timer
        if hasattr(self, 'message_timer'):
            self.message_timer.stop()
        
        event.accept()
    
    def run(self):
        """Start the GUI (compatibility method for existing code)"""
        try:
            self.log_output("🚀 IGED GUI started")
            self.show()
        except Exception as e:
            logger.error(f"GUI error: {e}")

def create_qt6_app(components):
    """Create and return Qt6 application and window"""
    app = QApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
    
    app.setApplicationName("IGED")
    app.setApplicationDisplayName("IGED - Project Human Bot")
    
    window = IGEDGUI(components)
    return app, window

if __name__ == "__main__":
    # Test run
    app = QApplication(sys.argv)
    window = IGEDGUI({})
    window.show()
    sys.exit(app.exec())