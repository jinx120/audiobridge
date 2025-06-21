#!/bin/bash

# Script to create placeholder icon files for AudioBridge

echo "Creating placeholder icon files for AudioBridge..."

# Create icons directory if it doesn't exist
mkdir -p resources/icons

# Function to create a simple colored square as a placeholder icon
create_icon() {
    local name=$1
    local color=$2
    local size=$3
    
    # Check if convert (ImageMagick) is installed
    if command -v convert &> /dev/null; then
        convert -size ${size}x${size} xc:${color} resources/icons/${name}.png
        echo "Created ${name}.png (${size}x${size}, ${color})"
    else
        echo "ImageMagick not found. Cannot create ${name}.png"
        echo "Please install ImageMagick or manually create icon files."
        echo "See resources/icons/README.md for details."
        exit 1
    fi
}

# Create placeholder icons
create_icon "app_icon" "#3498db" 128
create_icon "send_icon" "#2ecc71" 64
create_icon "receive_icon" "#e74c3c" 64
create_icon "settings_icon" "#f39c12" 64
create_icon "connect_icon" "#27ae60" 64
create_icon "disconnect_icon" "#c0392b" 64

echo ""
echo "Placeholder icons created successfully!"
echo "You can replace these with your own icons later."
echo ""
echo "Now you can build the project using:"
echo "  ./build.sh"
