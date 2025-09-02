#!/bin/bash

echo "=========================================="
echo "   IGED - UNREAL ENGINE 5 AI SYSTEM"
echo "=========================================="
echo ""
echo "Starting IGED UE5..."
echo ""

# Set your UE5 path here
UE5_PATH="/opt/UnrealEngine"

if [ ! -d "$UE5_PATH" ]; then
    echo "ERROR: Unreal Engine 5 not found!"
    echo "Please install from: https://www.unrealengine.com"
    exit 1
fi

echo "Launching IGED in Unreal Engine 5..."
"$UE5_PATH/Engine/Binaries/Linux/UnrealEditor" "$(dirname "$0")/IGED_UE5.uproject" &

echo ""
echo "IGED UE5 is running!"