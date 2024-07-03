#include "debug_overlay.h"
#include "../math/vec3.h"

#include <stddef.h>

static void *interface = NULL;

void set_debug_overlay_interface(void *debug_overlay_interface)
{
    interface = debug_overlay_interface;
}

int screen_position(struct vec3_t *point, struct vec3_t *screen)
{
    void **vtable = *(void ***)interface;

    int (*func)(void *, struct vec3_t *, struct vec3_t *) = vtable[9];
    
    return func(interface, point, screen);
}