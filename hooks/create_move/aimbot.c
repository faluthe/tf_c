#include "../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/entity/weapon_entity.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/engine_trace/engine_trace.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/estimate_abs_velocity/estimate_abs_velocity.h"
#include "../../source_sdk/global_vars/global_vars.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/net_channel_info/net_channel_info.h"
#include "../../source_sdk/user_cmd.h"
#include "../../utils/math/math_utils.h"
#include "../../utils/utils.h"
#include "../../x11/x11.h"
#include "../paint_traverse/paint_traverse.h"
#include "../hooks.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static bool hold_shot = false;
static bool use_aim_key = false;

float get_rocket_predicted_time(struct vec3_t ent_velocity, struct vec3_t ent_distance, bool ent_in_air, int rocket_speed_per_second)
{
    const int gravity = 800;

    // Not on the ground (affected by gravity)
    if (ent_in_air)
    {
        float e = (ent_distance.z * ent_distance.z) + (ent_distance.y * ent_distance.y) + (ent_distance.x * ent_distance.x);
        float d = (2 * ent_distance.x * ent_velocity.x) + (2 * ent_distance.y * ent_velocity.y) + (2 * ent_distance.z * ent_velocity.z);
        float c = (ent_velocity.z * ent_velocity.z) - (ent_distance.z * gravity) + (ent_distance.y * ent_distance.y) + (ent_velocity.x * ent_velocity.x) - (rocket_speed_per_second * rocket_speed_per_second);
        float b = (-1 * ent_velocity.z * gravity);
        float a = 0.25 * (gravity * gravity);

        double poly[5] = { e, d, c, b, a };
        double solutions[4];
        int num_sols = solve_real_poly(4, poly, solutions);
        double best_solution = __DBL_MAX__;
        for (int i = 0; i < num_sols; i++)
        {
            if (solutions[i] > 0.0 && solutions[i] < best_solution)
            {
                best_solution = solutions[i];
            }
        }

        return best_solution == __DBL_MAX__ ? -1.0f : best_solution;
    }
    else
    {
        int a = (rocket_speed_per_second * rocket_speed_per_second) - (ent_velocity.x * ent_velocity.x) - (ent_velocity.y * ent_velocity.y) - (ent_velocity.z * ent_velocity.z);
        int b = -2 * ( (ent_distance.x * ent_velocity.x) + (ent_distance.y * ent_velocity.y) + (ent_distance.z * ent_velocity.z) );
        int c = -1 * ( (ent_distance.x * ent_distance.x) + (ent_distance.y * ent_distance.y) + (ent_distance.z * ent_distance.z) );
        return positive_quadratic_root((float)a, (float)b, (float)c);

    }
}

bool is_ent_visible(void *localplayer, void *entity)
{
    struct vec3_t local_pos = get_ent_eye_pos(localplayer);
    struct vec3_t target_pos = get_bone_pos(entity, get_head_bone(entity));
    struct ray_t ray = init_ray(&local_pos, &target_pos);
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

bool is_pos_visible(void *localplayer, struct vec3_t pos)
{
    struct vec3_t local_pos = get_ent_eye_pos(localplayer);
    struct ray_t ray = init_ray(&local_pos, &pos);
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

struct vec3_t get_view_angle(struct vec3_t diff)
{
    // Common side between two right triangles
    float c = sqrt((diff.x * diff.x) + (diff.y * diff.y));

    float pitch_angle = atan2(diff.z, c) * 180 / M_PI;
    float yaw_angle = atan2(diff.y, diff.x) * 180 / M_PI;

    struct vec3_t view_angle = {
        .x = -pitch_angle,
        .y = yaw_angle,
        .z = 0
    };

    return view_angle;
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

void projectile_aimbot(void *localplayer, struct user_cmd *user_cmd, int weapon_id)
{
    struct vec3_t original_view_angle = user_cmd->viewangles;
    float original_side_move = user_cmd->sidemove;
    float original_forward_move = user_cmd->forwardmove;

    void *target_ent = NULL;
    float target_predicted_time = -1.0f;
    struct vec3_t projectile_target_view_angle;
    struct vec3_t projectile_target_pos;
    float smallest_fov_angle = __FLT_MAX__;
    void *net_channel_info = get_net_channel_info();
        
    if (net_channel_info == NULL)
    {
        return;
    }

    // Draw where a rocket was previously shot
    static struct vec3_t esp_projectile_pos = {0.0f, 0.0f, 0.0f};
    static float esp_predicted_time = 0.0f;
    const float projectile_esp_linger = 0.5f;
    if (esp_projectile_pos.x != 0.0f && esp_projectile_pos.y != 0.0f && esp_projectile_pos.z != 0.0f)
    {
        struct vec3_t projectile_predicted_screen;
        if (screen_position(&esp_projectile_pos, &projectile_predicted_screen) == 0)
        {
            float curtime = get_global_vars_curtime();
            float time_diff = esp_predicted_time - curtime;
            struct vec3_t color = time_diff > 0.0f ? (struct vec3_t) {75, 169, 200} : (struct vec3_t) {255, 75, 75};
            float data = time_diff > 0.0f ? time_diff : 0.0f;
            add_to_render_queue(L"rocket", (int)projectile_predicted_screen.x, (int)projectile_predicted_screen.y, color, data);
            if (curtime > esp_predicted_time + projectile_esp_linger)
            {
                esp_projectile_pos = (struct vec3_t){0.0f, 0.0f, 0.0f};
            }
        }
    }

    // Prioritize sentries
    for (int ent_index = 1; ent_index < get_max_entities(); ent_index++)
    {
        void *entity = get_client_entity(ent_index);

        if (entity == NULL || get_class_id(entity) != SENTRY)
        {
            continue;
        }

        if (is_ent_dormant(entity) || get_ent_team(entity) == get_ent_team(localplayer))
        {
            continue;
        }

        struct vec3_t ent_pos = get_ent_origin(entity);
        struct vec3_t ent_difference = get_difference(ent_pos, get_ent_eye_pos(localplayer));

        if (is_ent_visible(localplayer, entity))
        {
            float ent_distance = get_distance(ent_pos, get_ent_eye_pos(localplayer));
            float fov_distance = sqrt(powf(sin((user_cmd->viewangles.x - get_view_angle(ent_difference).x) * (M_PI / 180) ) * ent_distance, 2.0) + powf(sin((user_cmd->viewangles.y - get_view_angle(ent_difference).y) * (M_PI / 180)) * ent_distance, 2.0));

            if (fov_distance < 4*90 && fov_distance < smallest_fov_angle)
            {
                target_ent = entity;
                smallest_fov_angle = fov_distance;
                projectile_target_view_angle = get_view_angle(ent_difference);
                projectile_target_pos = ent_pos;
            }
        }
    }

    if (target_ent == NULL)
    {
        for (int ent_index = 1; ent_index <= get_max_clients(); ent_index++)
        {
            void *entity = get_client_entity(ent_index);

            if (entity == NULL || entity == localplayer)
            {
                continue;
            }

            if (is_ent_dormant(entity) || get_ent_lifestate(entity) != 1 || get_ent_team(entity) == get_ent_team(localplayer))
            {
                continue;
            }

            float latency = get_latency(net_channel_info, 0) + get_latency(net_channel_info, 1) + 0.05;
            struct vec3_t local_pos = get_ent_eye_pos(localplayer);
            struct vec3_t ent_velocity;
            estimate_abs_velocity(entity, &ent_velocity);
            bool ent_in_air = (get_ent_flags(entity) & 1) == 0;
            // In order of priority
            struct vec3_t ent_pos[] = {
                ent_in_air ? get_bone_pos(entity, 1) : get_ent_origin(entity),
                ent_in_air ? get_ent_origin(entity) : get_bone_pos(entity, 1),
                get_bone_pos(entity, get_head_bone(entity)),
            };

            for (int i = 0; i < 2; i++)
            {
                struct vec3_t ent_difference = get_difference(ent_pos[i], local_pos);
                
                float predicted_time = -1.0f;
                switch (get_player_class(localplayer))
                {
                    // TODO: Properly handle all weapons
                    case TF_CLASS_SOLDIER:
                        int rocket_speed_per_second = weapon_id == TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT ? 1980 : 1100;
                        predicted_time = get_rocket_predicted_time(ent_velocity, ent_difference, ent_in_air, rocket_speed_per_second);
                        break;
                    default:
                        return;
                }

                if (predicted_time == -1.0f)
                {
                    continue;
                }

                struct vec3_t rocket_predicted_pos = ent_pos[i];
                rocket_predicted_pos.x += (ent_velocity.x * predicted_time);
                rocket_predicted_pos.y += (ent_velocity.y * predicted_time);
                if (ent_in_air)
                {
                    rocket_predicted_pos.z += (ent_velocity.z * predicted_time) - (0.5f * 800 * (predicted_time * predicted_time));
                }
                else
                {
                    rocket_predicted_pos.z += (ent_velocity.z * predicted_time);
                }

                rocket_predicted_pos.x += (ent_velocity.x * latency);
                rocket_predicted_pos.y += (ent_velocity.y * latency);
                rocket_predicted_pos.z += (ent_velocity.z * latency);

                // TODO: Check if the rocket is going to hit a wall
                if (is_pos_visible(localplayer, rocket_predicted_pos))
                {
                    struct vec3_t target_view_angle = get_view_angle(ent_difference);
                    struct vec3_t new_view_angle = get_view_angle(get_difference(rocket_predicted_pos, local_pos));
                    
                    float ent_distance = get_distance(ent_pos[i], local_pos);
                    float fov_distance = sqrt(powf(sin((user_cmd->viewangles.x - target_view_angle.x) * (M_PI / 180) ) * ent_distance, 2.0) + powf(sin((user_cmd->viewangles.y - target_view_angle.y) * (M_PI / 180)) * ent_distance, 2.0));

                    if (fov_distance < 4*90 && fov_distance < smallest_fov_angle)
                    {
                        target_ent = entity;
                        target_predicted_time = predicted_time;
                        smallest_fov_angle = fov_distance;
                        projectile_target_view_angle = new_view_angle;
                        projectile_target_pos = rocket_predicted_pos;
                    }
                }
            }
        }
    }

    // Lowest priority: other buildings
    if (target_ent == NULL)
    {
        for (int ent_index = 1; ent_index < get_max_entities(); ent_index++)
        {
            void *entity = get_client_entity(ent_index);

            if (entity == NULL || is_ent_dormant(entity) || get_ent_team(entity) == get_ent_team(localplayer) || (get_class_id(entity) != DISPENSER && get_class_id(entity) != TELEPORTER))
            {
                continue;
            }

            struct vec3_t ent_pos = get_ent_origin(entity);
            struct vec3_t ent_difference = get_difference(ent_pos, get_ent_eye_pos(localplayer));

            if (is_ent_visible(localplayer, entity))
            {
                float ent_distance = get_distance(ent_pos, get_ent_eye_pos(localplayer));
                float fov_distance = sqrt(powf(sin((user_cmd->viewangles.x - get_view_angle(ent_difference).x) * (M_PI / 180) ) * ent_distance, 2.0) + powf(sin((user_cmd->viewangles.y - get_view_angle(ent_difference).y) * (M_PI / 180)) * ent_distance, 2.0));

                if (fov_distance < 4*90 && fov_distance < smallest_fov_angle)
                {
                    target_ent = entity;
                    smallest_fov_angle = fov_distance;
                    projectile_target_view_angle = get_view_angle(ent_difference);
                    projectile_target_pos = ent_pos;
                }
            }
        }
    }

    if (target_ent == NULL)
    {
        if (hold_shot)
        {
            user_cmd->buttons &= ~1;
        }

        return;
    }

    add_bbox_decorator(L"TARGET", (struct vec3_t){207, 115, 54}, target_ent);

    // Draw where aiming now
    struct vec3_t target_screen;
    if (screen_position(&projectile_target_pos, &target_screen) == 0)
    {
        add_to_render_queue(L"V", (int)target_screen.x, (int)target_screen.y, (struct vec3_t){207, 115, 54}, 0.0f);
    }

    if (use_aim_key && is_key_down('c')) user_cmd->buttons |= 1;

    if (((use_aim_key && is_key_down('c')) || (user_cmd->buttons & 1) != 0) && can_attack(localplayer))
    {
        user_cmd->viewangles = projectile_target_view_angle;
        esp_projectile_pos = projectile_target_pos;
        esp_predicted_time = get_global_vars_curtime() + target_predicted_time;
    }

    movement_fix(user_cmd, original_view_angle, original_forward_move, original_side_move);
}

void hitscan_aimbot(void *localplayer, struct user_cmd *user_cmd)
{
    struct vec3_t original_view_angle = user_cmd->viewangles;
    float original_side_move = user_cmd->sidemove;
    float original_forward_move = user_cmd->forwardmove;

    void *target_ent = NULL;
    struct vec3_t target_view_angle;
    float smallest_fov_angle = __FLT_MAX__;

    for (int ent_index = 1; ent_index <= get_max_clients(); ent_index++)
    {
        void *entity = get_client_entity(ent_index);

        if (entity == NULL || entity == localplayer)
        {
            continue;
        }

        if (is_ent_dormant(entity) || get_ent_lifestate(entity) != 1 || get_ent_team(entity) == get_ent_team(localplayer) || !is_ent_visible(localplayer, entity))
        {
            continue;
        }

        int aim_bone = (get_player_class(localplayer) != TF_CLASS_SNIPER || get_ent_health(entity) <= 50) ? 1 : get_head_bone(entity);
        struct vec3_t ent_pos = get_bone_pos(entity, aim_bone);
        struct vec3_t local_pos = get_ent_eye_pos(localplayer);
        struct vec3_t ent_difference = get_difference(ent_pos, local_pos);
        struct vec3_t new_view_angle = get_view_angle(ent_difference);

        float ent_distance = get_distance(ent_pos, local_pos);
        float fov_distance = sqrt(powf(sin((user_cmd->viewangles.x - new_view_angle.x) * (M_PI / 180) ) * ent_distance, 2.0) + powf(sin((user_cmd->viewangles.y - new_view_angle.y) * (M_PI / 180)) * ent_distance, 2.0));
    
        if (fov_distance < smallest_fov_angle)
        {
            target_ent = entity;
            smallest_fov_angle = fov_distance;
            target_view_angle = new_view_angle;
        }
    }

    // Hitscan prioritizes players over all buildings
    if (target_ent == NULL)
    {
        for (int ent_index = 1; ent_index < get_max_entities(); ent_index++)
        {
            void *entity = get_client_entity(ent_index);

            if (entity == NULL || 
                is_ent_dormant(entity) || 
                get_ent_team(entity) == get_ent_team(localplayer) || 
                (get_class_id(entity) != SENTRY && get_class_id(entity) != DISPENSER && get_class_id(entity) != TELEPORTER))
            {
                continue;
            }

            struct vec3_t ent_pos = get_ent_origin(entity);
            struct vec3_t ent_difference = get_difference(ent_pos, get_ent_eye_pos(localplayer));

            if (is_ent_visible(localplayer, entity))
            {
                float ent_distance = get_distance(ent_pos, get_ent_eye_pos(localplayer));
                float fov_distance = sqrt(powf(sin((user_cmd->viewangles.x - get_view_angle(ent_difference).x) * (M_PI / 180) ) * ent_distance, 2.0) + powf(sin((user_cmd->viewangles.y - get_view_angle(ent_difference).y) * (M_PI / 180)) * ent_distance, 2.0));

                if (fov_distance < 4*90 && fov_distance < smallest_fov_angle)
                {
                    target_ent = entity;
                    smallest_fov_angle = fov_distance;
                    target_view_angle = get_view_angle(ent_difference);
                }
            }
        }
    }

    if (target_ent == NULL)
    {
        return;
    }

    add_bbox_decorator(L"TARGET", (struct vec3_t){207, 115, 54}, target_ent);

    if (use_aim_key && is_key_down('c')) user_cmd->buttons |= 1;
    
    if (((use_aim_key && is_key_down('c')) || (user_cmd->buttons & 1) != 0) && can_attack(localplayer))
    {
        user_cmd->viewangles = target_view_angle;
    }

    movement_fix(user_cmd, original_view_angle, original_forward_move, original_side_move);
}

void aimbot(void *localplayer, struct user_cmd *user_cmd)
{
    void *active_weapon = get_client_entity(get_active_weapon(localplayer));
    if (active_weapon == NULL)
    {
        return;
    }
    int weapon_id = get_weapon_id(active_weapon);
    bool is_projectile_class = get_player_class(localplayer) == TF_CLASS_SOLDIER;
    bool is_projectile_weapon = weapon_id == TF_WEAPON_ROCKETLAUNCHER || weapon_id == TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT;
    
    if (is_projectile_class && is_projectile_weapon)
    {
        projectile_aimbot(localplayer, user_cmd, weapon_id);
    }
    else
    {
        hitscan_aimbot(localplayer, user_cmd);
    }
}
