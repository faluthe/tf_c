#include "convar.h"

const char *get_name(convar var)
{
    void **vtable = *(void ***)var;
    const char *(*func)(void *) = vtable[4];
  
    return func(var);
}

void set_value(convar var, int value)
{
    *(__int32_t *)(var + 0x58) = value;
}

__int32_t get_int(convar var)
{
    return *(__int32_t *)(var + 0x58);
}
