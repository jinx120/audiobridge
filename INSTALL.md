# AudioBridge Installation Guide

This guide provides instructions for installing the required dependencies for building AudioBridge on different operating systems.

## Prerequisites

AudioBridge requires the following dependencies:

- CMake 3.16 or higher
- Qt 5.15 or Qt 6.x development libraries (Qt5 is recommended and prioritized)
- C++17 compatible compiler
- PortAudio development libraries

## Linux

### Ubuntu/Debian

You can use the provided script to install all dependencies:

```bash
# Make the script executable
chmod +x make_scripts_executable.sh
./make_scripts_executable.sh

# Run the dependency installer
sudo ./install_dependencies_ubuntu.sh
```

Or manually install the dependencies:

```bash
# Update package lists
sudo apt update

# Install build tools
sudo apt install build-essential cmake git

# Install Qt development libraries
sudo apt install qtbase5-dev qtmultimedia5-dev libqt5multimedia5-plugins

# Install PortAudio development libraries
sudo apt install portaudio19-dev
```

### Fedora

```bash
# Install build tools
sudo dnf install gcc-c++ cmake git

# Install Qt development libraries
sudo dnf install qt5-qtbase-devel qt5-qtmultimedia-devel

# Install PortAudio development libraries
sudo dnf install portaudio-devel
```

### Arch Linux

```bash
# Install build tools
sudo pacman -S base-devel cmake git

# Install Qt development libraries
sudo pacman -S qt5-base qt5-multimedia

# Install PortAudio development libraries
sudo pacman -S portaudio
```

## macOS

Using Homebrew:

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install build tools
brew install cmake git

# Install Qt development libraries
brew install qt@5

# Install PortAudio development libraries
brew install portaudio

# Add Qt to your PATH (add this to your .bashrc or .zshrc)
echo 'export PATH="/usr/local/opt/qt@5/bin:$PATH"' >> ~/.zshrc
```

## Windows

### Using MSVC (Recommended)

1. **Install Visual Studio**:
   - Download and install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
   - During installation, select "Desktop development with C++"

2. **Install CMake**:
   - Download and install [CMake](https://cmake.org/download/)
   - Make sure to add CMake to your system PATH

3. **Install Qt**:
   - Download and install [Qt](https://www.qt.io/download-qt-installer)
   - Select Qt 5.15 or later with MSVC components

4. **Install PortAudio**:
   - Option 1: Build from source:
     - Download [PortAudio](http://www.portaudio.com/download.html)
     - Extract and build using CMake
   - Option 2: Use vcpkg:
     ```
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg
     .\bootstrap-vcpkg.bat
     .\vcpkg install portaudio
     ```

### Using MinGW

1. **Install MSYS2**:
   - Download and install [MSYS2](https://www.msys2.org/)
   - Update the package database:
     ```
     pacman -Syu
     ```

2. **Install build tools**:
   ```
   pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake git
   ```

3. **Install Qt development libraries**:
   ```
   pacman -S mingw-w64-x86_64-qt5
   ```

4. **Install PortAudio development libraries**:
   ```
   pacman -S mingw-w64-x86_64-portaudio
   ```

## Building AudioBridge

After installing the dependencies, you can build AudioBridge using the provided build scripts:

- **Linux/macOS**:
  ```bash
  ./build.sh
  ```

- **Windows**:
  ```
  build.bat
  ```

Or manually:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Troubleshooting

### CMake can't find Qt

If you see an error like:

```
CMake Error: Could not find a package configuration file provided by "Qt5" (or "Qt6")
```

You need to specify the Qt installation directory:

```bash
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt
```

For example:
- Windows: `-DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64`
- macOS: `-DCMAKE_PREFIX_PATH=/usr/local/opt/qt@5`
- Linux: Usually not needed if installed from package manager

You can also try installing Qt5 specifically:

- Ubuntu/Debian: `sudo apt install qtbase5-dev qtmultimedia5-dev libqt5multimedia5-plugins`
- Fedora: `sudo dnf install qt5-qtbase-devel qt5-qtmultimedia-devel`
- Arch Linux: `sudo pacman -S qt5-base qt5-multimedia`
- macOS: `brew install qt@5`

### CMake can't find PortAudio

If CMake can't find PortAudio, you may need to specify the PortAudio installation directory:

```bash
cmake .. -DPA_PATH=/path/to/portaudio
```

Or install PortAudio development libraries:

- Ubuntu/Debian: `sudo apt install portaudio19-dev`
- Fedora: `sudo dnf install portaudio-devel`
- Arch Linux: `sudo pacman -S portaudio`
- macOS: `brew install portaudio`

### Missing Icons

Before building, you'll need to add icon files to the `resources/icons` directory. You can use the provided script to create placeholder icons:

```bash
# Make the script executable
chmod +x create_placeholder_icons.sh
./create_placeholder_icons.sh
```

This script requires ImageMagick to be installed:

- Ubuntu/Debian: `sudo apt install imagemagick`
- Fedora: `sudo dnf install ImageMagick`
- Arch Linux: `sudo pacman -S imagemagick`
- macOS: `brew install imagemagick`

Alternatively, you can manually create the icon files as described in the README.md in the resources/icons directory.

### Build Script Permissions

On Linux/macOS, you may need to make the build script executable:

```bash
chmod +x build.sh
```

Or use the provided script:

```bash
chmod +x make_scripts_executable.sh
./make_scripts_executable.sh
```
