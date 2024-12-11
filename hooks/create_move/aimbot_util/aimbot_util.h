#include "../../../source_sdk/math/vec3.h"
#include "../../../source_sdk/user_cmd.h"

#include <stdbool.h>

float get_fov(struct vec3_t view_angle, struct vec3_t target_view_angle);
void movement_fix(struct user_cmd *user_cmd, struct vec3_t original_view_angle, float original_forward_move, float original_side_move);
bool is_ent_visible(void *localplayer, struct vec3_t *start_pos, void *entity);
bool is_pos_visible(void *localplayer, struct vec3_t *start_pos, struct vec3_t pos);
void *get_closet_fov_ent(void *localplayer, struct vec3_t *shoot_pos, struct vec3_t view_angle, struct vec3_t *result_angle);
