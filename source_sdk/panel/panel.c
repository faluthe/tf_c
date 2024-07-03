#include "panel.h"

#include <stddef.h>

static void *interface = NULL;

void set_panel_interface(void *panel_interface)
{
    interface = panel_interface;
}

const char *get_panel_name(void *panel)
{
    void **vtable = *(void ***)interface;

    const char *(*func)(void *, void *) = vtable[37];

    return func(interface, panel);
}