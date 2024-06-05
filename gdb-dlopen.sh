#!/bin/bash

DEBUG=true
LIB_PATH=$(pwd)/tf_c.so
PROCID=$(pgrep tf_linux64 | head -n 1)
BREAKPOINTS="log_msg"

if [ "$DEBUG" = true ]; then
    echo "Debug mode enabled"
    gcc *.c */*.c -shared -fpic -g -o tf_c.so -Wall
fi

echo -e "Library Path: $LIB_PATH\nProcess ID: $PROCID"

bash scripts/check-attach.sh $LIB_PATH $PROCID

# Verify attach with script return code
if [ $? -ne 0 ]; then
  exit 1
fi

if [ "$DEBUG" = true ]; then
    bash scripts/debug.sh $LIB_PATH $PROCID $BREAKPOINTS
else
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

    LIB_HANDLE=$(sudo gdb -n --batch -ex "attach $PROCID" \
                            -ex "call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)" \
                            -ex "detach" 2> /dev/null | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')

    if [ -z "$LIB_HANDLE" ]; then
        echo "Failed to load library"
        exit 1
    fi

    echo "Library loaded successfully at $LIB_HANDLE. Use Ctrl+C to unload."

    tail -f ./tf_c.log
fi