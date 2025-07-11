#!/usr/bin/env python3
"""
IGED Dependency Checker
Checks for required and optional dependencies and provides installation guidance
"""

import importlib
import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple


class DependencyChecker:
    """Manages dependency checking and installation guidance"""

    def __init__(self):
        self.required_deps = {
            "cryptography": "Core encryption functionality",
            "flask": "Web admin interface",
            "flask_cors": "Cross-origin resource sharing",
            "pathlib": "File path utilities",
            "logging": "System logging",
            "threading": "Multi-threading support",
            "json": "JSON data handling",
        }

        self.optional_deps = {
            "speech_recognition": "Voice command processing",
            "whisper": "Offline speech recognition",
            "pyaudio": "Audio input/output",
            "pandas": "Data manipulation and analysis",
            "numpy": "Numerical computing",
            "matplotlib": "Data visualization",
            "seaborn": "Statistical visualization",
            "requests": "HTTP requests",
            "psutil": "System monitoring",
            "python_nmap": "Network scanning",
            "pyinstaller": "Executable creation",
        }

        self.python_version_required = (3, 8)

    def check_python_version(self) -> Tuple[bool, str]:
        """Check if Python version meets requirements"""
        current_version = sys.version_info[:2]
        required_version = self.python_version_required

        if current_version >= required_version:
            return True, f"✅ Python {'.'.join(map(str, current_version))} (OK)"
        else:
            return (
                False,
                f"❌ Python {'.'.join(map(str, current_version))} (Required: {'.'.join(map(str, required_version))}+)",
            )

    def check_dependency(self, package_name: str) -> Tuple[bool, str, Optional[str]]:
        """Check if a specific dependency is available"""
        try:
            if package_name == "flask_cors":
                import flask_cors
            elif package_name == "python_nmap":
                import nmap
            elif package_name == "speech_recognition":
                import speech_recognition
            else:
                importlib.import_module(package_name)

            # Get version if possible
            try:
                if package_name == "flask_cors":
                    version = flask_cors.__version__
                elif package_name == "python_nmap":
                    import nmap

                    version = getattr(nmap, "__version__", "unknown")
                else:
                    module = importlib.import_module(package_name)
                    version = getattr(module, "__version__", "unknown")
                return True, f"✅ {package_name}", version
            except:
                return True, f"✅ {package_name}", None

        except ImportError:
            return False, f"❌ {package_name}", None

    def check_all_dependencies(self) -> Dict[str, Dict]:
        """Check all dependencies and return status"""
        results = {
            "python_version": {},
            "required": {},
            "optional": {},
            "summary": {"required_missing": 0, "optional_missing": 0},
        }

        # Check Python version
        python_ok, python_msg = self.check_python_version()
        results["python_version"] = {"status": python_ok, "message": python_msg}

        # Check required dependencies
        for dep, description in self.required_deps.items():
            available, message, version = self.check_dependency(dep)
            results["required"][dep] = {
                "available": available,
                "message": message,
                "description": description,
                "version": version,
            }
            if not available:
                results["summary"]["required_missing"] += 1

        # Check optional dependencies
        for dep, description in self.optional_deps.items():
            available, message, version = self.check_dependency(dep)
            results["optional"][dep] = {
                "available": available,
                "message": message,
                "description": description,
                "version": version,
            }
            if not available:
                results["summary"]["optional_missing"] += 1

        return results

    def check_directories(self) -> Dict[str, bool]:
        """Check if required directories exist"""
        required_dirs = [
            "config",
            "memory",
            "logs",
            "output",
            "output/data_analysis",
            "output/security",
            "output/network_intelligence",
            "output/remote_control",
            "output/exploits",
            "output/codegen",
            "plugins",
            "agents",
            "ui",
            "admin_panel",
        ]

        results = {}
        for directory in required_dirs:
            path = Path(directory)
            results[directory] = path.exists() and path.is_dir()

        return results

    def check_config_files(self) -> Dict[str, bool]:
        """Check if required configuration files exist"""
        required_files = ["config/secret.key", "requirements.txt", "README.md"]

        results = {}
        for file_path in required_files:
            path = Path(file_path)
            results[file_path] = path.exists() and path.is_file()

        return results

    def generate_installation_command(self) -> str:
        """Generate pip install command for missing dependencies"""
        missing_deps = []
        results = self.check_all_dependencies()

        for dep, info in results["required"].items():
            if not info["available"] and dep not in [
                "pathlib",
                "logging",
                "threading",
                "json",
            ]:
                if dep == "flask_cors":
                    missing_deps.append("flask-cors")
                elif dep == "python_nmap":
                    missing_deps.append("python-nmap")
                elif dep == "speech_recognition":
                    missing_deps.append("SpeechRecognition")
                else:
                    missing_deps.append(dep)

        for dep, info in results["optional"].items():
            if not info["available"]:
                if dep == "flask_cors":
                    missing_deps.append("flask-cors")
                elif dep == "python_nmap":
                    missing_deps.append("python-nmap")
                elif dep == "speech_recognition":
                    missing_deps.append("SpeechRecognition")
                elif dep == "whisper":
                    missing_deps.append("openai-whisper")
                else:
                    missing_deps.append(dep)

        if missing_deps:
            return f"pip install {' '.join(missing_deps)}"
        return "All dependencies are already installed!"

    def print_status_report(self):
        """Print a comprehensive status report"""
        print("\n" + "=" * 60)
        print("🔧 IGED DEPENDENCY STATUS REPORT")
        print("=" * 60)

        # Check dependencies
        results = self.check_all_dependencies()

        # Python version
        print(f"\n📍 {results['python_version']['message']}")

        # Required dependencies
        print(f"\n🔴 REQUIRED DEPENDENCIES:")
        for dep, info in results["required"].items():
            version_str = f" (v{info['version']})" if info["version"] else ""
            print(f"   {info['message']}{version_str} - {info['description']}")

        # Optional dependencies
        print(f"\n🟡 OPTIONAL DEPENDENCIES:")
        for dep, info in results["optional"].items():
            version_str = f" (v{info['version']})" if info["version"] else ""
            print(f"   {info['message']}{version_str} - {info['description']}")

        # Directory check
        print(f"\n📁 DIRECTORY STRUCTURE:")
        dir_results = self.check_directories()
        for directory, exists in dir_results.items():
            status = "✅" if exists else "❌"
            print(f"   {status} {directory}")

        # Config files check
        print(f"\n📄 CONFIGURATION FILES:")
        file_results = self.check_config_files()
        for file_path, exists in file_results.items():
            status = "✅" if exists else "❌"
            print(f"   {status} {file_path}")

        # Summary
        required_missing = results["summary"]["required_missing"]
        optional_missing = results["summary"]["optional_missing"]

        print(f"\n📊 SUMMARY:")
        print(f"   Required dependencies missing: {required_missing}")
        print(f"   Optional dependencies missing: {optional_missing}")

        if required_missing > 0 or optional_missing > 0:
            print(f"\n💡 TO INSTALL MISSING DEPENDENCIES:")
            print(f"   {self.generate_installation_command()}")
            print(f"\n   Or run the automated installer:")
            print(f"   python install_dependencies.py")
            print(f"   install_deps.bat  (Windows)")
        else:
            print(f"\n🎉 ALL DEPENDENCIES ARE INSTALLED!")

        print("=" * 60)


def main():
    """Main function for standalone execution"""
    checker = DependencyChecker()
    checker.print_status_report()


if __name__ == "__main__":
    main()
