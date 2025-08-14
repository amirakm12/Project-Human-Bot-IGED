"""
IGED GUI Manager - Intelligent Framework Selection
Automatically selects the best GUI framework (Qt6 or tkinter) based on availability
Ensures seamless operation with optimal performance
"""

import sys
import os
import logging
import importlib.util
from typing import Optional, Dict, Any, Tuple
from pathlib import Path
import json
import time

logger = logging.getLogger(__name__)

class GUIFrameworkDetector:
    """Detects and validates available GUI frameworks"""
    
    @staticmethod
    def check_qt6() -> Tuple[bool, str]:
        """Check if Qt6 (PySide6) is available and functional"""
        try:
            import PySide6
            from PySide6.QtWidgets import QApplication
            from PySide6.QtCore import QT_VERSION_STR
            
            # Test creating an app instance
            app = QApplication.instance()
            if app is None:
                test_app = QApplication([])
                test_app.quit()
            
            return True, f"PySide6 v{PySide6.__version__} (Qt {QT_VERSION_STR})"
        except ImportError:
            return False, "PySide6 not installed"
        except Exception as e:
            return False, f"PySide6 error: {str(e)}"
    
    @staticmethod
    def check_tkinter() -> Tuple[bool, str]:
        """Check if tkinter is available and functional"""
        try:
            import tkinter as tk
            # Test creating a root window
            root = tk.Tk()
            root.withdraw()
            root.destroy()
            return True, f"tkinter (Tcl/Tk {tk.TclVersion if hasattr(tk, 'TclVersion') else 'unknown'})"
        except ImportError:
            return False, "tkinter not installed"
        except Exception as e:
            return False, f"tkinter error: {str(e)}"
    
    @staticmethod
    def get_display_availability() -> bool:
        """Check if a display is available (for GUI apps)"""
        if sys.platform == "win32":
            return True  # Windows always has display
        
        # Check for X11 or Wayland on Linux
        display = os.environ.get('DISPLAY') or os.environ.get('WAYLAND_DISPLAY')
        return display is not None

class PerformanceOptimizer:
    """Optimizes GUI performance based on system capabilities"""
    
    def __init__(self):
        self.cache = {}
        self.performance_metrics = {}
        
    def optimize_for_system(self) -> Dict[str, Any]:
        """Determine optimal settings based on system capabilities"""
        settings = {
            'use_hardware_acceleration': True,
            'enable_animations': True,
            'cache_size': 100,
            'update_frequency': 100,  # ms
            'thread_pool_size': 4,
        }
        
        try:
            import psutil
            
            # Check available memory
            memory = psutil.virtual_memory()
            if memory.available < 2 * 1024 * 1024 * 1024:  # Less than 2GB
                settings['cache_size'] = 50
                settings['enable_animations'] = False
            
            # Check CPU count
            cpu_count = psutil.cpu_count()
            if cpu_count:
                settings['thread_pool_size'] = min(cpu_count, 8)
            
            # Check if running in VM or container
            if self._is_virtualized():
                settings['use_hardware_acceleration'] = False
                settings['update_frequency'] = 200
            
        except ImportError:
            # psutil not available, use default settings
            logger.info("psutil not available, using default performance settings")
        except Exception as e:
            logger.warning(f"Could not optimize performance settings: {e}")
        
        return settings
    
    def _is_virtualized(self) -> bool:
        """Detect if running in a virtualized environment"""
        try:
            # Check for common virtualization indicators
            if os.path.exists('/.dockerenv'):
                return True
            
            if sys.platform == "linux":
                with open('/proc/1/cgroup', 'r') as f:
                    if 'docker' in f.read() or 'lxc' in f.read():
                        return True
            
            import platform
            if 'microsoft' in platform.uname().release.lower():
                return True  # WSL
                
        except Exception:
            pass
        
        return False

class GUIManager:
    """
    Unified GUI Manager for IGED
    Provides seamless integration with both Qt6 and tkinter
    """
    
    def __init__(self, components: Dict[str, Any], prefer_qt6: bool = True):
        """
        Initialize GUI Manager
        
        Args:
            components: Dictionary of application components
            prefer_qt6: Whether to prefer Qt6 over tkinter when both available
        """
        self.components = components
        self.prefer_qt6 = prefer_qt6
        self.gui_instance = None
        self.framework = None
        self.app = None
        
        # Initialize subsystems
        self.detector = GUIFrameworkDetector()
        self.optimizer = PerformanceOptimizer()
        
        # Performance settings
        self.settings = self.optimizer.optimize_for_system()
        
        # Detect best framework
        self._detect_framework()
    
    def _detect_framework(self):
        """Detect and select the best GUI framework"""
        qt6_available, qt6_info = self.detector.check_qt6()
        tkinter_available, tkinter_info = self.detector.check_tkinter()
        has_display = self.detector.get_display_availability()
        
        logger.info("GUI Framework Detection:")
        logger.info(f"  Qt6/PySide6: {qt6_info}")
        logger.info(f"  Tkinter: {tkinter_info}")
        logger.info(f"  Display available: {has_display}")
        
        if not has_display:
            logger.warning("No display detected. GUI will not be available.")
            self.framework = None
            return
        
        # Select framework based on availability and preference
        if self.prefer_qt6 and qt6_available:
            self.framework = 'qt6'
            logger.info("Selected framework: Qt6 (PySide6)")
        elif tkinter_available:
            self.framework = 'tkinter'
            logger.info("Selected framework: tkinter")
        elif qt6_available:
            self.framework = 'qt6'
            logger.info("Selected framework: Qt6 (PySide6) - fallback")
        else:
            self.framework = None
            logger.error("No GUI framework available!")
    
    def create_gui(self) -> Optional[Any]:
        """
        Create GUI instance using the detected framework
        
        Returns:
            GUI instance or None if no framework available
        """
        if self.framework == 'qt6':
            return self._create_qt6_gui()
        elif self.framework == 'tkinter':
            return self._create_tkinter_gui()
        else:
            logger.error("No GUI framework available to create interface")
            return None
    
    def _create_qt6_gui(self):
        """Create Qt6-based GUI"""
        try:
            # Import Qt6 GUI module
            from .main_window_qt6 import IGEDGUI, QApplication
            from PySide6.QtCore import Qt
            
            # Create or get QApplication
            self.app = QApplication.instance()
            if self.app is None:
                self.app = QApplication(sys.argv)
            
            # Configure application
            self.app.setApplicationName("IGED")
            self.app.setApplicationDisplayName("IGED - Project Human Bot")
            self.app.setOrganizationName("IGED")
            
            # Apply performance optimizations
            if self.settings['use_hardware_acceleration']:
                # Qt6 uses different attribute system
                try:
                    self.app.setAttribute(Qt.AA_UseDesktopOpenGL, True)
                except AttributeError:
                    # Fallback for different Qt versions
                    pass
            
            # Create main window
            self.gui_instance = IGEDGUI(self.components)
            
            # Apply performance settings
            self._apply_qt6_optimizations()
            
            logger.info("Qt6 GUI created successfully")
            return self.gui_instance
            
        except Exception as e:
            logger.error(f"Failed to create Qt6 GUI: {e}")
            # Fallback to tkinter
            if self.detector.check_tkinter()[0]:
                logger.info("Falling back to tkinter...")
                self.framework = 'tkinter'
                return self._create_tkinter_gui()
            return None
    
    def _create_tkinter_gui(self):
        """Create tkinter-based GUI"""
        try:
            # Import tkinter GUI module
            from .main_window import IGEDGUI
            
            # Create GUI instance
            self.gui_instance = IGEDGUI(self.components)
            
            # Apply performance settings
            self._apply_tkinter_optimizations()
            
            logger.info("Tkinter GUI created successfully")
            return self.gui_instance
            
        except Exception as e:
            logger.error(f"Failed to create tkinter GUI: {e}")
            return None
    
    def _apply_qt6_optimizations(self):
        """Apply performance optimizations for Qt6"""
        if not self.gui_instance:
            return
        
        try:
            from PySide6.QtCore import QThreadPool
            
            # Set thread pool size
            QThreadPool.globalInstance().setMaxThreadCount(
                self.settings['thread_pool_size']
            )
            
            # Configure update timer
            if hasattr(self.gui_instance, 'message_timer'):
                self.gui_instance.message_timer.setInterval(
                    self.settings['update_frequency']
                )
            
            # Disable animations if needed
            if not self.settings['enable_animations']:
                self.gui_instance.setWindowOpacity(1.0)
            
        except Exception as e:
            logger.warning(f"Could not apply Qt6 optimizations: {e}")
    
    def _apply_tkinter_optimizations(self):
        """Apply performance optimizations for tkinter"""
        if not self.gui_instance or not hasattr(self.gui_instance, 'root'):
            return
        
        try:
            # Configure update frequency
            if hasattr(self.gui_instance, 'root'):
                self.gui_instance.root.update_idletasks()
            
        except Exception as e:
            logger.warning(f"Could not apply tkinter optimizations: {e}")
    
    def run(self):
        """Run the GUI application"""
        if not self.gui_instance:
            self.gui_instance = self.create_gui()
        
        if not self.gui_instance:
            logger.error("Cannot run GUI - no instance created")
            return False
        
        try:
            if self.framework == 'qt6':
                self.gui_instance.show()
                return self.app.exec()
            elif self.framework == 'tkinter':
                self.gui_instance.run()
                return True
        except Exception as e:
            logger.error(f"Error running GUI: {e}")
            return False
    
    def stop(self):
        """Stop the GUI application gracefully"""
        try:
            if self.framework == 'qt6' and self.app:
                self.app.quit()
            elif self.framework == 'tkinter' and self.gui_instance:
                if hasattr(self.gui_instance, 'root'):
                    self.gui_instance.root.quit()
        except Exception as e:
            logger.error(f"Error stopping GUI: {e}")
    
    def get_status(self) -> Dict[str, Any]:
        """Get current GUI status and metrics"""
        return {
            'framework': self.framework,
            'running': self.gui_instance is not None,
            'settings': self.settings,
            'performance_metrics': self.optimizer.performance_metrics
        }

class GUILauncher:
    """
    Enhanced launcher for IGED GUI with automatic framework selection
    and comprehensive error handling
    """
    
    def __init__(self, config_path: Optional[Path] = None):
        """
        Initialize GUI Launcher
        
        Args:
            config_path: Optional path to configuration file
        """
        self.config_path = config_path or Path("config/gui_config.json")
        self.config = self._load_config()
        self.components = {}
        
    def _load_config(self) -> Dict[str, Any]:
        """Load configuration from file or use defaults"""
        default_config = {
            'prefer_qt6': True,
            'auto_detect': True,
            'fallback_enabled': True,
            'performance_mode': 'auto',
            'theme': 'dark',
            'log_level': 'INFO'
        }
        
        if self.config_path.exists():
            try:
                with open(self.config_path, 'r') as f:
                    config = json.load(f)
                    default_config.update(config)
            except Exception as e:
                logger.warning(f"Could not load config: {e}")
        
        return default_config
    
    def setup_components(self, components: Optional[Dict[str, Any]] = None):
        """Setup application components"""
        if components:
            self.components = components
        else:
            # Initialize default components if needed
            self.components = self._initialize_default_components()
    
    def _initialize_default_components(self) -> Dict[str, Any]:
        """Initialize default components for standalone operation"""
        components = {}
        
        try:
            # Try to import and initialize core components
            # This allows GUI to run even without full application
            logger.info("Initializing default components for GUI")
            
            # Add mock components for testing
            components['voice'] = None
            components['orchestrator'] = None
            components['memory'] = None
            
        except Exception as e:
            logger.warning(f"Could not initialize default components: {e}")
        
        return components
    
    def launch(self) -> bool:
        """
        Launch the GUI with optimal configuration
        
        Returns:
            True if GUI launched successfully, False otherwise
        """
        try:
            # Setup logging
            logging.basicConfig(
                level=getattr(logging, self.config['log_level']),
                format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            
            logger.info("=" * 60)
            logger.info("IGED GUI Launcher - Starting")
            logger.info("=" * 60)
            
            # Create GUI manager
            manager = GUIManager(
                self.components,
                prefer_qt6=self.config['prefer_qt6']
            )
            
            # Check if GUI can be created
            if not manager.framework:
                logger.error("No GUI framework available!")
                
                if self.config['fallback_enabled']:
                    logger.info("Attempting to install missing dependencies...")
                    if self._install_gui_dependencies():
                        # Retry after installation
                        manager = GUIManager(
                            self.components,
                            prefer_qt6=self.config['prefer_qt6']
                        )
                
                if not manager.framework:
                    return False
            
            # Run GUI
            logger.info(f"Launching GUI with {manager.framework}")
            result = manager.run()
            
            logger.info("GUI closed")
            return result
            
        except KeyboardInterrupt:
            logger.info("GUI interrupted by user")
            return True
        except Exception as e:
            logger.error(f"Fatal error in GUI launcher: {e}", exc_info=True)
            return False
    
    def _install_gui_dependencies(self) -> bool:
        """Attempt to install missing GUI dependencies"""
        try:
            import subprocess
            
            logger.info("Installing PySide6...")
            result = subprocess.run(
                [sys.executable, "-m", "pip", "install", "PySide6>=6.6.1"],
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                logger.info("PySide6 installed successfully")
                return True
            else:
                logger.error(f"Failed to install PySide6: {result.stderr}")
                return False
                
        except Exception as e:
            logger.error(f"Could not install dependencies: {e}")
            return False

def main():
    """Main entry point for GUI application"""
    launcher = GUILauncher()
    launcher.setup_components()
    success = launcher.launch()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()