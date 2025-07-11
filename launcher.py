#!/usr/bin/env python3
"""
IGED - Project Human Bot Launcher
Main entry point for the sovereign AI assistant
"""

import signal
import sys
import threading
from pathlib import Path

# Add project root to path
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))

import logging


# Try to import GUI components (optional)
try:
    from pathlib import Path
    from ui.win_gui.main_window import IGEDGUI
    from admin_panel.web_admin import WebAdminPanel
    from core.encryption import EncryptionManager
    from core.memory_engine import MemoryEngine
    from core.command_parser import CommandParser
    from core.voice_pipeline import VoicePipeline
    from agents.orchestrator import Orchestrator
    from build_release.watchdog import Watchdog
    from cryptography.fernet import Fernet
    GUI_AVAILABLE = True
except ImportError as e:
    GUI_AVAILABLE = False
    print(f"⚠️ GUI not available: {e}")
    print("🌐 Web interface will be available at http://localhost:8080")

# Configure logging
try:
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        handlers=[logging.FileHandler("logs/iged.log"), logging.StreamHandler()],
    )
except Exception as e:
    # Fallback to console-only logging if file creation fails
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
        handlers=[logging.StreamHandler()],
    )
    print(f"⚠️ Could not create log file: {e}")
    print("📝 Logging to console only")

logger = logging.getLogger(__name__)


class IGEDLauncher:
    """Main launcher class for IGED system.

    Manages initialization and orchestration of all IGED components including
    voice pipeline, memory engine, agents, web admin, and GUI interfaces.
    """

    def __init__(self) -> None:
        """Initialize the IGED launcher.

        Sets up the component dictionary and initializes all system components.
        """
        self.running = False
        self.components = {}
        self.initialize_system()

    def initialize_system(self) -> None:
        """Initialize all IGED components.

        Creates necessary directories and initializes:
        - Encryption manager for secure data storage
        - Memory engine for command/result persistence
        - Command parser for natural language processing
        - Orchestrator for agent management
        - Voice pipeline for speech recognition
        - Watchdog for system monitoring

        Raises:
            Exception: If any critical component fails to initialize
        """
        try:
            logger.info("🚀 Initializing IGED System...")

            # Create necessary directories
            self.create_directories()

            # Initialize encryption
            self.components["encryption"] = EncryptionManager()

            # Initialize memory engine
            self.components["memory"] = MemoryEngine(self.components["encryption"])

            # Initialize command parser
            self.components["parser"] = CommandParser()

            # Initialize orchestrator
            self.components["orchestrator"] = Orchestrator(self.components["memory"])

            # Initialize voice pipeline
            self.components["voice"] = VoicePipeline(
                self.components["parser"],
                self.components["orchestrator"],
                self.components["memory"],
            )

            # Initialize watchdog
            self.components["watchdog"] = Watchdog(self.components)

            logger.info("✅ System initialization complete")

        except Exception as e:
            logger.error(f"❌ Failed to initialize system: {e}")
            raise

    def create_directories(self) -> None:
        """Create necessary project directories.

        Creates the following directory structure:
        - config/ - Configuration files and encryption keys
        - memory/ - Persistent memory storage
        - plugins/ - Plugin modules
        - agents/ - AI agent modules
        - ui/win_gui/ - GUI interface components
        - admin_panel/ - Web administration interface
        - android-client/ - Android integration
        - logs/ - System log files
        """
        directories = [
            "config",
            "memory",
            "plugins",
            "agents",
            "ui/win_gui",
            "admin_panel",
            "android-client",
            "logs",
        ]

        for directory in directories:
            Path(directory).mkdir(parents=True, exist_ok=True)

    def start_gui(self) -> None:
        """Start the GUI interface"""
        if not GUI_AVAILABLE:
            logger.info("🖥️ GUI not available, skipping...")
            return False

        try:
            logger.info("🖥️ Starting GUI interface...")
            self.components["gui"] = IGEDGUI(self.components)
            self.components["gui"].run()
            return True
        except Exception as e:
            logger.error(f"❌ Failed to start GUI: {e}")
            return False

    def start_web_admin(self) -> None:
        """Start the web admin panel"""
        try:
            logger.info("🌐 Starting web admin panel...")
            self.components["web_admin"] = WebAdminPanel(self.components)
            self.components["web_admin"].start()
        except Exception as e:
            logger.error(f"❌ Failed to start web admin: {e}")

    def start_voice_listening(self) -> None:
        """Start voice listening in background"""
        try:
            logger.info("🎤 Starting voice listening...")
            self.components["voice"].start_listening()
        except Exception as e:
            logger.error(f"❌ Failed to start voice listening: {e}")

    def run(self) -> None:
        """Main run loop for IGED system.

        Starts all system components in separate threads:
        - Voice listening thread for speech recognition
        - Web admin thread for remote management
        - Watchdog thread for system monitoring

        Attempts to start GUI interface, falls back to headless mode
        with interactive command prompt if GUI is unavailable.

        Handles graceful shutdown on KeyboardInterrupt and other exceptions.
        """
        try:
            self.running = True
            logger.info("🎯 IGED is now running!")

            # Start components in separate threads
            threads = []

            # Voice listening thread
            voice_thread = threading.Thread(
                target=self.start_voice_listening, daemon=True
            )
            voice_thread.start()
            threads.append(voice_thread)

            # Web admin thread
            web_thread = threading.Thread(target=self.start_web_admin, daemon=True)
            web_thread.start()
            threads.append(web_thread)

            # Watchdog thread
            watchdog_thread = threading.Thread(
                target=self.components["watchdog"].run, daemon=True
            )
            watchdog_thread.start()
            threads.append(watchdog_thread)

            # Start GUI (main thread) or run in headless mode
            if not self.start_gui():
                logger.info(
                    "🌐 Running in headless mode - Web interface available at http://localhost:8080"
                )
                logger.info("🎤 Voice commands available")
                logger.info("📱 Android client can connect on port 9090")
                logger.info("⌨️ Type 'quit' to exit")

                # Simple command loop for headless mode
                try:
                    while self.running:
                        try:
                            user_input = input("IGED> ").strip()
                            if user_input.lower() in ["quit", "exit", "q"]:
                                break
                            elif user_input:
                                # Process command through parser
                                result = self.components["parser"].parse_command(
                                    user_input
                                )
                                if result:
                                    self.components["orchestrator"].execute_task(result)
                        except EOFError:
                            break
                        except KeyboardInterrupt:
                            break
                except Exception as e:
                    logger.error(f"❌ Command loop error: {e}")

        except KeyboardInterrupt:
            logger.info("🛑 Shutdown requested...")
        except Exception as e:
            logger.error(f"❌ Runtime error: {e}")
        finally:
            self.shutdown()

    def shutdown(self) -> None:
        """Clean shutdown of all components"""
        logger.info("🔄 Shutting down IGED...")
        self.running = False

        # Stop voice listening
        if "voice" in self.components:
            self.components["voice"].stop_listening()

        # Stop web admin
        if "web_admin" in self.components:
            self.components["web_admin"].stop()

        # Stop watchdog
        if "watchdog" in self.components:
            self.components["watchdog"].stop()

        logger.info("✅ IGED shutdown complete")


def signal_handler(signum, frame) -> None:
    """Handle shutdown signals gracefully.

    Args:
        signum: Signal number received (SIGINT, SIGTERM, etc.)
        frame: Current stack frame (unused)
    """
    logger.info(f"📡 Received signal {signum}, shutting down...")
    sys.exit(0)


def main() -> None:
    """Main entry point for IGED application.

    Performs system initialization:
    1. Registers signal handlers for graceful shutdown
    2. Validates Python version compatibility (3.8+ required)
    3. Checks for required dependencies
    4. Generates encryption key if missing
    5. Launches IGED system

    Dependencies checked:
    - cryptography: Required for encryption
    - pandas: Optional for data analysis
    - numpy: Optional for numerical computing
    - matplotlib: Optional for data visualization

    Exits with code 1 if critical requirements are not met.
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
    missing_deps = []

    try:
        import cryptography
    except ImportError:
        missing_deps.append("cryptography")

    try:
        import pandas
    except ImportError:
        missing_deps.append("pandas")

    try:
        import numpy
    except ImportError:
        missing_deps.append("numpy")

    try:
        import matplotlib
    except ImportError:
        missing_deps.append("matplotlib")

    if missing_deps:
        print(f"⚠️ Missing dependencies: {', '.join(missing_deps)}")
        print("💡 Run: python install_dependencies.py")
        print("💡 Or: install_deps.bat (Windows)")
        print("\n🚀 Starting IGED anyway... (some features may not work)")

    # Check for required files
    if not Path("config/secret.key").exists():
        print("🔑 Generating encryption key...")
        try:
            from cryptography.fernet import Fernet
            key = Fernet.generate_key()
            Path("config").mkdir(exist_ok=True)
            with open("config/secret.key", "wb") as f:
                f.write(key)
        except ImportError:
            print("❌ cryptography not available, cannot generate key")
            print("Please install: pip install cryptography")
            sys.exit(1)

    # Launch IGED
    try:
        launcher = IGEDLauncher()
        launcher.run()
    except Exception as e:
        print(f"❌ IGED failed to start: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
