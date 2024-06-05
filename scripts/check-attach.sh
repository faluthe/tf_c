#!/bin/bash

LIB_PATH=$1
PROCID=$2

# Check if gdb is installed
if ! command -v gdb &> /dev/null
then
    echo "GDB is not installed"
    exit 1
fi

# Verify permissions
if [[ "$EUID" -ne 0 ]]; then
    echo "Please run as root"
    exit 1
fi

# Check if library exists
if [ ! -f "$LIB_PATH" ]; then
    echo "Library $LIB_PATH not found"
    exit 1
fi

# Check if library is already loaded (dont print anything)
if lsof -p $PROCID 2> /dev/null | grep -q $LIB_PATH; then
    echo "Library is already loaded"
    exit 1
fi

# Check if the process is running
if [ -z "$PROCID" ]; then
    echo "Process is not running"
    exit 1
fi

exit 0