#!/bin/bash

# AudioBridge dependency installer for Ubuntu/Debian-based systems

echo "Installing dependencies for AudioBridge on Ubuntu/Debian..."

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to print error message and exit
error_exit() {
    echo "ERROR: $1" >&2
    exit 1
}

# Check if running as root
if [ "$(id -u)" -ne 0 ]; then
    echo "This script needs to be run as root (with sudo)."
    echo "Please run: sudo $0"
    exit 1
fi

echo "Updating package lists..."
apt update || error_exit "Failed to update package lists."

echo "Installing build tools..."
apt install -y build-essential cmake git || error_exit "Failed to install build tools."

echo "Installing Qt5 development libraries..."
apt install -y qtbase5-dev qtmultimedia5-dev libqt5multimedia5-plugins || error_exit "Failed to install Qt5 libraries."

echo "Installing PortAudio development libraries..."
apt install -y portaudio19-dev || error_exit "Failed to install PortAudio libraries."

echo "Installing ImageMagick (for placeholder icons)..."
apt install -y imagemagick || error_exit "Failed to install ImageMagick."

echo ""
echo "All dependencies have been successfully installed!"
echo ""
echo "You can now build AudioBridge using the build script:"
echo "  chmod +x build.sh"
echo "  ./build.sh"
echo ""
echo "If you encounter any issues, please check the INSTALL.md file for troubleshooting tips."
