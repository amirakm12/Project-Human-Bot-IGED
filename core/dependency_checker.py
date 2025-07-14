"""
Dependency Checker for IGED
Validates and manages system dependencies

This module provides comprehensive dependency checking and management
for the IGED system, ensuring all required packages are available.
"""

import importlib
import logging
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Any

logger = logging.getLogger(__name__)


class DependencyChecker:
    """
    Comprehensive dependency checker and manager for IGED.
    
    This class handles validation, installation, and management of
    all required dependencies for the IGED system.
    """
    
    def __init__(self):
        """Initialize the dependency checker with default settings."""
        self.required_packages = {
            'core': [
                'cryptography',
                'pathlib',
                'json',
                'logging',
                'threading',
                'queue'
            ],
            'web': [
                'flask',
                'flask_cors'
            ],
            'voice': [
                'speech_recognition',
                'whisper'
            ],
            'data': [
                'pandas',
                'numpy',
                'matplotlib'
            ],
            'security': [
                'nmap',
                'requests',
                'psutil'
            ],
            'optional': [
                'tkinter',
                'pynput',
                'pyautogui',
                'PIL'
            ]
        }
        
        self.dependency_status: Dict[str, Any] = {}
        self.missing_dependencies: List[str] = []
        self.failed_imports: List[str] = []
    
    def check_all_dependencies(self) -> Dict[str, Any]:
        """
        Check all system dependencies.
        
        Returns:
            Dictionary containing complete dependency status report
        """
        logger.info("🔧 Checking all system dependencies...")
        
        self.dependency_status.clear()
        self.missing_dependencies.clear()
        self.failed_imports.clear()
        
        total_packages = 0
        available_packages = 0
        
        for category, packages in self.required_packages.items():
            category_status = {}
            category_available = 0
            
            for package in packages:
                total_packages += 1
                status = self._check_package(package)
                category_status[package] = status
                
                if status['available']:
                    available_packages += 1
                    category_available += 1
                else:
                    self.missing_dependencies.append(package)
                    if category != 'optional':
                        self.failed_imports.append(package)
            
            self.dependency_status[category] = {
                'packages': category_status,
                'available_count': category_available,
                'total_count': len(packages),
                'availability_ratio': (category_available / len(packages)) * 100
            }
        
        # Generate summary report
        report = {
            'total_packages': total_packages,
            'available_packages': available_packages,
            'missing_packages': len(self.missing_dependencies),
            'critical_missing': len(self.failed_imports),
            'overall_ratio': (available_packages / total_packages) * 100,
            'dependency_status': self.dependency_status,
            'missing_dependencies': self.missing_dependencies,
            'failed_imports': self.failed_imports,
            'system_compatible': len(self.failed_imports) == 0
        }
        
        self._log_dependency_report(report)
        return report
    
    def _check_package(self, package_name: str) -> Dict[str, Any]:
        """
        Check availability of a specific package.
        
        Args:
            package_name: Name of the package to check
            
        Returns:
            Dictionary containing package status information
        """
        try:
            # Try to import the package
            module = importlib.import_module(package_name)
            
            # Get package version if available
            version = getattr(module, '__version__', 'unknown')
            
            # Get package location
            location = getattr(module, '__file__', 'unknown')
            
            return {
                'available': True,
                'version': version,
                'location': location,
                'import_time': self._measure_import_time(package_name)
            }
            
        except ImportError as e:
            return {
                'available': False,
                'error': str(e),
                'import_time': None
            }
        except Exception as e:
            return {
                'available': False,
                'error': f"Unexpected error: {str(e)}",
                'import_time': None
            }
    
    def _measure_import_time(self, package_name: str) -> Optional[float]:
        """
        Measure the time it takes to import a package.
        
        Args:
            package_name: Name of the package to measure
            
        Returns:
            Import time in seconds, or None if measurement fails
        """
        try:
            import time
            start_time = time.time()
            importlib.import_module(package_name)
            end_time = time.time()
            return end_time - start_time
        except Exception:
            return None
    
    def _log_dependency_report(self, report: Dict[str, Any]) -> None:
        """
        Log dependency report with appropriate log levels.
        
        Args:
            report: Dependency report to log
        """
        overall_ratio = report['overall_ratio']
        
        if overall_ratio >= 90:
            logger.info(f"✅ Dependency check complete: {overall_ratio:.1f}% available")
        elif overall_ratio >= 70:
            logger.warning(f"⚠️ Some dependencies missing: {overall_ratio:.1f}% available")
        else:
            logger.error(f"❌ Many dependencies missing: {overall_ratio:.1f}% available")
        
        if report['critical_missing'] > 0:
            logger.error(f"❌ Critical dependencies missing: {report['failed_imports']}")
        
        if report['missing_dependencies']:
            logger.info(f"📋 Missing packages: {report['missing_dependencies']}")
    
    def install_missing_dependencies(self, category: Optional[str] = None) -> Dict[str, Any]:
        """
        Install missing dependencies.
        
        Args:
            category: Specific category to install, or None for all
            
        Returns:
            Dictionary containing installation results
        """
        logger.info("📦 Installing missing dependencies...")
        
        packages_to_install = []
        
        if category:
            if category in self.required_packages:
                packages_to_install = [
                    pkg for pkg in self.required_packages[category]
                    if pkg in self.missing_dependencies
                ]
            else:
                logger.error(f"❌ Unknown category: {category}")
                return {'success': False, 'error': f'Unknown category: {category}'}
        else:
            # Install all missing non-optional packages
            for cat, packages in self.required_packages.items():
                if cat != 'optional':
                    packages_to_install.extend([
                        pkg for pkg in packages
                        if pkg in self.missing_dependencies
                    ])
        
        if not packages_to_install:
            logger.info("✅ No packages to install")
            return {'success': True, 'installed_packages': [], 'message': 'No packages to install'}
        
        installation_results = {}
        successful_installs = []
        failed_installs = []
        
        for package in packages_to_install:
            try:
                result = self._install_package(package)
                installation_results[package] = result
                
                if result['success']:
                    successful_installs.append(package)
                    logger.info(f"✅ Successfully installed: {package}")
                else:
                    failed_installs.append(package)
                    logger.error(f"❌ Failed to install: {package} - {result.get('error', 'Unknown error')}")
                    
            except Exception as e:
                failed_installs.append(package)
                installation_results[package] = {'success': False, 'error': str(e)}
                logger.error(f"❌ Installation error for {package}: {e}")
        
        return {
            'success': len(failed_installs) == 0,
            'installed_packages': successful_installs,
            'failed_packages': failed_installs,
            'installation_results': installation_results,
            'total_attempted': len(packages_to_install)
        }
    
    def _install_package(self, package_name: str) -> Dict[str, Any]:
        """
        Install a specific package using pip.
        
        Args:
            package_name: Name of the package to install
            
        Returns:
            Dictionary containing installation result
        """
        try:
            # Map some package names to their pip equivalents
            pip_package_name = self._get_pip_package_name(package_name)
            
            # Run pip install
            result = subprocess.run(
                [sys.executable, '-m', 'pip', 'install', pip_package_name],
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            
            if result.returncode == 0:
                return {
                    'success': True,
                    'package': package_name,
                    'pip_package': pip_package_name,
                    'output': result.stdout
                }
            else:
                return {
                    'success': False,
                    'package': package_name,
                    'pip_package': pip_package_name,
                    'error': result.stderr,
                    'return_code': result.returncode
                }
                
        except subprocess.TimeoutExpired:
            return {
                'success': False,
                'package': package_name,
                'error': 'Installation timeout (5 minutes)'
            }
        except Exception as e:
            return {
                'success': False,
                'package': package_name,
                'error': str(e)
            }
    
    def _get_pip_package_name(self, package_name: str) -> str:
        """
        Get the pip package name for a given import name.
        
        Args:
            package_name: Import name of the package
            
        Returns:
            Pip package name
        """
        # Mapping of import names to pip package names
        pip_mapping = {
            'speech_recognition': 'SpeechRecognition',
            'PIL': 'Pillow',
            'cv2': 'opencv-python',
            'sklearn': 'scikit-learn',
            'flask_cors': 'Flask-CORS'
        }
        
        return pip_mapping.get(package_name, package_name)
    
    def check_python_version(self) -> Dict[str, Any]:
        """
        Check Python version compatibility.
        
        Returns:
            Dictionary containing Python version information
        """
        version_info = sys.version_info
        version_string = f"{version_info.major}.{version_info.minor}.{version_info.micro}"
        
        # Check if version meets minimum requirements
        min_version = (3, 8)
        compatible = version_info[:2] >= min_version
        
        return {
            'version': version_string,
            'version_info': version_info[:3],
            'compatible': compatible,
            'minimum_required': f"{min_version[0]}.{min_version[1]}",
            'platform': sys.platform,
            'executable': sys.executable
        }
    
    def check_system_requirements(self) -> Dict[str, Any]:
        """
        Check system requirements and compatibility.
        
        Returns:
            Dictionary containing system requirement status
        """
        requirements = {
            'python_version': self.check_python_version(),
            'available_memory': self._check_available_memory(),
            'disk_space': self._check_disk_space(),
            'network_access': self._check_network_access()
        }
        
        # Determine overall compatibility
        compatible = all([
            requirements['python_version']['compatible'],
            requirements['available_memory']['sufficient'],
            requirements['disk_space']['sufficient']
        ])
        
        requirements['overall_compatible'] = compatible
        
        return requirements
    
    def _check_available_memory(self) -> Dict[str, Any]:
        """
        Check available system memory.
        
        Returns:
            Dictionary containing memory information
        """
        try:
            if 'psutil' in [pkg for cat in self.required_packages.values() for pkg in cat]:
                import psutil
                memory = psutil.virtual_memory()
                
                # Require at least 1GB of available memory
                min_memory_gb = 1.0
                available_gb = memory.available / (1024**3)
                sufficient = available_gb >= min_memory_gb
                
                return {
                    'total_gb': memory.total / (1024**3),
                    'available_gb': available_gb,
                    'used_percent': memory.percent,
                    'sufficient': sufficient,
                    'minimum_required_gb': min_memory_gb
                }
            else:
                return {
                    'available': False,
                    'error': 'psutil not available for memory check',
                    'sufficient': True  # Assume sufficient if we can't check
                }
                
        except Exception as e:
            return {
                'available': False,
                'error': str(e),
                'sufficient': True  # Assume sufficient if we can't check
            }
    
    def _check_disk_space(self) -> Dict[str, Any]:
        """
        Check available disk space.
        
        Returns:
            Dictionary containing disk space information
        """
        try:
            if 'psutil' in [pkg for cat in self.required_packages.values() for pkg in cat]:
                import psutil
                disk = psutil.disk_usage('.')
                
                # Require at least 500MB of free space
                min_space_gb = 0.5
                free_gb = disk.free / (1024**3)
                sufficient = free_gb >= min_space_gb
                
                return {
                    'total_gb': disk.total / (1024**3),
                    'free_gb': free_gb,
                    'used_percent': (disk.used / disk.total) * 100,
                    'sufficient': sufficient,
                    'minimum_required_gb': min_space_gb
                }
            else:
                return {
                    'available': False,
                    'error': 'psutil not available for disk check',
                    'sufficient': True  # Assume sufficient if we can't check
                }
                
        except Exception as e:
            return {
                'available': False,
                'error': str(e),
                'sufficient': True  # Assume sufficient if we can't check
            }
    
    def _check_network_access(self) -> Dict[str, Any]:
        """
        Check network access for package installation.
        
        Returns:
            Dictionary containing network access information
        """
        try:
            import urllib.request
            import socket
            
            # Test connection to PyPI
            socket.setdefaulttimeout(10)
            urllib.request.urlopen('https://pypi.org')
            
            return {
                'available': True,
                'can_install_packages': True
            }
            
        except Exception as e:
            return {
                'available': False,
                'can_install_packages': False,
                'error': str(e)
            }
    
    def generate_requirements_file(self, file_path: str = "requirements.txt") -> bool:
        """
        Generate a requirements.txt file with all dependencies.
        
        Args:
            file_path: Path where to save the requirements file
            
        Returns:
            True if file was generated successfully, False otherwise
        """
        try:
            requirements_content = []
            
            # Add header
            requirements_content.append("# IGED System Requirements")
            requirements_content.append("# Generated automatically by DependencyChecker")
            requirements_content.append("")
            
            # Add packages by category
            for category, packages in self.required_packages.items():
                if category == 'optional':
                    continue  # Skip optional packages
                    
                requirements_content.append(f"# {category.title()} dependencies")
                
                for package in packages:
                    pip_name = self._get_pip_package_name(package)
                    
                    # Try to get version if package is available
                    if package in self.dependency_status:
                        pkg_status = self.dependency_status[package]
                        if pkg_status.get('available') and pkg_status.get('version') != 'unknown':
                            requirements_content.append(f"{pip_name}>={pkg_status['version']}")
                        else:
                            requirements_content.append(pip_name)
                    else:
                        requirements_content.append(pip_name)
                
                requirements_content.append("")
            
            # Write to file
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write('\n'.join(requirements_content))
            
            logger.info(f"📋 Requirements file generated: {file_path}")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to generate requirements file: {e}")
            return False
    
    def get_dependency_summary(self) -> str:
        """
        Get a human-readable dependency summary.
        
        Returns:
            Formatted string containing dependency summary
        """
        if not self.dependency_status:
            return "❌ No dependency check has been performed yet."
        
        summary_lines = []
        summary_lines.append("🔧 IGED Dependency Summary")
        summary_lines.append("=" * 30)
        
        for category, status in self.dependency_status.items():
            available = status['available_count']
            total = status['total_count']
            ratio = status['availability_ratio']
            
            if ratio == 100:
                status_icon = "✅"
            elif ratio >= 70:
                status_icon = "⚠️"
            else:
                status_icon = "❌"
            
            summary_lines.append(f"{status_icon} {category.title()}: {available}/{total} ({ratio:.1f}%)")
            
            # List missing packages
            missing_in_category = []
            for pkg, pkg_status in status['packages'].items():
                if not pkg_status['available']:
                    missing_in_category.append(pkg)
            
            if missing_in_category:
                summary_lines.append(f"   Missing: {', '.join(missing_in_category)}")
        
        if self.missing_dependencies:
            summary_lines.append("")
            summary_lines.append("📋 Installation command:")
            pip_packages = [self._get_pip_package_name(pkg) for pkg in self.missing_dependencies]
            summary_lines.append(f"pip install {' '.join(pip_packages)}")
        
        return '\n'.join(summary_lines) 