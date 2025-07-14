#!/usr/bin/env python3
"""
IGED - Project Human Bot
Main Entry Point

This is the primary entry point for the IGED sovereign AI assistant.
For development and detailed control, you can also use launcher.py directly.
"""

import sys
import os
from pathlib import Path

def main():
    """
    Main entry point for IGED Project Human Bot
    
    This function serves as the primary entry point and delegates
    to the main launcher for actual execution.
    """
    print("🤖 IGED - Project Human Bot")
    print("=" * 50)
    print("🚀 Starting sovereign AI assistant...")
    print()
    
    # Import and run the main launcher
    try:
        from launcher import main as launcher_main
        launcher_main()
    except ImportError as e:
        print(f"❌ Error importing launcher: {e}")
        print("Please ensure all dependencies are installed.")
        print("Run: python install_dependencies.py")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error starting IGED: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()