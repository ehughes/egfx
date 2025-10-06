#!/bin/bash
# Build and run test project with Ninja

set -e

echo "Cleaning build folder..."
if [ -d "build" ]; then
    rm -rf build
fi
mkdir -p build

echo "Running CMake with Ninja generator..."
cd build
cmake .. -G "Ninja"

echo "Building with Ninja..."
ninja

echo "Running executable..."
EXE="egfx_test_320_172_16BPP_RGB565"
if [ -f "$EXE" ]; then
    ./"$EXE"
else
    echo "Error: Executable not found: $EXE"
    cd ..
    exit 1
fi

cd ..
