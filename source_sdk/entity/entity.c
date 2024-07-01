#include "../math/vec3.h"
#include "../../utils/utils.h"

#include "entity.h"

#include <stdbool.h>

__int32_t get_ent_flags(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x460);
}

__int32_t get_ent_health(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0xD4);
}

__int32_t get_ent_team(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0xDC);
}

struct vec3_t get_ent_origin(void *entity)
{
    // x + 0x340, y + 0x344, z + 0x348
    return *(struct vec3_t *)((__uint64_t)(entity) + 0x340);
}

struct vec3_t get_ent_eye_pos(void *entity)
{
    struct vec3_t origin = get_ent_origin(entity);
    float eye_z_diff = *(float *)((__uint64_t)(entity) + 0x14C);

    return (struct vec3_t){origin.x, origin.y, origin.z + eye_z_diff};
}

struct vec3_t get_ent_velocity(void *entity)
{
    return *(struct vec3_t *)((__uint64_t)(entity) + 0x168);
}

__int32_t setup_bones(void *entity, void *bone_to_world_out, __int32_t max_bones, __int32_t bone_mask, float current_time)
{
    void **vtable = *(void ***)entity;

    __int32_t (*func)(void *, void *, __int32_t, __int32_t, float) = vtable[96];

    return func(entity, bone_to_world_out, max_bones, bone_mask, current_time);
}

struct vec3_t get_bone_pos(void *entity, int bone_num)
{
    // 128 bones, 3x4 matrix
    float bone_to_world_out[128][3][4];
    if (setup_bones(entity, bone_to_world_out, 128, 0x100, 0.0f))
    {
        // Saw this in the source leak, don't know how it works
        return (struct vec3_t){bone_to_world_out[bone_num][0][3], bone_to_world_out[bone_num][1][3], bone_to_world_out[bone_num][2][3]};
    }

    return (struct vec3_t){0.0f, 0.0f, 0.0f};
}

bool is_ent_dormant(void *entity)
{
    void *networkable = (void *)((__uint64_t)(entity) + 0x10);
    void **vtable = *(void ***)networkable;

    bool (*func)(void *) = vtable[8];

    return func(networkable);
}

bool get_ent_lifestate(void *entity)
{
    return *(__int8_t *)((__uint64_t)(entity) + 0x746);
}