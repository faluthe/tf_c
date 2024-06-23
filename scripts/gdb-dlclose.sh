#!/bin/bash

LIB_PATH=${1:-$(pwd)/tf_c.so}
PROCID=${2:-$(pgrep tf_linux64 | head -n 1)}

# RTLD_NOLOAD | RTLD_NOW = 6

LIB_HANDLE=$(gdb -n --batch -ex "attach $PROCID" \
                    -ex "set \$handle = ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 6)" \
                    -ex "p \$handle" \
                    -ex "detach" 2> /dev/null | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')

echo $LIB_PATH loaded at $LIB_HANDLE. Unloading...

if [[ -n "$LIB_HANDLE" ]]; then
    RC=$(gdb -n --batch -ex "attach $PROCID" \
            -ex "call ((int (*) (void *)) dlclose)((void *) $LIB_HANDLE)" \
            -ex "call ((char * (*) (void)) dlerror)()" \
            -ex "detach" 2> /dev/null | grep -oP '\$2 = 0x\K[0-9a-f]+')
    
    if [[ "$RC" == "0" ]]; then
        echo "Library unloaded successfully"
    else
        echo "Failed to unload library"
    fi
else
    echo "Failed to load library"
fi
