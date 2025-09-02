#!/bin/bash

clear
echo "=============================================="
echo "     IGED - UNREAL ENGINE 5 AI SYSTEM"
echo "=============================================="
echo ""
echo "  🎮 100% Unreal Engine 5"
echo "  🚀 Zero Python Dependencies"
echo "  ⚡ Native C++ Performance"
echo "  🤖 6 AI Agents Ready"
echo ""
echo "=============================================="
echo ""

# Check if running in Windows Subsystem for Linux
if grep -q Microsoft /proc/version; then
    echo "Detected WSL - Launching Windows version..."
    cmd.exe /c "cd IGED_UE5 && BUILD_AND_RUN.bat"
    exit 0
fi

# Check for Unreal Engine 5
UE5_PATHS=(
    "/opt/UnrealEngine"
    "/usr/local/UnrealEngine"
    "$HOME/UnrealEngine"
    "/Applications/Epic Games/UE_5.3"
)

UE5_FOUND=false
for path in "${UE5_PATHS[@]}"; do
    if [ -d "$path" ]; then
        UE5_PATH="$path"
        UE5_FOUND=true
        echo "✅ Found Unreal Engine 5 at: $path"
        break
    fi
done

if [ "$UE5_FOUND" = false ]; then
    echo "❌ Unreal Engine 5 not found!"
    echo ""
    echo "Please install Unreal Engine 5 from:"
    echo "https://www.unrealengine.com/download"
    echo ""
    echo "Or update this script with your UE5 installation path."
    exit 1
fi

echo ""
echo "🔨 Building IGED Project..."
echo "=============================================="

# Generate project files
"$UE5_PATH/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh" \
    -project="$(pwd)/IGED_UE5/IGED_UE5.uproject" \
    -game -rocket -progress 2>/dev/null

# Build the project
echo "Compiling C++ code..."
"$UE5_PATH/Engine/Build/BatchFiles/Linux/Build.sh" \
    IGED Linux Development \
    "$(pwd)/IGED_UE5/IGED_UE5.uproject" \
    -waitmutex 2>/dev/null

echo ""
echo "=============================================="
echo "✅ Build Complete!"
echo "=============================================="
echo ""
echo "🚀 Launching IGED AI System..."
echo ""

# Launch the editor
"$UE5_PATH/Engine/Binaries/Linux/UnrealEditor" \
    "$(pwd)/IGED_UE5/IGED_UE5.uproject" &

echo "=============================================="
echo "   IGED IS NOW RUNNING!"
echo "=============================================="
echo ""
echo "Features:"
echo "  • Code Generation Agent"
echo "  • Data Mining Agent"
echo "  • Security Operations Agent"
echo "  • Network Intelligence Agent"
echo "  • Remote Control Agent"
echo "  • Advanced SecOps Agent"
echo ""
echo "Controls:"
echo "  • Press TAB for menu"
echo "  • Press V for voice input"
echo "  • Press ENTER to execute command"
echo ""
echo "=============================================="