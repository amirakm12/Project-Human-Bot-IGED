#!/usr/bin/env python3
"""
IGED Quick Run Script
Simple entry point for quick launching
"""

import subprocess
import sys
from pathlib import Path

def main():
    """Quick run script for IGED"""
    print("🔥 IGED Quick Launcher")
    print("=" * 30)
    
    # Check if launcher exists
    if not Path("launcher.py").exists():
        print("❌ launcher.py not found!")
        print("Please run this from the IGED project directory.")
        sys.exit(1)
    
    # Run the launcher
    try:
        subprocess.run([sys.executable, "launcher.py"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"❌ IGED failed to start: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n👋 IGED shutdown requested")
        sys.exit(0)

if __name__ == "__main__":
    main()