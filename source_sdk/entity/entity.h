#ifndef ENTITY_H
#define ENTITY_H

#include "../math/vec3.h"

#include <stdbool.h>
#include <stdint.h>

__int32_t get_ent_flags(void *entity);
void set_ent_flags(void *entity, __int32_t flags);
__int32_t get_ent_health(void *entity);
__int32_t get_ent_max_health(void *entity);
__int32_t get_ent_team(void *entity);
__int32_t get_player_class(void *entity);
struct vec3_t get_ent_origin(void *entity);
void set_ent_origin(void *entity, float x, float y, float z);
struct vec3_t get_ent_eye_pos(void *entity);
struct vec3_t get_ent_angles(void *entity);
void set_ent_angles(void *entity, struct vec3_t angles);
struct vec3_t get_ent_velocity(void *entity);
void set_ent_velocity(void *entity, struct vec3_t velocity);
__int32_t setup_bones(void *entity, void *bone_to_world_out, __int32_t max_bones, __int32_t bone_mask, float current_time);
struct vec3_t get_bone_pos(void *entity, int bone_num);
bool is_ent_dormant(void *entity);
bool get_ent_lifestate(void *entity);
__int32_t get_active_weapon(void *entity);
struct vec3_t* get_collideable_mins(void *entity);
struct vec3_t* get_collideable_maxs(void *entity);
int get_tick_base(void *entity);
int get_head_bone(void *entity);
bool can_attack(void *localplayer);
int get_class_id(void *entity);
struct vec3_t get_shoot_pos(void* entity);
void set_current_command(void *entity, void *command);
bool player_is_ducking(void *entity);
__uint8_t get_b_ducked(void *entity);
void set_b_ducked(void *entity, __uint8_t ducked);
float get_duck_time(void *entity);
void set_duck_time(void *entity, float time);
float get_duck_jump_time(void *entity);
void set_duck_jump_time(void *entity, float time);
__uint8_t get_b_ducking(void *entity);
void set_b_ducking(void *entity, __uint8_t ducking);
__uint8_t get_b_in_duck_jump(void *entity);
void set_b_in_duck_jump(void *entity, __uint8_t in_duck_jump);
__int32_t get_ground_entity_handle(void *entity);
void set_ground_entity_handle(void *entity, __int32_t handle);
float get_model_scale(void *entity);
void set_model_scale(void *entity, float scale);
__uint64_t get_eh_handle(void *entity);
struct vec3_t get_base_velocity(void *entity);
void set_base_velocity(void *entity, struct vec3_t velocity);
struct vec3_t get_view_offset(void *entity);
void set_view_offset(void *entity, struct vec3_t offset);

enum
{
	TF_CLASS_UNDEFINED = 0,
	TF_CLASS_SCOUT,
    TF_CLASS_SNIPER,
    TF_CLASS_SOLDIER,
	TF_CLASS_DEMOMAN,
	TF_CLASS_MEDIC,
	TF_CLASS_HEAVYWEAPONS,
	TF_CLASS_PYRO,
	TF_CLASS_SPY,
	TF_CLASS_ENGINEER
};

enum class_id
{
	AMMO_OR_HEALTH_PACK = 1,
	DISPENSER = 86,
	SENTRY = 88,
	TELEPORTER = 89,
	ARROW = 122,
	ROCKET = 264,
	PILL_OR_STICKY = 217,
	FLARE = 257,
	CROSSBOW_BOLT = 259,
};

struct player_info_t
{
	char name[32];
	int user_id;
	char guid[33];
	unsigned long friends_id;
	char friends_name[32];
	bool fakeplayer;
	bool ishltv;
	unsigned long custom_files[4];
	unsigned char files_downloaded;
};

#endif //ENTITY_H
