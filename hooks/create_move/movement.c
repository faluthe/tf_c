#include "../../config/config.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/user_cmd.h"
#include "../../utils/math/math_utils.h"
#include "../../utils/utils.h"
#include "create_move.h"
#include <math.h>
#include <stdbool.h>

void bunny_hop(void *localplayer, struct user_cmd *user_cmd)
{
    /* scouts double jump only works sometimes, very inconsistent
     * i haven't figured out why (FROM TESTING: i belive the on ground flag sometimes is not getting set correctly)
     * in any case, i'll just disable bhop on scout until i figure it out */

    if ( !config.misc.bunny_hop || get_player_class(localplayer) == TF_CLASS_SCOUT )
    {
        return;
    }

    static bool was_jumping = false;
    bool on_ground = (get_ent_flags(localplayer) & FL_ONGROUND);

    if ( user_cmd->buttons & IN_JUMP )
    {
        if ( !was_jumping && !on_ground )
        {
            user_cmd->buttons &= ~IN_JUMP;
        }
        else if ( was_jumping )
        {
            was_jumping = false;
        }
    }
    else if ( !was_jumping )
    {
        was_jumping = true;
    }
}

void autostrafe(void *localplayer, struct user_cmd *user_cmd)
{
    // TBD: figure out how to implement directional / rage autostrafe
    // i don't think this kind of autostrafe does anything

    if ( !config.misc.legit_autostrafe || get_player_class(localplayer) == TF_CLASS_SCOUT )
    {
        return;
    }

    const float cl_sidespeed = 450.0f; // assume default value

    const bool on_ground = get_ent_flags(localplayer) & FL_ONGROUND;
    const bool on_water = get_ent_flags(localplayer) & FL_INWATER;

    if ( on_ground || on_water )
    {
        return;
    }

    if ( user_cmd->mouse_dx < 0 )
    {
        user_cmd->sidemove = -cl_sidespeed;
    }
    else if ( user_cmd->mouse_dx > 0 )
    {
        user_cmd->sidemove = cl_sidespeed;
    }
}

void rage_autostrafe(void *localplayer, struct user_cmd *user_cmd)
{
    // inspired from:
    // https://github.com/degeneratehyperbola/NEPS

    if ( !config.misc.rage_autostrafe || get_player_class(localplayer) == TF_CLASS_SCOUT )
    {
        return;
    }

    const bool on_ground = get_ent_flags(localplayer) & FL_ONGROUND;
    const bool on_water = get_ent_flags(localplayer) & FL_INWATER;

    if ( on_ground || on_water )
    {
        return;
    }

    const struct vec3_t velocity = get_ent_velocity(localplayer);
    const float speed = vec_lenght2d(velocity);

    if ( speed < 2 )
    {
        return;
    }

    // assume default vallues
    const float sv_airaccelerate = 10.0f;
    const float sv_maxspeed = 320.0f;
    const float cl_forwardspeed = 450.0f;
    const float cl_sidespeed = 450.0f;

    // this is hardcoded in tf2, unless a sourcemod that changes movement touched it
    const float wishspeed = 30.0f;

    float terminal = wishspeed / sv_airaccelerate / sv_maxspeed * 100.0f / speed;

    if ( terminal < -1 || terminal > 1 )
    {
        return;
    }

    float bdelta = acosf(terminal);

    struct vec3_t viewangles = get_ent_angles(localplayer);

    float yaw = deg_2_rad(viewangles.y);
    float velocity_direction = atan2f(velocity.y, velocity.x) - yaw;
    float target_angle = atan2f(-user_cmd->sidemove, user_cmd->forwardmove);
    float delta = delta_rad_angle2f(velocity_direction, target_angle);

    float move_direction = delta < 0 ? velocity_direction + bdelta : velocity_direction - bdelta;

    user_cmd->forwardmove = cosf(move_direction) * cl_forwardspeed;
    user_cmd->sidemove = -sinf(move_direction) * cl_sidespeed;
}
