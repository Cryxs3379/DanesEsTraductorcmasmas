#!/bin/bash
# Build script for Linux/macOS with vcpkg

set -e

echo "Building Traductor Danes-Espanol C++ Native..."

# Check if vcpkg is available
if [ -z "$VCPKG_ROOT" ]; then
    echo "Warning: VCPKG_ROOT not set. Make sure vcpkg is installed and configured."
    VCPKG_ROOT=$(pwd)/vcpkg
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
cmake --build . --config Release --parallel

echo "Build completed successfully!"
echo ""
echo "Executables available in:"
echo "  bin/traductor_cli      - Command line interface"
echo "  bin/traductor_gui      - Qt GUI (if Qt available)"
echo "  bin/traductor_rest     - REST API server (if Drogon available)"
