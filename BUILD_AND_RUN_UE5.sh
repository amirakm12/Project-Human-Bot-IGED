#!/bin/bash

# IGED Unreal Engine 5 Build and Launch Script for Linux
echo "========================================"
echo "IGED UE5 - Enterprise AI System"
echo "========================================"
echo ""

# Set UE5 installation path (adjust this to your UE5 location)
UE5_PATH="/opt/UnrealEngine"
PROJECT_PATH="$(dirname "$0")/IGED_UE5"
PROJECT_FILE="$PROJECT_PATH/IGED_UE5.uproject"

echo "Checking Unreal Engine 5 installation..."
if [ ! -d "$UE5_PATH" ]; then
    echo "ERROR: Unreal Engine 5 not found at $UE5_PATH"
    echo "Please install UE5 or update the path in this script"
    exit 1
fi

echo ""
echo "Building IGED UE5 Project..."
echo "========================================"

# Generate project files
echo "Generating project files..."
"$UE5_PATH/Engine/Build/BatchFiles/Linux/Build.sh" -projectfiles -project="$PROJECT_FILE" -game -rocket -progress

# Build the project
echo "Building project..."
"$UE5_PATH/Engine/Build/BatchFiles/Linux/Build.sh" IGED Linux Development "$PROJECT_FILE" -waitmutex

# Cook content
echo "Cooking content..."
"$UE5_PATH/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun -project="$PROJECT_FILE" -noP4 -platform=Linux -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="$PROJECT_PATH/Packaged"

echo ""
echo "========================================"
echo "Build Complete!"
echo "========================================"
echo ""

# Launch the editor
echo "Launching IGED in Unreal Engine 5..."
"$UE5_PATH/Engine/Binaries/Linux/UnrealEditor" "$PROJECT_FILE" &

echo ""
echo "IGED UE5 is now running!"
echo ""