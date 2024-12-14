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

void set_ent_flags(void *entity, __int32_t flags)
{
    *(__int32_t *)((__uint64_t)(entity) + 0x460) = flags;
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

void set_ent_origin(void *entity, float x, float y, float z)
{
    struct vec3_t origin = {x, y, z};
    *(struct vec3_t *)((__uint64_t)(entity) + 0x328) = origin;
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

void set_ent_angles(void *entity, struct vec3_t angles)
{
    *(struct vec3_t *)((__uint64_t)(entity) + 0x334) = angles;
}

struct vec3_t get_ent_velocity(void *entity)
{
    return *(struct vec3_t *)((__uint64_t)(entity) + 0x168);
}

void set_ent_velocity(void *entity, struct vec3_t velocity)
{
    *(struct vec3_t *)((__uint64_t)(entity) + 0x168) = velocity;
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

void set_tick_base(void *entity, int tick_base)
{
    *(__int32_t *)((__uint64_t)(entity) + 0x1718) = tick_base;
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

void set_current_command(void *entity, void *command)
{
    *(__uint64_t *)((__uint64_t)(entity) + 0x1620) = (__uint64_t)(command);
}

bool player_is_ducking(void *entity)
{
    return get_ent_flags(entity) & 2;
}

__uint8_t get_b_ducked(void *entity)
{
    return *(__uint8_t *)((__uint64_t)(entity) + 0x1288);
}

void set_b_ducked(void *entity, __uint8_t ducked)
{
    *(__uint8_t *)((__uint64_t)(entity) + 0x1288) = ducked;
}

float get_duck_time(void *entity)
{
    return *(float *)((__uint64_t)(entity) + 0x128C);
}

void set_duck_time(void *entity, float time)
{
    *(float *)((__uint64_t)(entity) + 0x128C) = time;
}

float get_duck_jump_time(void *entity)
{
    return *(float *)((__uint64_t)(entity) + 0x1290);
}

void set_duck_jump_time(void *entity, float time)
{
    *(float *)((__uint64_t)(entity) + 0x1290) = time;
}

__uint8_t get_b_ducking(void *entity)
{
    return *(__uint8_t *)((__uint64_t)(entity) + 0x1289);
}

void set_b_ducking(void *entity, __uint8_t ducking)
{
    *(__uint8_t *)((__uint64_t)(entity) + 1289) = ducking;
}

__uint8_t get_b_in_duck_jump(void *entity)
{
    return *(__uint8_t *)((__uint64_t)(entity) + 0x1290);
}

void set_b_in_duck_jump(void *entity, __uint8_t in_duck_jump)
{
    *(__uint8_t *)((__uint64_t)(entity) + 0x1290) = in_duck_jump;
}

__int32_t get_ground_entity_handle(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0x31C);
}

void set_ground_entity_handle(void *entity, __int32_t handle)
{
    *(__int32_t *)((__uint64_t)(entity) + 0x31C) = handle;
}

float get_model_scale(void *entity)
{
    return *(float *)((__uint64_t)(entity) + 0x91C);
}

void set_model_scale(void *entity, float scale)
{
    *(float *)((__uint64_t)(entity) + 0x91C) = scale;
}

void *get_handle_entity(void *entity)
{
    void *collideable = get_collideable(entity);
    void **vtable = *(void ***)collideable;

    void *(*func)(void *) = vtable[0];

    return func(collideable);
}

__uint64_t get_eh_handle(void *entity)
{
    // void *handle_entity = get_handle_entity(entity);

    // log_msg("Handle entity: %p\n", handle_entity);

    // void **vtable = *(void ***)handle_entity;

    // void *(*func)(void *) = vtable[2];

    // return func(entity);

    return *(__uint64_t *)((__uint64_t)(entity) + 0x8);
}

struct vec3_t get_base_velocity(void *entity)
{
    return *(struct vec3_t *)((__uint64_t)(entity) + 0x1F8);
}

void set_base_velocity(void *entity, struct vec3_t velocity)
{
    *(struct vec3_t *)((__uint64_t)(entity) + 0x1F8) = velocity;
}

struct vec3_t get_view_offset(void *entity)
{
    return *(struct vec3_t *)((__uint64_t)(entity) + 0x144);
}

void set_view_offset(void *entity, struct vec3_t view_offset)
{
    *(struct vec3_t *)((__uint64_t)(entity) + 0x144) = view_offset;
}

void set_player_button_state(void *entity, int buttons)
{
    static void (*func)(void *, int) = NULL;

    if (func == NULL)
    {
        // Pattern: E8 ?? ?? ?? ?? 48 8B 03 49 8D 75 10
        __uint8_t pattern[] = {0xE8, 0x0, 0x0, 0x0, 0x0, 0x48, 0x8B, 0x03, 0x49, 0x8D, 0x75, 0x10};
        void *scan_result = pattern_scan("client.so", pattern, "x????xxxxxxx");

        if (!scan_result)
        {
            log_msg("Failed to find SetPlayerButtonState pattern\n");
            return;
        }

        __uint32_t rel_addr = *(__uint32_t *)((__uint64_t)scan_result + 1);
        __uint64_t abs_addr = (__uint64_t)scan_result + 5 + rel_addr;

        func = (__typeof__(func))abs_addr;



        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState pattern found at %p\n", scan_result);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
        log_msg("UpdateButtonState found at %p\n", func);
    }

    func(entity, buttons);
}

void set_local_view_angles(void *entity, struct vec3_t view_angles)
{
    void **vtable = *(void ***)entity;

    void (*func)(void *, struct vec3_t) = vtable[366];

    func(entity, view_angles);
}

bool physics_run_think(void *entity, int thinkmethod)
{
    bool (*func)(void *, int) = NULL;

    if (func == NULL)
    {
        __uint8_t pattern[] = {0xF6, 0x87, 0x0, 0x0, 0x0, 0x0, 0x0F, 0x85, 0x0, 0x0, 0x0, 0x0, 0x55, 0x83, 0xFE, 0x02};
        void *scan_result = pattern_scan("client.so", pattern, "xx?????xx????xxxx");

        if (!scan_result)
        {
            log_msg("Failed to find PhysicsRunThink pattern\n");
            return false;
        }

        func = scan_result;
    }

    return func(entity, thinkmethod);
}

void pre_think(void *entity)
{
    void **vtable = *(void ***)entity;

    void (*func)(void *) = vtable[325];

    func(entity);
}

int get_player_next_think_tick(void *entity)
{
    return *(__int32_t *)((__uint64_t)(entity) + 0xB4);
}

void set_next_think(void *entity, int next_think)
{
    void (*func)(void *, int, void *) = NULL;

    if (func == NULL)
    {
        __uint8_t pattern[] = {0xE8, 0x0, 0x0, 0x0, 0x0, 0x48, 0x8B, 0x03, 0xF3, 0x0F, 0x10, 0x40, 0x0};
        void *scan_result = pattern_scan("client.so", pattern, "x????xxxxxxx?");

        if (!scan_result)
        {
            log_msg("Failed to find SetNextThink pattern\n");
            return;
        }

        __uint32_t rel_addr = *(__uint32_t *)((__uint64_t)scan_result + 1);
        __uint64_t abs_addr = (__uint64_t)scan_result + 5 + rel_addr;

        log_msg("SetNextThink found at %p\n", abs_addr);

        func = (__typeof__(func))abs_addr;
    }

    func(entity, next_think, NULL);
}

void player_think(void *entity)
{
    void **vtable = *(void ***)entity;

    void (*func)(void *) = vtable[174];

    func(entity);
}

void post_think(void *entity)
{
    void **vtable = *(void ***)entity;

    void (*func)(void *) = vtable[326];

    func(entity);
}