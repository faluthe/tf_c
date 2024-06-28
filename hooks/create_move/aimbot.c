
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/global_vars/global_vars.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/user_cmd.h"
#include "../../utils/utils.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

void aim_at_best_target(void *localplayer, struct user_cmd *user_cmd)
{
    float smallest_angle = 999999.0f;
    float target_yaw_angle = 0.0f;
    float target_pitch_angle = 0.0f;
    int target_index = 0;

    for (int ent_index = 1; ent_index <= get_max_clients(); ent_index++)
    {
        void *entity = get_client_entity(ent_index);

        if (entity == NULL || entity == localplayer)
        {
            continue;
        }

        __int32_t ent_health = get_ent_health(entity);

        // TBD: Check if entity is alive
        if (ent_health <= 1 || ent_health > 450 || get_ent_team(entity) == get_ent_team(localplayer))
        {
            continue;
        }

        static int bone_to_aim_at = 6;

        struct vec3_t ent_pos = get_bone_pos(entity, bone_to_aim_at);
        struct vec3_t ent_vel = get_ent_velocity(entity);
        struct vec3_t local_pos = get_ent_eye_pos(localplayer);
        struct vec3_t local_vel = get_ent_velocity(localplayer);
        // Not accurate, but good enough for now
        float interval_per_tick = get_global_vars_interval_per_tick();

        local_pos.x += (local_vel.x * interval_per_tick);
        local_pos.y += (local_vel.y * interval_per_tick);
        local_pos.z += (local_vel.z * interval_per_tick);

        // Add hammer units per tick to entity position
        ent_pos.x += (ent_vel.x * interval_per_tick);
        ent_pos.y += (ent_vel.y * interval_per_tick);
        ent_pos.z += (ent_vel.z * interval_per_tick);
        
        float x_diff = ent_pos.x - local_pos.x;
        float y_diff = ent_pos.y - local_pos.y;
        float z_diff = ent_pos.z - local_pos.z;
        float hypo = sqrt(x_diff * x_diff + y_diff * y_diff);
        float yaw_angle = atan2(y_diff, x_diff) * 180 / M_PI;
        float pitch_angle = atan2(z_diff, hypo) * 180 / M_PI;

        // viewangle_z is roll
        float current_pitch = user_cmd->viewangles.x;
        float current_yaw = user_cmd->viewangles.y;

        float delta_pitch = abs(pitch_angle - current_pitch);
        if (delta_pitch > 180)
        {
            delta_pitch = 360 - delta_pitch;
        }
        float delta_yaw = abs(yaw_angle - current_yaw);
        if (delta_yaw > 180)
        {
            delta_yaw = 360 - delta_yaw;
        }
        float angle_to_ent = delta_yaw + delta_pitch;

        if (angle_to_ent < smallest_angle)
        {          
            smallest_angle = angle_to_ent;
            target_yaw_angle = yaw_angle;
            target_pitch_angle = pitch_angle;
            target_index = ent_index;
        }
    }

    if (target_index != 0)
    {
        user_cmd->viewangles.x = -target_pitch_angle;
        user_cmd->viewangles.y = target_yaw_angle;
    }
}