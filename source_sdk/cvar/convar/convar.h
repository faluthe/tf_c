#include <stdbool.h>
#include <stdint.h>

typedef void * convar;

const char *get_name(convar var);
void set_value(convar var, int value);
__int32_t get_int(convar var);
