#include <stddef.h>

static void *interface = NULL;

void set_engine_trace_interface(void * engine_trace_interface)
{
    interface = engine_trace_interface;
}

struct ray_t
{
    // tbd
};

struct ray_t
{
    // tbd
};

struct trace_filter
{
    // tbd
};

struct trace_t
{
    // tbd
};

void trace_ray(struct ray_t *ray, unsigned int f_mask, struct trace_filter *p_trace_filter, struct trace_t *p_trace)
{
    void **vtable = *(void ***)interface;
    void (*func)(void *, struct ray_t *, unsigned int, struct trace_filter *, struct trace_t *) = vtable[4];

    return func(interface, ray, f_mask, p_trace_filter, p_trace);
}