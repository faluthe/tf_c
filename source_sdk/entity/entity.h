#ifndef ENTITY_H
#define ENTITY_H

#include "../math/vec3.h"

#include <stdbool.h>
#include <stdint.h>

__int32_t get_ent_flags(void *entity);
__int32_t get_ent_health(void *entity);
__int32_t get_ent_max_health(void *entity);
__int32_t get_ent_team(void *entity);
__int32_t get_player_class(void *entity);
struct vec3_t get_ent_origin(void *entity);
struct vec3_t get_ent_eye_pos(void *entity);
struct vec3_t get_ent_angles(void *entity);
struct vec3_t get_ent_velocity(void *entity);
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
void set_thirdperson(void *entity, bool value);

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

// usefull for using with get_ent_flags()
// EX:  bool on_ground = get_ent_flags(localplayer) & FL_ONGROUND;

enum entity_flags {
    FL_ONGROUND = (1 << 0),
    FL_DUCKING = (1 << 1),
    FL_WATERJUMP = (1 << 2),
    FL_ONTRAIN = (1 << 3),
    FL_INRAIN = (1 << 4),
    FL_FROZEN = (1 << 5),
    FL_ATCONTROLS = (1 << 6),
    FL_CLIENT = (1 << 7),
    FL_FAKECLIENT = (1 << 8),
    FL_INWATER = (1 << 9),
    FL_FLY = (1 << 10),
    FL_SWIM = (1 << 11),
    FL_CONVEYOR = (1 << 12),
    FL_NPC = (1 << 13),
    FL_GODMODE = (1 << 14),
    FL_NOTARGET = (1 << 15),
    FL_AIMTARGET = (1 << 16),
    FL_PARTIALGROUND = (1 << 17),
    FL_STATICPROP = (1 << 18),
    FL_GRAPHED = (1 << 19),
    FL_GRENADE = (1 << 20),
    FL_STEPMOVEMENT = (1 << 21),
    FL_DONTTOUCH = (1 << 22),
    FL_BASEVELOCITY = (1 << 23),
    FL_WORLDBRUSH = (1 << 24),
    FL_OBJECT = (1 << 25),
    FL_KILLME = (1 << 26),
    FL_ONFIRE = (1 << 27),
    FL_DISSOLVING = (1 << 28),
    FL_TRANSRAGDOLL = (1 << 29),
    FL_UNBLOCKABLE_BY_PLAYER = (1 << 30)

};

#endif //ENTITY_H
