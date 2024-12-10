#include "../../../config/config.h"
#include "../../../source_sdk/entity/entity.h"
#include "../../../source_sdk/engine_trace/engine_trace.h"
#include "../../../source_sdk/engine_client/engine_client.h"
#include "../../../source_sdk/entity_list/entity_list.h"
#include "../../../source_sdk/math/vec3.h"
#include "../../../source_sdk/user_cmd.h"
#include "../../../utils/math/math_utils.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

float get_fov(struct vec3_t view_angle, struct vec3_t target_view_angle)
{
    float x_diff = target_view_angle.x - view_angle.x;
    float y_diff = target_view_angle.y - view_angle.y;

    float x = remainderf(x_diff, 360.0f);
    float y = remainderf(y_diff, 360.0f);

    float clamped_x = x > 89.0f ? 89.0f : x < -89.0f ? -89.0f : x;
    float clamped_y = y > 180.0f ? 180.0f : y < -180.0f ? -180.0f : y;

    float fov = hypotf(clamped_x, clamped_y);

    return fov;
}

void movement_fix(struct user_cmd *user_cmd, struct vec3_t original_view_angle, float original_forward_move, float original_side_move)
{
    float yaw_delta = user_cmd->viewangles.y - original_view_angle.y;
    float original_yaw_correction = 0;
    float current_yaw_correction = 0;

    if (original_view_angle.y < 0.0f)
    {
        original_yaw_correction = 360.0f + original_view_angle.y;
    }
    else
    {
        original_yaw_correction = original_view_angle.y;
    }
    
    if (user_cmd->viewangles.y < 0.0f)
    {
        current_yaw_correction = 360.0f + user_cmd->viewangles.y;
    }
    else
    {
	current_yaw_correction = user_cmd->viewangles.y;
    }

    if (current_yaw_correction < original_yaw_correction)
    {
        yaw_delta = abs(current_yaw_correction - original_yaw_correction);
    }
    else
    {
        yaw_delta = 360.0f - abs(original_yaw_correction - current_yaw_correction);
    }
    yaw_delta = 360.0f - yaw_delta;

    user_cmd->forwardmove = cos((yaw_delta) * (M_PI/180)) * original_forward_move + cos((yaw_delta + 90.f) * (M_PI/180)) * original_side_move;
    user_cmd->sidemove = sin((yaw_delta) * (M_PI/180)) * original_forward_move + sin((yaw_delta + 90.f) * (M_PI/180)) * original_side_move;
}

bool is_ent_visible(void *localplayer, struct vec3_t *start_pos, void *entity)
{
    struct vec3_t target_pos = get_bone_pos(entity, get_head_bone(entity));
    struct ray_t ray = init_ray(start_pos, &target_pos);
    struct trace_filter filter;
    init_trace_filter(&filter, localplayer);
    struct trace_t trace;
    trace_ray(&ray, 0x4200400b, &filter, &trace);
    if (trace.entity == entity || trace.fraction > 0.97f)
    {
        return true;
    }

    return false;
}

bool is_pos_visible(void *localplayer, struct vec3_t *start_pos, struct vec3_t pos)
{
    struct ray_t ray = init_ray(start_pos, &pos);
    // TBD: Ignore players
    struct trace_filter filter;
    init_trace_filter(&filter, localplayer);
    struct trace_t trace;
    trace_ray(&ray, 0x4200400b, &filter, &trace);
    if ((trace.endpos.x == pos.x && trace.endpos.y == pos.y && trace.endpos.z == pos.z) || trace.fraction > 0.97f)
    {
        return true;
    }

    return false;
}

void *get_closet_fov_ent(void *localplayer, struct vec3_t *shoot_pos, struct vec3_t view_angle, struct vec3_t *result_angle)
{
    void *target_ent = NULL;
    float smallest_fov_angle = __FLT_MAX__;
    int max_clients = get_max_clients();

    for (int ent_index = 1; ent_index <= get_max_entities(); ent_index++)
    {
        void *entity = get_client_entity(ent_index);

        if (entity == NULL || 
            entity == localplayer || 
            is_ent_dormant(entity) || 
            get_ent_team(entity) == get_ent_team(localplayer) ||
            !is_ent_visible(localplayer, shoot_pos, entity))
        {
            continue;
        }

        struct vec3_t ent_pos;
        if (ent_index <= max_clients && get_ent_lifestate(entity) == 1)
        {
            int aim_bone = (get_player_class(localplayer) == TF_CLASS_SNIPER && get_ent_health(entity) > 50) ? get_head_bone(entity) : 1;
            ent_pos = get_bone_pos(entity, aim_bone);
        }
        else
        {
            if (get_class_id(entity) != SENTRY && get_class_id(entity) != DISPENSER && get_class_id(entity) != TELEPORTER)
            {
                continue;
            }

            ent_pos = get_ent_origin(entity);
            ent_pos.z += 5.0f;
        }

        struct vec3_t new_view_angle = get_view_angle(get_difference(ent_pos, *shoot_pos));
        float fov_distance = get_fov(view_angle, new_view_angle);

        if (fov_distance <= config.aimbot.fov && fov_distance < smallest_fov_angle)
        {
            target_ent = entity;
            smallest_fov_angle = fov_distance;
            *result_angle = new_view_angle;
        }
    }

    return target_ent;
}