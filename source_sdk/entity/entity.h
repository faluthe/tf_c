#include "../math/vec3.h"

#include <stdint.h>

__int32_t get_ent_flags(void *entity);
__int32_t get_ent_health(void *entity);
__int32_t get_ent_team(void *entity);
struct vec3_t get_ent_origin(void *entity);
struct vec3_t get_ent_eye_pos(void *entity);
struct vec3_t get_ent_velocity(void *entity);
__int32_t setup_bones(void *entity, void *bone_to_world_out, __int32_t max_bones, __int32_t bone_mask, float current_time);
struct vec3_t get_bone_pos(void *entity, int bone_num);