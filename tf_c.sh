#!/bin/bash

DEBUG=false
COMPILE=true
if [ "$1" = "debug" ]; then
    DEBUG=true
    BREAKPOINTS=""
fi

LIB_PATH=$(pwd)/tf_c.so
PROCID=$(pgrep tf_linux64 | head -n 1)

if [ "$DEBUG" = true ] || [ "$COMPILE" = true ]; then
    echo "Compiling shared library with $(find . -name '*.c')"
    gcc $(find . -name '*.c') -shared -fpic -g -o tf_c.so -Wall #I would suggest switching to Makefile, over this.
    if [ $? -ne 0 ]; then
	echo "Failed to build"
	exit 1
    fi
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

    #check if log file exists
    ls /tmp/tf_c.log > /dev/null
    if [ $? -ne 0 ]; then
	sudo --user=#1000 touch /tmp/tf_c.log #creates the log file as a user
	if [ $? -ne 0 ]; then
	    echo "Could not create log file"
	    exit 1
	fi
    fi

    LIB_HANDLE=$(sudo gdb -n --batch -ex "attach $PROCID" \
                            -ex "call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)" \
                            -ex "detach" 2> /dev/null | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')

    if [ -z "$LIB_HANDLE" ]; then
        echo "Failed to load library"
        exit 1
    fi

    echo "Library loaded successfully at $LIB_HANDLE. Use Ctrl+C to unload."
    
    tail -f /tmp/tf_c.log
    # rm tf_c.log
    # log file should get automatically cleaned up on power off or boot
fi
