# AudioBridge

AudioBridge is a cross-platform GUI application that allows you to stream audio between computers over a local network. It's particularly useful for scenarios where you want to use a Bluetooth headset connected to one computer for audio input/output on another computer that doesn't have Bluetooth capabilities.

## Features

- **Cross-Platform**: Works on Windows, macOS, and Linux
- **Dual Mode Operation**: 
  - **Sender Mode**: Captures audio from one computer and sends it over the network
  - **Receiver Mode**: Receives audio from the network and plays it on another computer
- **Low Latency**: Optimized for minimal delay, especially in LAN environments
- **Flexible Audio Options**:
  - Raw audio streaming for lowest latency
  - Opus-encoded audio for better bandwidth usage
  - Configurable sample rates and buffer sizes
- **Modern UI**: Clean, intuitive interface with light and dark themes
- **Network Status**: Real-time latency monitoring and connection status

## Use Case Example

1. **Setup**:
   - Computer A: Linux gaming PC with no Bluetooth (connected via Ethernet)
   - Computer B: Windows laptop with Bluetooth headset connected
   
2. **Configuration**:
   - Computer A (Linux): Run AudioBridge in Sender mode
   - Computer B (Windows): Run AudioBridge in Receiver mode
   
3. **Result**:
   - Audio output from Computer A is streamed to Computer B and played through the Bluetooth headset
   - Microphone input from the Bluetooth headset on Computer B is streamed to Computer A

## Building from Source

### Prerequisites

- CMake 3.16 or higher
- Qt 5.15 or Qt 6.x development libraries (Qt5 is recommended and prioritized)
- C++17 compatible compiler
- PortAudio development libraries

See [INSTALL.md](INSTALL.md) for detailed installation instructions for each operating system.

### Build Instructions

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/audiobridge.git
   cd audiobridge
   ```

2. **Using the build scripts (recommended)**:
   
   On Linux/macOS:
   ```bash
   chmod +x build.sh
   ./build.sh
   ```
   
   On Windows:
   ```
   build.bat
   ```

3. **Manual build**:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

4. **Install (optional)**:
   ```bash
   cmake --install .
   ```

### Troubleshooting

If you encounter build issues, please check the [INSTALL.md](INSTALL.md) file for troubleshooting tips.

## Usage

1. **Start AudioBridge** on both computers.

2. **Choose the mode**:
   - On the computer that will send audio (source), select "Sender" mode
   - On the computer that will receive audio (destination), select "Receiver" mode

3. **Configure network settings**:
   - On the receiver, note the IP address
   - On the sender, enter the receiver's IP address

4. **Select audio devices**:
   - Choose the appropriate input and output devices on each computer

5. **Configure audio quality** (optional):
   - Select transmission mode (Raw or Opus)
   - Adjust sample rate and buffer size as needed

6. **Click "Start"** on both computers to begin streaming audio.

## Adding Icons

Before building, you'll need to add icon files to the `resources/icons` directory. See the README.md in that directory for details.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Qt framework for the cross-platform GUI
- PortAudio for cross-platform audio handling
- Opus codec for audio compression (when used)
