#!/bin/bash

DEBUG=true
LIB_PATH=$(pwd)/tf_c.so
PROCID=$(pgrep tf_linux64 | head -n 1)

if [ "$DEBUG" = true ]; then
    echo "Debug mode enabled"
    gcc *.c */*.c -shared -fpic -g -o tf_c.so -Wall
fi

echo -e "Library Path: $LIB_PATH\nProcess ID: $PROCID"

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

unload() {
    echo -e "\nUnloading library with handle $LIB_HANDLE"
    
    RC=$(gdb -n --batch -ex "attach $PROCID" \
                   -ex "call ((int (*) (void *)) dlclose)((void *) $LIB_HANDLE)" \
                   -ex "call ((char * (*) (void)) dlerror)()" \
                   -ex "detach" 2> /dev/null | grep -oP '\$2 = 0x\K[0-9a-f]+')
    
    if [[ "$RC" == "0" ]]; then
        echo "Library unloaded successfully"
    else
        echo "Failed to unload library"
    fi
}

trap unload SIGINT

if [ "$DEBUG" = true ]; then
    GDB_OUT=$(sudo gdb -n --batch -ex "attach $PROCID" \
                            -ex "call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)" \
                            -ex "call ((char * (*) (void)) dlerror)()" -ex "detach")
    
    echo "$GDB_OUT"
    LIB_HANDLE=$(echo "$GDB_OUT" | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')
    ERROR=$(echo "$GDB_OUT" | grep -oP '\$2 = 0x[0-9a-f]+ "\K[^"]+')
    
    if [ -n "$ERROR" ]; then
        echo "Error: $ERROR"
        exit 1
    fi
else
    LIB_HANDLE=$(sudo gdb -n --batch -ex "attach $PROCID" \
                            -ex "call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)" \
                            -ex "detach" 2> /dev/null | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')
fi

if [ -z "$LIB_HANDLE" ]; then
    echo "Failed to load library"
    exit 1
fi

echo "Library loaded successfully at $LIB_HANDLE. Use Ctrl+C to unload."

tail -f ./tf_c.log