# Work in progress

LIB_PATH=$1
PROCID=$2
BREAKPOINTS=""

gdb_command="gdb -ex 'attach $PROCID' -ex 'set breakpoint pending on'"

for (( i=3; i<=$#; i++ )); do
  gdb_command+=" -ex 'break ${!i}'"
done

gdb_command+=" -ex \"call ((void* (*) (const char*, int)) dlopen)(\\\"$LIB_PATH\\\", 1)\""
gdb_command+=" -ex \"call ((char* (*) (void)) dlerror)()\""

eval $gdb_command

# for (( i=3; i<=$#; i++ )); do  
#   BREAKPOINTS+="-ex \"break log_msg\""
# done

# echo BREAKPOINTS: $BREAKPOINTS
# echo "-ex \"attach $PROCID\" -ex \"set breakpoint pending on\" $BREAKPOINTS -ex \"call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)\" -ex \"call ((char * (*) (void)) dlerror)()\""

# sudo gdb -ex "attach $PROCID" -ex "set breakpoint pending on" \
#          $BREAKPOINTS -ex "call ((void * (*) (const char*, int)) dlopen)(\"$LIB_PATH\", 1)" \
#          -ex "call ((char * (*) (void)) dlerror)()"

# echo "$GDB_OUT"
# LIB_HANDLE=$(echo "$GDB_OUT" | grep -oP '\$1 = \(void \*\) \K0x[0-9a-f]+')
# ERROR=$(echo "$GDB_OUT" | grep -oP '\$2 = 0x[0-9a-f]+ "\K[^"]+')

# if [ -n "$ERROR" ]; then
#     echo "Error: $ERROR"
#     exit 1
# fi

# echo -e "\nUnloading library with handle $LIB_HANDLE"

# RC=$(gdb -n --batch -ex "attach $PROCID" \
#             -ex "call ((int (*) (void *)) dlclose)((void *) $LIB_HANDLE)" \
#             -ex "call ((char * (*) (void)) dlerror)()" \
#             -ex "detach" | grep -oP '\$2 = 0x\K[0-9a-f]+')

# if [[ "$RC" == "0" ]]; then
#     echo "Library unloaded successfully"
# else
#     echo "Failed to unload library"
# fi