#include "../../config/config.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/user_cmd.h"
#include "../paint_traverse/paint_traverse.h"
#include "aimbot_util/aimbot_util.h"

#include <stddef.h>

void hitscan_aimbot(void *localplayer, struct user_cmd *user_cmd)
{
    struct vec3_t original_view_angle = user_cmd->viewangles;
    float original_side_move = user_cmd->sidemove;
    float original_forward_move = user_cmd->forwardmove;

    struct vec3_t target_view_angle;
    struct vec3_t local_pos = get_shoot_pos(localplayer);
    void *target_ent = get_closet_fov_ent(localplayer, &local_pos, user_cmd->viewangles, &target_view_angle);

    if (target_ent == NULL)
    {
        return;
    }

    add_bbox_decorator(L"TARGET", (struct vec3_t){207, 115, 54}, target_ent);

    if (config.aimbot.key.use_key && config.aimbot.key.is_pressed)
    {
        user_cmd->buttons |= 1;
    }
    
    if (((config.aimbot.key.use_key && config.aimbot.key.is_pressed) ||
        (!config.aimbot.key.use_key && (user_cmd->buttons & 1) != 0)) &&
        can_attack(localplayer))
    {
        user_cmd->viewangles = target_view_angle;
    }

    movement_fix(user_cmd, original_view_angle, original_forward_move, original_side_move);
}