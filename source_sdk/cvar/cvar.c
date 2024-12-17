#include "convar/convar.h"
#include "cvar.h"

#include <stdint.h>
#include <stddef.h>

static void *interface = NULL;

void set_cvar_interface(void *cvar_interface)
{
    interface = cvar_interface;
}

convar find_var(const char *var_name)
{
    void **vtable = *(void ***)interface;
    void *(*func)(void *, const char *) = vtable[12];
  
    return func(interface, var_name);
}

void con_color_printf(struct Color *clr, const char *fmt, ...)
{
    void **vtable = *(void ***)interface;
    void (*func)(void *, struct Color *, const char *, ...) = vtable[23];

    func(interface, clr, fmt);
}

void con_printf(const char *fmt, ...)
{
    void **vtable = *(void ***)interface;
    void (*func)(void *, const char *, ...) = vtable[24];

    func(interface, fmt);
}

