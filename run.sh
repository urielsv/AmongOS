#!/bin/bash

# Check the operating system
OS=$(uname)

# Base QEMU command
QEMU_CMD="qemu-system-x86_64 -drive file=Image/x64BareBonesImage.qcow2,format=qcow2,if=ide -m 512"

# Set the QEMU command for debug mode
if [[ "$1" == "debug" ]]; then
    QEMU_CMD="qemu-system-x86_64 -s -S -drive file=Image/x64BareBonesImage.qcow2,format=qcow2,if=ide -m 512 -d int"
fi

# Check if the OS is Darwin (macOS)
if [[ "$OS" == "Darwin" ]]; then
    echo "Running QEMU on macOS..."
else 
    # Assuming Linux/Windows
    if [[ "$OS" == "Linux" ]]; then
        # Check if running under WSL2
        if grep -qEi "(Microsoft|WSL)" /proc/version &> /dev/null ; then
            echo "Running on WSL2..."
            # Ensure PulseAudio is installed and running
            if ! pgrep -x "pulseaudio" > /dev/null; then
                echo "PulseAudio is not running. Starting PulseAudio..."
                pulseaudio --start
            fi
        fi
    fi
    # Add audio options for Linux/Windows
    QEMU_CMD="$QEMU_CMD -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0"
    echo "Running QEMU on Linux/Windows..."
fi

# Execute the command and handle debug output
echo "Executing: $QEMU_CMD"
if [[ "$1" == "debug" ]]; then
    # Capture output and filter for 'v=' lines
    $QEMU_CMD 2>&1 | grep 'v='
else
    # Normal execution
    $QEMU_CMD
fi

