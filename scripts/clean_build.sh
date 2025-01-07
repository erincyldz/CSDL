#!/bin/bash

# Get the directory of the current script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR/.." # Adjust to point to ProjectRoot

# Remove and recreate the build directory
BUILD_DIR="$PROJECT_ROOT/build"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit

# Run CMake and build
cmake ..
make
