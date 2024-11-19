#include "../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/entity/weapon_entity.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/global_vars/global_vars.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/net_channel_info/net_channel_info.h"
#include "../../source_sdk/user_cmd.h"
#include "../../utils/math/math_utils.h"
#include "../../utils/utils.h"
#include "../paint_traverse/paint_traverse.h"
#include "../hooks.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Use localplayer's class to determine whether to baim, then use ent's class to determine head/torso bone id (different per class)
int get_best_bone(void *localplayer, void *entity)
{
    bool baim = (get_ent_class(localplayer) != TF_CLASS_SNIPER || get_ent_health(entity) <= 50);

    if (baim)
    {
        return 1;
    }

    __int32_t ent_class = get_ent_class(entity);

    if (ent_class == TF_CLASS_DEMOMAN)
    {
        return 16;
    }
    else if (ent_class == TF_CLASS_ENGINEER)
    {
        return 8;
    }
    else if (ent_class == TF_CLASS_SNIPER || ent_class == TF_CLASS_SOLDIER)
    {
        return 5;
    }
    else
    {
        return 6;
    }
}

float get_predicted_time(void *entity, struct vec3_t ent_distance, int rocket_speed_per_second)
{
    int gravity = 800;

    struct vec3_t ent_velocity = get_ent_velocity(entity);

    // Not on the ground (affected by gravity)
    if ((get_ent_flags(entity) & 1) == 0)
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

bool can_attack(void *active_weapon, void *localplayer)
{
    if (active_weapon == NULL)
    {
        return false;
    }

    float next_attack = get_next_attack(active_weapon);
    int tick_base = get_tick_base(localplayer);
    log_msg("Tick base: %d\n", tick_base);
    float curtime = tick_base * get_global_vars_interval_per_tick();

    if (next_attack <= curtime)
    {
        return true;
    } else
    {
        return false;
    }
}

void aim_at_best_target(void *localplayer, struct user_cmd *user_cmd)
{
    float smallest_angle = __FLT_MAX__;
    int target_ent_index = 0;
    struct vec3_t target_ent_pos;
    struct vec3_t target_view_angle;

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

        struct vec3_t ent_pos = get_bone_pos(entity, get_best_bone(localplayer, entity));
        struct vec3_t local_pos = get_ent_eye_pos(localplayer);
        struct vec3_t ent_distance = get_distance(ent_pos, local_pos);

        // Common side between two right triangles
        float w = sqrt((ent_distance.x * ent_distance.x) + (ent_distance.y * ent_distance.y));

        float pitch_angle = atan2(ent_distance.z, w) * 180 / M_PI;        
        float yaw_angle = atan2(ent_distance.y, ent_distance.x) * 180 / M_PI;

        struct vec3_t new_view_angle = {
            .x = -pitch_angle,
            .y = yaw_angle,
            .z = 0
        };

        float yaw_delta = fabsf(new_view_angle.y - user_cmd->viewangles.y);

        if (yaw_delta < 50.0f && yaw_delta < smallest_angle)
        {
            smallest_angle = yaw_delta;
            target_ent_index = ent_index;
            target_ent_pos = ent_pos;
            target_view_angle = new_view_angle;
        }
    }

    void *target_ent = get_client_entity(target_ent_index);
    if (target_ent == NULL)
    {
        return;
    }

    add_bbox_decorator(L"TARGET", (struct vec3_t){255, 0, 0}, target_ent);

    if ((user_cmd->buttons & 1) != 0 && target_ent_index != 0)
    {
        void *active_weapon = get_client_entity(get_active_weapon(localplayer));
        if (active_weapon == NULL)
        {
            return;
        }
        int weapon_id = get_weapon_id(active_weapon);

        // Projectile prediction
        if (get_ent_class(localplayer) == TF_CLASS_SOLDIER && (weapon_id == TF_WEAPON_ROCKETLAUNCHER || weapon_id == TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT))
        {
            struct vec3_t target_ent_origin = get_ent_origin(target_ent);
            struct vec3_t local_pos = get_ent_eye_pos(localplayer);
            if (target_ent_origin.z < local_pos.z)
            {
                target_ent_pos.z = get_ent_origin(target_ent).z;
            }
        
            int rocket_speed_per_second = weapon_id == TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT ? 1980 : 1100;
            
            float predicted_time = target_ent == NULL ? -1.0f : get_predicted_time(target_ent, get_distance(local_pos, target_ent_pos), rocket_speed_per_second);

            if (predicted_time == -1)
            {
                return;
            }

            void *netchannel_info = get_net_channel_info();
            if (netchannel_info == NULL)
            {
                return;
            }
            // float latency = get_latency(netchannel_info, 0) + get_latency(netchannel_info, 1);
            float latency = 1.0f;
            struct vec3_t target_ent_velocity = get_ent_velocity(target_ent);
            target_ent_velocity.x *= latency;
            target_ent_velocity.y *= latency;
            target_ent_velocity.z *= latency;
            target_ent_pos.x += (target_ent_velocity.x * predicted_time);
            target_ent_pos.y += (target_ent_velocity.y * predicted_time);
            if ((get_ent_flags(target_ent) & 1) == 0)
            {
                target_ent_pos.z += (target_ent_velocity.z * predicted_time) - (0.25f * 800 * (predicted_time * predicted_time));
            }
            else
            {
                target_ent_pos.z += (target_ent_velocity.z * predicted_time);
            }

            struct vec3_t ent_distance = get_distance(target_ent_pos, local_pos);

            // Common side between two right triangles
            float w = sqrt((ent_distance.x * ent_distance.x) + (ent_distance.y * ent_distance.y));

            float pitch_angle = atan2(ent_distance.z, w) * 180 / M_PI;        
            float yaw_angle = atan2(ent_distance.y, ent_distance.x) * 180 / M_PI;

            struct vec3_t new_view_angle = {
                .x = -pitch_angle,
                .y = yaw_angle,
                .z = 0
            };

            struct vec3_t ent2d;
            if (screen_position(&target_ent_pos, &ent2d) != 0)
            {
                return;
            }

            add_to_render_queue(L"ROCKET HERE", (int)ent2d.x, (int)ent2d.y);

            user_cmd->viewangles = new_view_angle;
        }
        else
        {
            if (can_attack(active_weapon, localplayer))
            {
                log_msg("Can attack\n");
            } else
            {
                log_msg("Cannot attack\n");
                user_cmd->buttons &= ~1;
                return;
            }
            
            user_cmd->viewangles = target_view_angle;
        }
    }
}