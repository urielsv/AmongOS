#!/bin/bash

# Check the operating system
OS=$(uname)

# Set the QEMU command
QEMU_CMD="qemu-system-x86_64 -s -hda Image/x64BareBonesImage.qcow2 -m 512"

# Check if the OS is Darwin (macOS)
if [[ "$OS" == "Darwin" ]]; then
    # Additional options for macOS can be added here
    echo "Running QEMU on macOS..."
else
    # Assuming Linux/Windows (adapt for your needs)
    QEMU_CMD="$QEMU_CMD -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0"
    echo "Running QEMU on Linux/Windows..."
fi

# Execute the command
echo "Executing: $QEMU_CMD"
$QEMU_CMD
