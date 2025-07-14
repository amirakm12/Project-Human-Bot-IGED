#!/usr/bin/env python3
"""
IGED - Project Human Bot Launcher
Main entry point for the sovereign AI assistant

This module serves as the primary launcher for the IGED system, initializing
all core components and managing the application lifecycle.
"""

import os
import sys
import threading
import signal
from pathlib import Path
from typing import Dict, Any, Optional

# Add project root to path
PROJECT_ROOT = Path(__file__).parent
sys.path.insert(0, str(PROJECT_ROOT))

# Core imports
from core.voice_pipeline import VoicePipeline
from core.command_parser import CommandParser
from core.memory_engine import MemoryEngine
from core.encryption import EncryptionManager
from agents.orchestrator import AgentOrchestrator
from admin_panel.web_admin import WebAdminPanel
from watchdog import Watchdog
import logging

# GUI imports (optional)
try:
    from ui.win_gui.main_window import IGEDGUI
    GUI_AVAILABLE = True
except ImportError as e:
    GUI_AVAILABLE = False
    print(f"⚠️ GUI not available: {e}")
    print("🌐 Web interface will be available at http://localhost:8080")

# Configure logging
try:
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.FileHandler('logs/iged.log'),
            logging.StreamHandler()
        ]
    )
except Exception as e:
    # Fallback to console-only logging if file creation fails
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler()
        ]
    )
    print(f"⚠️ Could not create log file: {e}")
    print("📝 Logging to console only")

logger = logging.getLogger(__name__)


class IGEDLauncher:
    """
    Main launcher class for the IGED system.
    
    This class manages the initialization and lifecycle of all IGED components
    including voice processing, command parsing, memory management, and GUI.
    """
    
    def __init__(self):
        """Initialize the IGED launcher with default settings."""
        self.running = False
        self.components: Dict[str, Any] = {}
        self.initialize_system()
    
    def initialize_system(self) -> None:
        """
        Initialize all IGED components in the correct order.
        
        Raises:
            Exception: If any critical component fails to initialize
        """
        try:
            logger.info("🚀 Initializing IGED System...")
            
            # Create necessary directories
            self._create_directories()
            
            # Initialize encryption manager
            self.components['encryption'] = EncryptionManager()
            
            # Initialize memory engine with encryption
            self.components['memory'] = MemoryEngine(self.components['encryption'])
            
            # Initialize command parser
            self.components['parser'] = CommandParser()
            
            # Initialize orchestrator with memory
            self.components['orchestrator'] = AgentOrchestrator(self.components['memory'])
            
            # Initialize voice pipeline with all dependencies
            self.components['voice'] = VoicePipeline(
                self.components['parser'],
                self.components['orchestrator'],
                self.components['memory']
            )
            
            # Initialize system watchdog
            self.components['watchdog'] = Watchdog(self.components)
            
            logger.info("✅ System initialization complete")
            
        except Exception as e:
            logger.error(f"❌ Failed to initialize system: {e}")
            raise
    
    def _create_directories(self) -> None:
        """Create necessary project directories if they don't exist."""
        required_directories = [
            'config',
            'memory',
            'plugins',
            'agents',
            'ui/win_gui',
            'admin_panel',
            'android-client',
            'logs'
        ]
        
        for directory in required_directories:
            Path(directory).mkdir(parents=True, exist_ok=True)
    
    def start_gui(self) -> bool:
        """
        Start the GUI interface if available.
        
        Returns:
            bool: True if GUI started successfully, False otherwise
        """
        if not GUI_AVAILABLE:
            logger.info("🖥️ GUI not available, skipping...")
            return False
            
        try:
            logger.info("🖥️ Starting GUI interface...")
            self.components['gui'] = IGEDGUI(self.components)
            self.components['gui'].run()
            return True
        except Exception as e:
            logger.error(f"❌ Failed to start GUI: {e}")
            return False
    
    def start_web_admin(self) -> None:
        """
        Start the web admin panel.
        
        Raises:
            Exception: If web admin fails to start
        """
        try:
            logger.info("🌐 Starting web admin panel...")
            self.components['web_admin'] = WebAdminPanel(self.components)
            self.components['web_admin'].start()
        except Exception as e:
            logger.error(f"❌ Failed to start web admin: {e}")
    
    def start_voice_listening(self) -> None:
        """
        Start voice listening in background thread.
        
        Raises:
            Exception: If voice pipeline fails to start
        """
        try:
            logger.info("🎤 Starting voice listening...")
            self.components['voice'].start_listening()
        except Exception as e:
            logger.error(f"❌ Failed to start voice listening: {e}")
    
    def run(self) -> None:
        """
        Main application run loop.
        
        Starts all components and manages the application lifecycle.
        Handles both GUI and headless modes.
        """
        try:
            self.running = True
            logger.info("🎯 IGED is now running!")
            
            # Start components in separate threads
            component_threads = []
            
            # Voice listening thread
            voice_thread = threading.Thread(target=self.start_voice_listening, daemon=True)
            voice_thread.start()
            component_threads.append(voice_thread)
            
            # Web admin thread
            web_thread = threading.Thread(target=self.start_web_admin, daemon=True)
            web_thread.start()
            component_threads.append(web_thread)
            
            # Watchdog thread
            watchdog_thread = threading.Thread(target=self.components['watchdog'].run, daemon=True)
            watchdog_thread.start()
            component_threads.append(watchdog_thread)
            
            # Start GUI (main thread) or run in headless mode
            if not self.start_gui():
                logger.info("🌐 Running in headless mode - Web interface available at http://localhost:8080")
                logger.info("🎤 Voice commands available")
                logger.info("📱 Android client can connect on port 9090")
                logger.info("⌨️ Type 'quit' to exit")
                
                # Simple command loop for headless mode
                self._run_headless_mode()
            
        except KeyboardInterrupt:
            logger.info("🛑 Shutdown requested...")
        except Exception as e:
            logger.error(f"❌ Runtime error: {e}")
        finally:
            self.shutdown()
    
    def _run_headless_mode(self) -> None:
        """
        Run the application in headless mode with command line interface.
        
        Provides a simple command loop for text-based interaction.
        """
        try:
            while self.running:
                try:
                    user_input = input("IGED> ").strip()
                    if user_input.lower() in ['quit', 'exit', 'q']:
                        break
                    elif user_input:
                        # Process command through parser
                        command_result = self.components['parser'].parse_command(user_input)
                        if command_result:
                            self.components['orchestrator'].execute_command(command_result)
                except EOFError:
                    break
                except KeyboardInterrupt:
                    break
        except Exception as e:
            logger.error(f"❌ Command loop error: {e}")
    
    def shutdown(self) -> None:
        """
        Clean shutdown of all components.
        
        Stops all running threads and cleans up resources.
        """
        logger.info("🔄 Shutting down IGED...")
        self.running = False
        
        # Stop voice listening
        if 'voice' in self.components:
            self.components['voice'].stop_listening()
        
        # Stop web admin
        if 'web_admin' in self.components:
            self.components['web_admin'].stop()
        
        # Stop watchdog
        if 'watchdog' in self.components:
            self.components['watchdog'].stop()
        
        logger.info("✅ IGED shutdown complete")


def signal_handler(signum: int, frame) -> None:
    """
    Handle shutdown signals gracefully.
    
    Args:
        signum: Signal number received
        frame: Current stack frame
    """
    logger.info(f"📡 Received signal {signum}, shutting down...")
    sys.exit(0)


def main() -> None:
    """
    Main entry point for the IGED application.
    
    Performs initial setup, dependency checking, and launches the application.
    """
    # Register signal handlers
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # Check Python version
    if sys.version_info < (3, 8):
        print("❌ Python 3.8+ required")
        sys.exit(1)
    
    # Check dependencies
    print("🔧 Checking dependencies...")
    missing_dependencies = _check_dependencies()
    
    if missing_dependencies:
        print(f"⚠️ Missing dependencies: {', '.join(missing_dependencies)}")
        print("💡 Run: python install_dependencies.py")
        print("💡 Or: install_deps.bat (Windows)")
        print("\n🚀 Starting IGED anyway... (some features may not work)")
    
    # Ensure encryption key exists
    _ensure_encryption_key()
    
    # Launch IGED
    try:
        launcher = IGEDLauncher()
        launcher.run()
    except Exception as e:
        print(f"❌ IGED failed to start: {e}")
        sys.exit(1)


def _check_dependencies() -> list:
    """
    Check for required dependencies.
    
    Returns:
        list: List of missing dependency names
    """
    missing_deps = []
    
    dependencies_to_check = [
        'cryptography',
        'pandas',
        'numpy',
        'matplotlib'
    ]
    
    for dep in dependencies_to_check:
        try:
            __import__(dep)
        except ImportError:
            missing_deps.append(dep)
    
    return missing_deps


def _ensure_encryption_key() -> None:
    """
    Ensure that encryption key exists, create if missing.
    
    Raises:
        SystemExit: If encryption key cannot be created
    """
    key_path = Path("config/secret.key")
    
    if not key_path.exists():
        print("🔑 Generating encryption key...")
        try:
            from cryptography.fernet import Fernet
            key = Fernet.generate_key()
            key_path.parent.mkdir(exist_ok=True)
            with open(key_path, "wb") as f:
                f.write(key)
        except ImportError:
            print("❌ cryptography not available, cannot generate key")
            print("Please install: pip install cryptography")
            sys.exit(1)


if __name__ == "__main__":
    main() 