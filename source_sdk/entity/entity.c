#include "../../utils/utils.h"
#include "../entity_list/entity_list.h"
#include "../global_vars/global_vars.h"
#include "../math/vec3.h"
#include "weapon_entity.h"

#include "entity.h"

#include <stdbool.h>
#include <stddef.h>

__int32_t get_ent_flags(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x460);
}

__int32_t get_ent_health(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0xD4);
}

__int32_t get_ent_max_health(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x1df8);
}

__int32_t get_ent_team(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0xDC);
}

__int32_t get_player_class(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x1BA0);
}

struct vec3_t get_ent_origin(void *entity)
{
    // x + 0x328, y + 0x332, z + 0x346
    return *(struct vec3_t *)((__uint64_t)(entity) + 0x328);
}

void *get_collideable(void *entity)
{
    return (void *)((__uint64_t)(entity) + 0x240);
}

void *get_networkable(void *entity)
{
    return (void *)((__uint64_t)(entity) + 0x10);
}

struct vec3_t get_ent_eye_pos(void *entity)
{
    struct vec3_t origin = get_ent_origin(entity);
    float eye_z_diff = *(float *)((__uint64_t)(entity) + 0x14C);

    return (struct vec3_t){origin.x, origin.y, origin.z + eye_z_diff};
}

struct vec3_t get_ent_angles(void *entity)
{
  return *(struct vec3_t *)((__uint64_t)(entity) + 0x334);
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
    void *networkable = get_networkable(entity);
    void **vtable = *(void ***)networkable;

    bool (*func)(void *) = vtable[8];

    return func(networkable);
}

bool get_ent_lifestate(void *entity)
{
    return *(__int8_t *)((__uint64_t)(entity) + 0x746);
}

__int32_t get_active_weapon(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x11D0) & 0xFFF;
}

struct vec3_t *get_collideable_mins(void *entity)
{
    void *collideable = get_collideable(entity);
    void **vtable = *(void ***)collideable;

    struct vec3_t *(*func)(void *) = vtable[1];

    return func(collideable);
}

struct vec3_t *get_collideable_maxs(void *entity)
{
    void *collideable = get_collideable(entity);
    void **vtable = *(void ***)collideable;

    struct vec3_t *(*func)(void *) = vtable[2];

    return func(collideable);
}

int get_tick_base(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x1718);
}

bool can_attack(void *localplayer)
{
    void *active_weapon = get_client_entity(get_active_weapon(localplayer));

    if (active_weapon == NULL)
    {
        return false;
    }

    if (get_player_class(localplayer) == TF_CLASS_HEAVYWEAPONS)
    {
        return true;
    }

    float next_attack = get_next_attack(active_weapon);
    float curtime = get_tick_base(localplayer) * get_global_vars_interval_per_tick();

    if (next_attack > curtime)
    {
        return false;
    }

    return true;
}

int get_head_bone(void *entity)
{
    __int32_t ent_class = get_player_class(entity);

    switch (ent_class)
    {
    case TF_CLASS_SCOUT:
    case TF_CLASS_PYRO:
    case TF_CLASS_SPY:
    case TF_CLASS_MEDIC:
    case TF_CLASS_HEAVYWEAPONS:
        return 6;
    case TF_CLASS_SNIPER:
    case TF_CLASS_SOLDIER:
        return 5;
    case TF_CLASS_DEMOMAN:
        return 16;
    case TF_CLASS_ENGINEER:
        return 8;
    }

    return 0;
}

void *get_client_class(void *entity)
{
    void *networkable = get_networkable(entity);
    void **vtable = *(void ***)networkable;

    void *(*func)(void *) = vtable[2];

    return func(networkable);
}

int get_class_id(void *entity)
{
    void *client_class = get_client_class(entity);
    return *(__int32_t *)((__uint64_t)(client_class) + 0x28);
}

struct vec3_t get_shoot_pos(void* entity)
{
    void **vtable = *(void ***)entity;

    struct vec3_t (*func)(void *) = vtable[302];

    return func(entity);
}

void set_thirdperson(void *entity, bool value)
{
  *(bool *)((__uint64_t)(entity) + 0x240C) = value;
}
