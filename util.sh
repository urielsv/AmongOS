#!/bin/bash

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default configurations
OS=$(uname)
QEMU_BASE="qemu-system-x86_64"
DRIVE_OPTS="-drive file=Image/x64AmongOS.qcow2,format=qcow2,if=ide"
MEM_DEFAULT="512"
MEM_OPTS="-m $MEM_DEFAULT"
MONITOR_PORT="45454"
GDB_PORT="1234"

# Function to show usage
show_usage() {
    echo -e "${BLUE}AmongOS Development Environment${NC}"
    echo "Usage: $0 [options]"
    echo -e "\n${GREEN}Basic Options:${NC}"
    echo "  normal            Run normally"
    echo "  debug            Basic debug mode (-s -S for GDB)"
    echo "  monitor          Enable QEMU monitor"
    
    echo -e "\n${GREEN}Debug Options:${NC}"
    echo "  --gdb-port=PORT  Set GDB port (default: 1234)"
    echo "  --speed=N        Set execution speed (1-10, higher is slower)"
    echo "  --mem=SIZE       Set memory size in MB (default: 512)"
    echo "  --no-reboot      Quit on reboot"
    echo "  --no-shutdown    Quit on shutdown"
    
    echo -e "\n${GREEN}Trace Options:${NC}"
    echo "  --trace=TYPE     Set trace type (Options: int,exec,in,out,cpu,pcall)"
    echo "  --log=FILE       Save debug output to file"
    
    echo -e "\n${GREEN}Display Options:${NC}"
    echo "  --nographic      Run without graphical output"
    echo "  --display=TYPE   Set display type (sdl,gtk,none)"
    
    echo -e "\n${GREEN}Examples:${NC}"
    echo "  $0 debug --speed=5 --mem=1024"
    echo "  $0 debug --trace=int,exec --log=debug.log"
    echo "  $0 monitor --no-reboot"
}

# Function to check if port is available
check_port() {
    if lsof -Pi :$1 -sTCP:LISTEN -t >/dev/null ; then
        echo -e "${RED}Error: Port $1 is already in use${NC}"
        exit 1
    fi
}

# Function to attach GDB
attach_gdb() {
    echo -e "${YELLOW}To attach GDB, run:${NC}"
    echo "gdb"
    echo "(gdb) target remote localhost:$GDB_PORT"
    echo "(gdb) symbol-file kernel.elf  # if you have debug symbols"
    echo "(gdb) break kmain  # or your entry point"
    echo "(gdb) continue"
}

# Function to show monitor commands
show_monitor_help() {
    echo -e "${YELLOW}QEMU Monitor available on port $MONITOR_PORT${NC}"
    echo "To connect: nc localhost $MONITOR_PORT"
    echo -e "\nUseful monitor commands:"
    echo "  info registers  - Show CPU registers"
    echo "  info cpus      - Show CPU info"
    echo "  info memory    - Show memory info"
    echo "  info blocks    - Show block devices"
    echo "  system_reset   - Reset the system"
    echo "  quit          - Quit QEMU"
}

# Parse command line arguments
DEBUG_OPTS=""
SPEED_SHIFT=0
TRACE_OPTS=""
LOG_FILE=""
ADDITIONAL_OPTS=""

while [[ $# -gt 0 ]]; do
    case $1 in
        normal)
            shift
            ;;
        debug)
            DEBUG_OPTS="-s -S"
            shift
            ;;
        monitor)
            ADDITIONAL_OPTS="$ADDITIONAL_OPTS -monitor tcp:127.0.0.1:$MONITOR_PORT,server,nowait"
            shift
            ;;
        --gdb-port=*)
            GDB_PORT="${1#*=}"
            check_port $GDB_PORT
            DEBUG_OPTS="-gdb tcp::$GDB_PORT"
            shift
            ;;
        --speed=*)
            SPEED_VAL="${1#*=}"
            SPEED_SHIFT=$(( $SPEED_VAL / 2 + 2 ))
            shift
            ;;
        --mem=*)
            MEM_SIZE="${1#*=}"
            MEM_OPTS="-m $MEM_SIZE"
            shift
            ;;
        --trace=*)
            TRACE_TYPE="${1#*=}"
            TRACE_OPTS="-d $TRACE_TYPE"
            shift
            ;;
        --log=*)
            LOG_FILE="${1#*=}"
            shift
            ;;
        --no-reboot)
            ADDITIONAL_OPTS="$ADDITIONAL_OPTS -no-reboot"
            shift
            ;;
        --no-shutdown)
            ADDITIONAL_OPTS="$ADDITIONAL_OPTS -no-shutdown"
            shift
            ;;
        --nographic)
            ADDITIONAL_OPTS="$ADDITIONAL_OPTS -nographic"
            shift
            ;;
        --display=*)
            DISPLAY_TYPE="${1#*=}"
            ADDITIONAL_OPTS="$ADDITIONAL_OPTS -display $DISPLAY_TYPE"
            shift
            ;;
        --help|-h)
            show_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_usage
            exit 1
            ;;
    esac
done

# Construct QEMU command
QEMU_CMD="$QEMU_BASE $DRIVE_OPTS $MEM_OPTS $DEBUG_OPTS $ADDITIONAL_OPTS"

# Add speed control if specified
if [[ $SPEED_SHIFT -gt 0 ]]; then
    QEMU_CMD="$QEMU_CMD -icount shift=$SPEED_SHIFT,sleep=on"
fi

# Add trace options if specified
if [[ -n "$TRACE_OPTS" ]]; then
    QEMU_CMD="$QEMU_CMD $TRACE_OPTS"
fi

# OS-specific configurations
if [[ "$OS" == "Darwin" ]]; then
    echo -e "${BLUE}Running QEMU on macOS...${NC}"
else 
    if [[ "$OS" == "Linux" ]]; then
        if grep -qEi "(Microsoft|WSL)" /proc/version &> /dev/null ; then
            echo -e "${BLUE}Running on WSL2...${NC}"
            if ! pgrep -x "pulseaudio" > /dev/null; then
                echo -e "${YELLOW}PulseAudio is not running. Starting PulseAudio...${NC}"
                pulseaudio --start
            fi
        fi
    fi
    QEMU_CMD="$QEMU_CMD -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0"
    echo -e "${BLUE}Running QEMU on Linux/Windows...${NC}"
fi

# Show debug information if in debug mode
if [[ -n "$DEBUG_OPTS" ]]; then
    attach_gdb
fi

# Show monitor information if monitor is enabled
if [[ $QEMU_CMD == *"monitor tcp"* ]]; then
    show_monitor_help
fi

# Execute the command
echo -e "${GREEN}Executing: $QEMU_CMD${NC}"
if [[ -n "$LOG_FILE" ]]; then
    # If log file is specified, save output there
    $QEMU_CMD 2>&1 | tee "$LOG_FILE"
elif [[ -n "$TRACE_OPTS" ]]; then
    # If tracing is enabled, filter output
    $QEMU_CMD 2>&1 | grep -E 'v=|IN:|OUT:|EIP|CPU'
else
    # Normal execution
    $QEMU_CMD
fi
