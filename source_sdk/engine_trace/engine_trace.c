#include "engine_trace.h"

static void *interface = NULL;

void set_engine_trace_interface(void * engine_trace_interface)
{
    interface = engine_trace_interface;
}

struct vector_aligned vector_aligned_subtract(struct vec3_t *a, struct vec3_t *b)
{
    struct vector_aligned result = {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };

    return result;
}

struct ray_t init_ray(struct vec3_t *start, struct vec3_t *end)
{
    struct vector_aligned delta = vector_aligned_subtract(end, start);
    bool is_swept = (delta.x != 0.0f || delta.y != 0.0f || delta.z != 0.0f);

    struct ray_t ray = {
        .start = { start->x, start->y, start->z },
        .delta = { delta.x, delta.y, delta.z },
        .start_offset = { 0.0f, 0.0f, 0.0f },
        .extents = { 0.0f, 0.0f, 0.0f },
        .is_ray = true,
        .is_swept = is_swept
    };

    return ray;
}

bool should_hit_entity(struct trace_filter *interface, void *entity, int contents_mask)
{
    return entity != interface->skip;
}

enum trace_type_t get_type(struct trace_filter *interface)
{
    return TRACE_EVERYTHING;
}

static void *trace_filter_vtable[] = { should_hit_entity, get_type };

void init_trace_filter(struct trace_filter *filter, void *skip)
{
    filter->vtable = trace_filter_vtable;
    filter->skip = skip;
}

void trace_ray(struct ray_t *ray, unsigned int f_mask, struct trace_filter *p_trace_filter, struct trace_t *p_trace)
{
    void **vtable = *(void ***)interface;
    void (*func)(void *, struct ray_t *, unsigned int, struct trace_filter *, struct trace_t *) = vtable[4];

    return func(interface, ray, f_mask, p_trace_filter, p_trace);
}