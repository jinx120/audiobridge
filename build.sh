#!/bin/bash

# AudioBridge build script for Linux/macOS

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to print error message and exit
error_exit() {
    echo "ERROR: $1" >&2
    exit 1
}

# Check for required dependencies
echo "Checking dependencies..."

# Check for CMake
if ! command_exists cmake; then
    error_exit "CMake is not installed. Please install CMake and try again."
fi

# Check for C++ compiler
if ! command_exists g++ && ! command_exists clang++; then
    error_exit "No C++ compiler found. Please install g++ or clang++ and try again."
fi

# Check for Qt (basic check)
if ! pkg-config --exists Qt5Core 2>/dev/null && ! pkg-config --exists Qt6Core 2>/dev/null; then
    echo "WARNING: Qt development libraries might not be installed or not in PATH."
    echo "If the build fails, please install Qt5 or Qt6 development libraries."
fi

# Check for PortAudio (basic check)
if ! pkg-config --exists portaudio-2.0 2>/dev/null; then
    echo "WARNING: PortAudio development libraries might not be installed or not in PATH."
    echo "If the build fails, please install PortAudio development libraries."
fi

echo "Creating build directory..."
# Create build directory if it doesn't exist
mkdir -p build
cd build

echo "Configuring with CMake..."
# Configure with CMake
if ! cmake ..; then
    error_exit "CMake configuration failed. Please check the error messages above."
fi

echo "Building the project..."
# Build the project
if ! cmake --build . -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2); then
    error_exit "Build failed. Please check the error messages above."
fi

echo ""
echo "Build completed successfully!"
echo ""
echo "To run AudioBridge, execute:"
echo "  ./AudioBridge"
echo ""
echo "Note: You may need to add icon files to resources/icons/ before building."
echo "See resources/icons/README.md for details."
echo ""
echo "If you encounter any issues, please check the INSTALL.md file for troubleshooting tips."
