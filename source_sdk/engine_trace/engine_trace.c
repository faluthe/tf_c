#include "../math/vec3.h"

#include <stdbool.h>
#include <stddef.h>

static void *interface = NULL;

void set_engine_trace_interface(void * engine_trace_interface)
{
    interface = engine_trace_interface;
}

enum trace_type_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

struct __attribute__((aligned(16))) vector_aligned
{
    float x, y, z;
};

struct vector_aligned vector_aligned_subtract(struct vec3_t *a, struct vec3_t *b)
{
    struct vector_aligned result = {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z
    };

    return result;
}

struct ray_t
{
    struct vector_aligned start;	    // starting point, centered within the extents
	struct vector_aligned delta;	    // direction + length of the ray
	struct vector_aligned start_offset;	// Add this to m_Start to get the actual ray start
	struct vector_aligned extents;	    // Describes an axis aligned box extruded along a ray
	bool is_ray;	                    // are the extents zero?
	bool is_swept;	                    // is delta != 0?
};

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

struct trace_filter
{
    void **vtable;
    void *skip;
};

// virtual bool ShouldHitEntity( IHandleEntity *pEntity, int contentsMask ) = 0;
bool should_hit_entity(struct trace_filter *interface, void *entity, int contents_mask)
{
    return entity != interface->skip;
}

// virtual TraceType_t GetTraceType() const = 0;
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

struct cplane_t
{
    struct vec3_t normal;
    float dist;
    unsigned char type;
    unsigned char signbits;
    unsigned char pad[2];
};

struct csurface_t
{
    const char *name;
    short surface_props;
    unsigned short flags;
};

struct trace_t
{
    struct vec3_t startpos;
    struct vec3_t endpos;
    struct cplane_t plane;
    float fraction;
    int contents;
    unsigned short disp_flags;
    bool all_solid;
    bool start_solid;
    float fraction_left_solid;
    struct csurface_t surface;
    int hit_group;
    short physics_bone;
    void *entity;
    int hitbox;

    // bool did_hit() const { return m_fraction < 1.f || m_allsolid || m_start_solid; }
};

void trace_ray(struct ray_t *ray, unsigned int f_mask, struct trace_filter *p_trace_filter, struct trace_t *p_trace)
{
    void **vtable = *(void ***)interface;
    void (*func)(void *, struct ray_t *, unsigned int, struct trace_filter *, struct trace_t *) = vtable[4];

    return func(interface, ray, f_mask, p_trace_filter, p_trace);
}