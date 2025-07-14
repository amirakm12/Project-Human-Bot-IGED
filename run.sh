#!/bin/bash

echo "========================================"
echo "IGED - Project Human Bot Launcher"
echo "========================================"
echo

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "❌ ERROR: Python 3 is not installed"
    echo "Please install Python 3.8+ using your package manager"
    exit 1
fi

# Check if main files exist
if [ ! -f "launcher.py" ]; then
    echo "❌ ERROR: launcher.py not found"
    echo "Please run this from the IGED project directory"
    exit 1
fi

# Make the script executable (for future runs)
chmod +x "$0"

# Run IGED
echo "🚀 Starting IGED..."
echo
python3 main.py

# Exit code handling
if [ $? -ne 0 ]; then
    echo
    echo "❌ IGED exited with an error."
    read -p "Press Enter to continue..."
fi