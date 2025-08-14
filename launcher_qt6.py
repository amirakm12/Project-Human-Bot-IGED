#!/usr/bin/env python3
"""
IGED Enhanced Launcher - Qt6 Optimized Edition
Intelligent launcher with automatic GUI framework detection and optimization
Ensures maximum compatibility and peak performance
"""

import sys
import os
import logging
import argparse
import json
import subprocess
import platform
from pathlib import Path
from typing import Optional, Dict, Any, List
import time

# Add project root to path
sys.path.insert(0, str(Path(__file__).parent))

# Configure initial logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

class DependencyManager:
    """Manages application dependencies and ensures all requirements are met"""
    
    def __init__(self):
        self.missing_deps = []
        self.optional_deps = []
        self.python_version = sys.version_info
        
    def check_python_version(self) -> bool:
        """Verify Python version meets requirements"""
        min_version = (3, 8)
        if self.python_version < min_version:
            logger.error(f"Python {min_version[0]}.{min_version[1]}+ required, found {sys.version}")
            return False
        return True
    
    def check_core_dependencies(self) -> Dict[str, bool]:
        """Check all core dependencies"""
        deps = {
            'cryptography': 'cryptography>=3.4.8',
            'flask': 'flask>=2.3.3',
            'requests': 'requests>=2.31.0',
            'pandas': 'pandas>=2.0.0',
            'numpy': 'numpy>=1.24.0',
            'psutil': 'psutil>=5.9.0'
        }
        
        results = {}
        for name, requirement in deps.items():
            try:
                __import__(name)
                results[name] = True
            except ImportError:
                results[name] = False
                self.missing_deps.append(requirement)
        
        return results
    
    def check_gui_dependencies(self) -> Dict[str, bool]:
        """Check GUI framework availability"""
        results = {}
        
        # Check PySide6 (Qt6)
        try:
            import PySide6
            from PySide6.QtCore import QT_VERSION_STR
            results['PySide6'] = True
            logger.info(f"✓ PySide6 available (Qt {QT_VERSION_STR})")
        except ImportError:
            results['PySide6'] = False
            self.optional_deps.append('PySide6>=6.6.1')
        
        # Check tkinter
        try:
            import tkinter
            results['tkinter'] = True
            logger.info("✓ tkinter available")
        except ImportError:
            results['tkinter'] = False
            logger.warning("✗ tkinter not available")
        
        return results
    
    def install_missing_dependencies(self, auto_install: bool = False) -> bool:
        """Install missing dependencies"""
        if not self.missing_deps and not self.optional_deps:
            return True
        
        all_deps = self.missing_deps + self.optional_deps
        
        if not auto_install:
            print("\n" + "=" * 60)
            print("Missing Dependencies Detected:")
            print("=" * 60)
            for dep in self.missing_deps:
                print(f"  ✗ {dep} (REQUIRED)")
            for dep in self.optional_deps:
                print(f"  ○ {dep} (optional)")
            
            response = input("\nInstall missing dependencies? [Y/n]: ").strip().lower()
            if response and response != 'y':
                return False
        
        logger.info("Installing dependencies...")
        try:
            # Upgrade pip first
            subprocess.check_call([sys.executable, "-m", "pip", "install", "--upgrade", "pip"])
            
            # Install dependencies
            for dep in all_deps:
                logger.info(f"Installing {dep}...")
                result = subprocess.run(
                    [sys.executable, "-m", "pip", "install", dep],
                    capture_output=True,
                    text=True
                )
                if result.returncode != 0:
                    logger.error(f"Failed to install {dep}: {result.stderr}")
                else:
                    logger.info(f"✓ {dep} installed successfully")
            
            return True
        except Exception as e:
            logger.error(f"Failed to install dependencies: {e}")
            return False

class SystemOptimizer:
    """Optimizes system settings for best performance"""
    
    def __init__(self):
        self.system_info = self._gather_system_info()
        self.optimizations = {}
        
    def _gather_system_info(self) -> Dict[str, Any]:
        """Gather system information for optimization"""
        info = {
            'platform': platform.system(),
            'platform_release': platform.release(),
            'platform_version': platform.version(),
            'architecture': platform.machine(),
            'processor': platform.processor(),
            'python_version': platform.python_version(),
        }
        
        try:
            import psutil
            info['cpu_count'] = psutil.cpu_count()
            info['memory_total'] = psutil.virtual_memory().total
            info['memory_available'] = psutil.virtual_memory().available
        except ImportError:
            # psutil not available, use defaults
            info['cpu_count'] = 4
            info['memory_available'] = 4 * 1024 * 1024 * 1024  # Assume 4GB
        
        return info
    
    def optimize_for_platform(self) -> Dict[str, Any]:
        """Generate platform-specific optimizations"""
        opts = {
            'use_qt6': True,  # Prefer Qt6 for better performance
            'enable_gpu': False,
            'thread_count': 4,
            'cache_enabled': True,
            'log_level': 'INFO'
        }
        
        # Platform-specific optimizations
        if self.system_info['platform'] == 'Windows':
            opts['enable_gpu'] = True
            opts['use_native_dialogs'] = True
        elif self.system_info['platform'] == 'Darwin':  # macOS
            opts['use_metal'] = True
            opts['enable_gpu'] = True
        elif self.system_info['platform'] == 'Linux':
            # Check for GPU support
            if 'DISPLAY' in os.environ:
                opts['enable_gpu'] = True
        
        # CPU-based optimizations
        if 'cpu_count' in self.system_info:
            opts['thread_count'] = min(self.system_info['cpu_count'], 8)
        
        # Memory-based optimizations
        if 'memory_available' in self.system_info:
            if self.system_info['memory_available'] < 2 * 1024 * 1024 * 1024:  # < 2GB
                opts['cache_enabled'] = False
                opts['log_level'] = 'WARNING'
        
        self.optimizations = opts
        return opts

class IGEDLauncher:
    """Main launcher for IGED application with Qt6 support"""
    
    def __init__(self):
        self.dep_manager = DependencyManager()
        self.optimizer = SystemOptimizer()
        self.config = {}
        self.components = {}
        
    def print_banner(self):
        """Display application banner"""
        banner = """
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║     ██╗ ██████╗ ███████╗██████╗      ██████╗ ████████╗ ██████╗
║     ██║██╔════╝ ██╔════╝██╔══██╗    ██╔═══██╗╚══██╔══╝██╔════╝
║     ██║██║  ███╗█████╗  ██║  ██║    ██║   ██║   ██║   ███████╗
║     ██║██║   ██║██╔══╝  ██║  ██║    ██║▄▄ ██║   ██║   ██╔═══██╗
║     ██║╚██████╔╝███████╗██████╔╝    ╚██████╔╝   ██║   ╚██████╔╝
║     ╚═╝ ╚═════╝ ╚══════╝╚═════╝      ╚═════╝    ╚═╝    ╚═════╝
║                                                              ║
║           Enhanced Edition - Qt6 Framework Upgrade          ║
║                    Peak Performance Mode                    ║
╚══════════════════════════════════════════════════════════════╝
        """
        print(banner)
    
    def initialize(self, args: argparse.Namespace) -> bool:
        """Initialize the application"""
        self.print_banner()
        
        # Check Python version
        if not self.dep_manager.check_python_version():
            return False
        
        # Check and install dependencies
        logger.info("Checking dependencies...")
        core_deps = self.dep_manager.check_core_dependencies()
        gui_deps = self.dep_manager.check_gui_dependencies()
        
        if not all(core_deps.values()) or (args.gui and not any(gui_deps.values())):
            if args.auto_install or args.install_deps:
                if not self.dep_manager.install_missing_dependencies(auto_install=args.auto_install):
                    logger.error("Failed to install required dependencies")
                    return False
            else:
                logger.error("Missing required dependencies. Use --install-deps to install")
                return False
        
        # Apply system optimizations
        logger.info("Optimizing for your system...")
        self.config = self.optimizer.optimize_for_platform()
        
        # Override with command-line arguments
        if args.prefer_tkinter:
            self.config['use_qt6'] = False
        if args.log_level:
            self.config['log_level'] = args.log_level.upper()
        
        # Configure logging
        logging.getLogger().setLevel(getattr(logging, self.config['log_level']))
        
        return True
    
    def load_components(self) -> bool:
        """Load application components"""
        try:
            logger.info("Loading application components...")
            
            # Initialize core components
            self.components = {
                'config': self.config,
                'voice': None,  # Will be loaded on demand
                'orchestrator': None,
                'memory': None,
                'optimization_engine': None,
                'data_validator': None
            }
            
            # Try to load actual components
            try:
                from core.orchestrator import Orchestrator
                self.components['orchestrator'] = Orchestrator()
                logger.info("✓ Orchestrator loaded")
            except ImportError:
                logger.warning("Orchestrator not available")
            
            # Load optimization engine
            try:
                from core.optimization_engine import get_optimization_engine
                self.components['optimization_engine'] = get_optimization_engine()
                self.components['optimization_engine'].start_monitoring(interval=30)
                logger.info("✓ Optimization Engine loaded and monitoring started")
            except ImportError:
                logger.warning("Optimization Engine not available")
            
            # Load data validator
            try:
                from core.data_validator import get_validator
                self.components['data_validator'] = get_validator()
                logger.info("✓ Data Validator loaded")
            except ImportError:
                logger.warning("Data Validator not available")
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to load components: {e}")
            return False
    
    def launch_gui(self) -> int:
        """Launch the GUI application"""
        try:
            from ui.win_gui.gui_manager import GUILauncher
            
            logger.info("Launching GUI...")
            launcher = GUILauncher()
            launcher.setup_components(self.components)
            
            # Override config
            launcher.config['prefer_qt6'] = self.config.get('use_qt6', True)
            launcher.config['performance_mode'] = 'optimized'
            
            success = launcher.launch()
            return 0 if success else 1
            
        except ImportError as e:
            logger.error(f"Failed to import GUI manager: {e}")
            return 1
        except Exception as e:
            logger.error(f"Failed to launch GUI: {e}")
            return 1
    
    def launch_cli(self) -> int:
        """Launch CLI mode"""
        logger.info("Launching CLI mode...")
        
        try:
            print("\n" + "=" * 60)
            print("IGED CLI Mode - Type 'help' for commands, 'quit' to exit")
            print("=" * 60 + "\n")
            
            while True:
                try:
                    command = input("IGED> ").strip()
                    
                    if command.lower() in ['quit', 'exit', 'q']:
                        break
                    elif command.lower() == 'help':
                        self.print_cli_help()
                    elif command.lower() == 'status':
                        self.print_status()
                    elif command.lower() == 'gui':
                        return self.launch_gui()
                    else:
                        print(f"Processing: {command}")
                        # Process command through components
                        
                except KeyboardInterrupt:
                    print("\nUse 'quit' to exit")
            
            return 0
            
        except Exception as e:
            logger.error(f"CLI error: {e}")
            return 1
    
    def print_cli_help(self):
        """Print CLI help"""
        help_text = """
Available Commands:
  help    - Show this help message
  status  - Show system status
  gui     - Launch GUI mode
  quit    - Exit the application
        """
        print(help_text)
    
    def print_status(self):
        """Print system status"""
        print("\nSystem Status:")
        print("=" * 40)
        print(f"Platform: {self.optimizer.system_info['platform']}")
        print(f"Python: {self.optimizer.system_info['python_version']}")
        print(f"Qt6 Available: {self.dep_manager.check_gui_dependencies().get('PySide6', False)}")
        print(f"Optimizations: {json.dumps(self.config, indent=2)}")
    
    def run(self, args: argparse.Namespace) -> int:
        """Run the application"""
        try:
            # Initialize
            if not self.initialize(args):
                return 1
            
            # Load components
            if not self.load_components():
                return 1
            
            # Launch appropriate mode
            if args.cli:
                return self.launch_cli()
            elif args.gui or not args.no_gui:
                return self.launch_gui()
            else:
                logger.error("No mode specified")
                return 1
                
        except KeyboardInterrupt:
            logger.info("Application interrupted")
            return 0
        except Exception as e:
            logger.error(f"Fatal error: {e}", exc_info=True)
            return 1

def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="IGED - Enhanced Qt6 Edition",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Mode selection
    mode_group = parser.add_mutually_exclusive_group()
    mode_group.add_argument('--gui', action='store_true', 
                           help='Launch GUI mode (default)')
    mode_group.add_argument('--cli', action='store_true',
                           help='Launch CLI mode')
    mode_group.add_argument('--no-gui', action='store_true',
                           help='Disable GUI mode')
    
    # Framework selection
    parser.add_argument('--prefer-tkinter', action='store_true',
                       help='Prefer tkinter over Qt6')
    
    # Dependency management
    parser.add_argument('--install-deps', action='store_true',
                       help='Install missing dependencies')
    parser.add_argument('--auto-install', action='store_true',
                       help='Automatically install dependencies without prompting')
    
    # Logging
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                       help='Set logging level')
    
    # Performance
    parser.add_argument('--optimize', action='store_true',
                       help='Enable all performance optimizations')
    
    args = parser.parse_args()
    
    # Create and run launcher
    launcher = IGEDLauncher()
    sys.exit(launcher.run(args))

if __name__ == "__main__":
    main()