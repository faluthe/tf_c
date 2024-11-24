#include "../math/vec3.h"

#include <stdbool.h>
#include <stddef.h>

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

struct ray_t
{
    struct vector_aligned start;	    // starting point, centered within the extents
	struct vector_aligned delta;	    // direction + length of the ray
	struct vector_aligned start_offset;	// Add this to m_Start to get the actual ray start
	struct vector_aligned extents;	    // Describes an axis aligned box extruded along a ray
	bool is_ray;	                    // are the extents zero?
	bool is_swept;	                    // is delta != 0?
};

struct trace_filter
{
    void **vtable;
    void *skip;
};

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

struct ray_t init_ray(struct vec3_t *start, struct vec3_t *end);

bool should_hit_entity(struct trace_filter *interface, void *entity, int contents_mask);
enum trace_type_t get_type(struct trace_filter *interface);
void init_trace_filter(struct trace_filter *filter, void *skip);

void set_engine_trace_interface(void * engine_trace_interface);
void trace_ray(struct ray_t *ray, unsigned int f_mask, struct trace_filter *p_trace_filter, struct trace_t *p_trace);