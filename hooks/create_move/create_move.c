#include "../../config/config.h"
#include "../../utils/utils.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/user_cmd.h"
#include "../paint_traverse/paint_traverse.h"
#include "create_move.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>




#include "../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../source_sdk/client_prediction/client_prediction.h"
#include "../../source_sdk/game_movement/game_movement.h"
#include "../../source_sdk/global_vars/global_vars.h"
#include "../../source_sdk/md5/md5.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/user_cmd.h"
#include "../../utils/math/math_utils.h"

#include <stdint.h>
#include <string.h>

// extern
__int64_t (*paint_traverse_original)(void *, void *, __int8_t, __int8_t) = NULL;


struct vec3_t velocity_to_angles(struct vec3_t direction)
{
    float yaw, pitch;

    if (direction.y == 0 && direction.x == 0)
    {
        yaw = 0.0f;

        if (direction.z > 0.0f)
        {
            pitch = 270.0f;
        }
        else
        {
            pitch = 90.0f;
        }
    }
    else
    {
        float magnitude = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        yaw = atan2f(direction.y, direction.x) * 180 / M_PI;
        pitch = atan2f(-direction.z, magnitude) * 180 / M_PI;

        if (yaw < 0)
        {
            yaw += 360;
        }

        if (pitch < 0)
        {
            pitch += 360;
        }
    }

    return (struct vec3_t){pitch, yaw, 0.0f};
}

int time_to_ticks(float time)
{
    return (int)(0.5f + time / get_global_vars_interval_per_tick());
}

float ticks_to_time(int ticks)
{
    return get_global_vars_interval_per_tick() * ticks;
}

static struct user_cmd dummy_cmd;

void draw_predicted_movement()
{
    void *localplayer = get_localplayer();

    if (!localplayer || get_ent_lifestate(localplayer) != 1)
    {
        return;
    }

    memset(&dummy_cmd, 0, sizeof(struct user_cmd));
    set_current_command(localplayer, (void *)&dummy_cmd);

    // STORE PLAYER DATA
    log_msg("setting up player data\n");
    struct vec3_t old_origin = get_ent_origin(localplayer);
    struct vec3_t old_velocity = get_ent_velocity(localplayer);
    struct vec3_t old_base_velocity = get_base_velocity(localplayer);
    struct vec3_t old_view_offset = get_view_offset(localplayer);
    __int32_t old_ground_entity = get_ground_entity_handle(localplayer);
    __int32_t old_flags = get_ent_flags(localplayer);
    float old_duck_time = get_duck_time(localplayer);
    float old_duck_jump_time = get_duck_jump_time(localplayer);
    __uint8_t old_ducked = get_b_ducked(localplayer);
    __uint8_t old_ducking = get_b_ducking(localplayer);
    __uint8_t old_in_duck_jump = get_b_in_duck_jump(localplayer);
    float old_model_scale = get_model_scale(localplayer);

    log_msg("Old Origin: (%f, %f, %f)\n", old_origin.x, old_origin.y, old_origin.z);
    log_msg("Old Velocity: (%f, %f, %f)\n", old_velocity.x, old_velocity.y, old_velocity.z);
    log_msg("Old Base Velocity: (%f, %f, %f)\n", old_base_velocity.x, old_base_velocity.y, old_base_velocity.z);
    log_msg("Old View Offset: (%f, %f, %f)\n", old_view_offset.x, old_view_offset.y, old_view_offset.z);
    log_msg("Old Ground Entity: %d\n", old_ground_entity);
    log_msg("Old Flags: %d\n", old_flags);
    log_msg("Old Duck Time: %f\n", old_duck_time);
    log_msg("Old Duck Jump Time: %f\n", old_duck_jump_time);
    log_msg("Old Ducked: %d\n", old_ducked);
    log_msg("Old Ducking: %d\n", old_ducking);
    log_msg("Old In Duck Jump: %d\n", old_in_duck_jump);
    log_msg("Old Model Scale: %f\n", old_model_scale);

    log_msg("setting prediction shit\n");
    __uint8_t old_in_prediction = get_in_prediction();
    __uint8_t old_first_time_predicted = get_first_time_predicted();
    float old_frame_time = get_global_vars_frametime();

    if (player_is_ducking(localplayer))
    {
        set_ent_flags(localplayer, get_ent_flags(localplayer) & ~2);
        set_b_ducked(localplayer, true);
        set_duck_time(localplayer, 0.0f);
        set_duck_jump_time(localplayer, 0.0f);
        set_b_ducking(localplayer, false);
        set_b_in_duck_jump(localplayer, false);
    }

    // if player != localplayer
    // set_ground_entity_handle(localplayer, 0);

    log_msg("Setting model scale\n");
    set_model_scale(localplayer, get_model_scale(localplayer) - 0.03125f);

    // On ground
    if ((get_ent_flags(localplayer) & 1) == 0)
    {
        log_msg("On ground\n");
        struct vec3_t origin = get_ent_origin(localplayer);
        set_ent_origin(localplayer, origin.x, origin.y, origin.z + 0.03125f);
    }

    struct vec3_t vel = get_ent_velocity(localplayer);
    log_msg("Setting velocity\n");
    if (fabsf(vel.x) < 0.01f)
    {
        set_ent_velocity(localplayer, (struct vec3_t){0.015f, vel.y, vel.z});
    }

    if (fabsf(vel.y) < 0.01f)
    {
        set_ent_velocity(localplayer, (struct vec3_t){vel.x, 0.015f, vel.z});
    }

    // Set up move data
    log_msg("Setting up move data\n");
    struct move_data move_data;
    memset(&move_data, 0, sizeof(struct move_data));
    move_data.first_run_of_functions = false;
    move_data.game_code_moved_player = false;
    move_data.player_handle = get_eh_handle(localplayer);
    move_data.velocity = get_ent_velocity(localplayer);
    move_data.abs_origin = get_ent_origin(localplayer);

    log_msg("Setting up angles\n");
    set_ent_angles(localplayer, (struct vec3_t){0.0f, velocity_to_angles(move_data.velocity).y, 0.0f});

    struct vec3_t forward, right;
    angle_vectors(move_data.angles, &forward, &right, NULL);

    move_data.forward_move = (move_data.velocity.y - right.y / right.x * move_data.velocity.x) / (forward.y - right.y / right.x * forward.x);
    move_data.side_move = (move_data.velocity.x - forward.x * move_data.forward_move) / right.x;

    // Run ticks
    for (int n = 0; n < time_to_ticks(1.0f); n++)
    {
        set_in_prediction(true);
        set_first_time_predicted(false);
        set_global_vars_frametime(get_engine_paused() ? 0.0f : get_global_vars_interval_per_tick());

        process_movement(localplayer, &move_data);

        log_msg("Tick (%d) Move data abs origin: (%f, %f, %f)\n", n, move_data.abs_origin.x, move_data.abs_origin.y, move_data.abs_origin.z);
    }
     // I think draw here with move data.abs_origin
    struct vec3_t origin_2d;
    if (screen_position(&move_data.abs_origin, &origin_2d) == 0)
    {
        add_to_render_queue(L"X", origin_2d.x, origin_2d.y, (struct vec3_t){255.0f, 0.0f, 0.0f}, 0.0f);
    }

    log_msg("Restoring shit\n");
    set_current_command(localplayer, NULL);

    // TBD: RESTORE PLAYER DATA
    set_ent_origin(localplayer, old_origin.x, old_origin.y, old_origin.z);
    set_ent_velocity(localplayer, old_velocity);
    set_base_velocity(localplayer, old_base_velocity);
    set_view_offset(localplayer, old_view_offset);
    set_ground_entity_handle(localplayer, old_ground_entity);
    set_ent_flags(localplayer, old_flags);
    set_duck_time(localplayer, old_duck_time);
    set_duck_jump_time(localplayer, old_duck_jump_time);
    set_b_ducked(localplayer, old_ducked);
    set_b_ducking(localplayer, old_ducking);
    set_b_in_duck_jump(localplayer, old_in_duck_jump);
    set_model_scale(localplayer, old_model_scale);

    set_in_prediction(old_in_prediction);
    set_first_time_predicted(old_first_time_predicted);
    set_global_vars_frametime(old_frame_time);
}

int get_tick_base_prediction(struct user_cmd *cmd, void *localplayer)
{
    static int tick = 0;
    static struct user_cmd *last_cmd = NULL;

    if (cmd)
    {
        if (!last_cmd || last_cmd->has_been_predicted)
        {
            tick = get_tick_base(localplayer);
        }
        else
        {
            tick++;
        }
    }

    return tick;
}

void start_prediction(struct user_cmd *user_cmd)
{
    void *localplayer = get_localplayer();
    
    if (!localplayer || get_ent_lifestate(localplayer) != 1)
    {
        return;
    }

    // Basically going to rebuild CPrediction::RunCommand here


    // StartCommand( player, ucmd )
    
    // ResetInstanceCounters -- dis gonna be a bitch to find unless vprof exports the string
    set_current_command(localplayer, user_cmd);
    // SetPredictionRadomSeed
    // int *random_seed = sig scan for the old seed (?) maybe just use cmd->random_seed?
    // int old_random_seed = *random_seed (?) might not need this 
    // *random_seed = md5_pseudo_random(cmd->cmd_number & __INT_MAX__ (?))
    // Maybe SetPredictionPlayer?

    float old_curtime = get_global_vars_curtime();
    float old_frametime = get_global_vars_frametime();
    int old_tickcount = get_global_vars_tickcount();

    int old_tickbase = get_tick_base(localplayer);
    bool old_is_first_prediction = get_first_time_predicted();
    bool old_in_prediction = get_in_prediction();

    set_global_vars_curtime(ticks_to_time(get_tick_base_prediction(user_cmd, localplayer)));
    set_global_vars_frametime(get_engine_paused() ? 0.0f : get_global_vars_interval_per_tick());
    set_global_vars_tickcount(get_tick_base_prediction(user_cmd, localplayer));

    set_first_time_predicted(false);
    set_in_prediction(true);

    // gamemovement->StartTrackPredictionErrors( player )
    // set_player_button_state(localplayer, user_cmd->buttons);
    // set_local_view_angles(localplayer, user_cmd->viewangles);

    // RunPreThink( player )
    // if (!player->PhysicsRunThink(0))
    // {
    //     player->PreThink(); -- virtual
    // }

    // RunThink( player, TICK_INTERVAL )
    // int thinktick = get_player_next_tick_think(localplayer);
    // if (thinktick > 0 && thinktick <= get_global_vars_tickcount())
    // {
    //     player->SetNextThink(TICK_NEVER_THINK (-1), NULL);
    //     player->Think();
    // }

    // movehelper->sethost(localplayer)

    // iprediction->SetupMove( player, ucmd, movehelper, &movedata )
    // gamemovement->ProcessMovement( player, &movedata )
    // iprediction->FinishMove( player, ucmd, &movedata )

    // player->PostThink()
    // set_tickbase(localplayer, old_tickbase);

    set_in_prediction(old_in_prediction);
    set_first_time_predicted(old_is_first_prediction);
}

void end_prediction()
{
    void *localplayer = get_localplayer();
    
    if (!localplayer || get_ent_lifestate(localplayer) != 1)
    {
        return;
    }

    // movehelper->sethost(NULL);
    // set_global_vars_curtime(old_curtime);
    // set_global_vars_frametime(old_frametime);
    // set_global_vars_tickcount(old_tickcount);
    set_current_command(localplayer, NULL);

    // *random_seed = -1;
}



static bool silent_aim = true;

// extern
__int64_t (*create_move_original)(void *, float, void *) = NULL;

__int64_t create_move_hook(void *this, float sample_time, struct user_cmd *user_cmd)
{
    __int64_t rc = create_move_original(this, sample_time, user_cmd);
    static bool hooked = false;

    if (!hooked)
    {
        log_msg("CreateMove hooked!\n");
        hooked = true;
    }

    if (!is_in_game())
    {
        return rc;
    }

    void *localplayer = get_localplayer();
    
    if (!localplayer)
    {
        log_msg("localplayer is NULL\n");
        return rc;
    }
    
    if (user_cmd->tick_count > 1)
    {
        clear_render_queue();
        aimbot(localplayer, user_cmd);
        draw_predicted_movement();
    }

    // If player is not on ground unset jump button flag (breaks scout double jump)
    if ((get_ent_flags(localplayer) & 1) == 0 && config.bunny_hop)
    {
        user_cmd->buttons &= ~2;
    }

    if (silent_aim)
    {
        return false;
    } 
    else
    {
        return rc;
    }
}
