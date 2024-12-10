#include "debug_overlay.h"
#include "../math/vec3.h"

#include <stdbool.h>
#include <stddef.h>

static void *interface = NULL;

void set_debug_overlay_interface(void *debug_overlay_interface)
{
    interface = debug_overlay_interface;
}

void add_box_overlay(struct vec3_t *origin, struct vec3_t *min, struct vec3_t *max, struct vec3_t *orientation, int r, int g, int b, int a, float duration)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, struct vec3_t *, struct vec3_t *, struct vec3_t *, struct vec3_t *, int, int, int, int, float) = vtable[1];
    
    func(interface, origin, min, max, orientation, r, g, b, a, duration);
}

void add_line_overlay(struct vec3_t *origin, struct vec3_t *dest, int r, int g, int b, bool no_depth_test, float duration)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, struct vec3_t *, struct vec3_t *, int, int, int, bool, float) = vtable[3];
    
    func(interface, origin, dest, r, g, b, no_depth_test, duration);
}

int screen_position(struct vec3_t *point, struct vec3_t *screen)
{
    void **vtable = *(void ***)interface;

    int (*func)(void *, struct vec3_t *, struct vec3_t *) = vtable[9];
    
    return func(interface, point, screen);
}