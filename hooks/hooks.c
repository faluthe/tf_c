#include "../interfaces/interfaces.h"
#include "../source_sdk/engine_client/engine_client.h"
#include "../source_sdk/entity_list/entity_list.h"
#include "../utils/utils.h"
#include "hooks.h"

#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

__int64_t (*create_move_original)(void *, float, void *) = NULL;
void **client_mode_vtable = NULL;

__int64_t create_move_hook(void *this, float sample_time, void *user_cmd)
{
    static bool hooked = false;

    __int64_t rc = create_move_original(this, sample_time, user_cmd);
    void *localplayer = get_localplayer();

    if (!rc || !localplayer)
    {
        return rc;
    }

    if (!hooked)
    {
        log_msg("Hi from hook function! time: %f ptr: %p og: %p\n", sample_time, user_cmd, create_move_original);
        log_msg("Localplayer: %p\n", localplayer);
        hooked = true;
    }

    __int32_t flags = *(__int32_t *)((__uint64_t)(localplayer) + 0x460);
    __int32_t buttons = *(__int32_t *)((__uint64_t)(user_cmd) + 0x28);

    // TBD: Recreate UserCmd struct

    __int32_t tick_count = *(__int32_t *)((__uint64_t)(user_cmd) + 0x8);
    if (tick_count < 1)
    {
        return rc;
    }

    float lowest_score = 999999.0f;
    float target_xy_angle = 0.0f;
    float target_hz_angle = 0.0f;
    int target_index = 0;

    // 2048 = ads
    if ((buttons & 2048) != 0)
    {
        for (int ent_index = 1; ent_index <= get_max_clients(); ent_index++)
        {
            void *entity = get_client_entity(ent_index);

            if (entity == NULL || entity == localplayer)
            {
                continue;
            }

            __int32_t ent_health = *(__int32_t *)((__uint64_t)(entity) + 0x0D4);

            // TBD: Check if entity is alive
            if (ent_health <= 1 || ent_health > 450)
            {
                continue;
            }

            float target_pos_x = *(float *)((__uint64_t)(entity) + 0x340);
            float target_pos_y = *(float *)((__uint64_t)(entity) + 0x344);
            float target_pos_z = *(float *)((__uint64_t)(entity) + 0x348);

            float position_x = *(float *)((__uint64_t)(localplayer) + 0x340);
            float position_y = *(float *)((__uint64_t)(localplayer) + 0x344);
            float position_z = *(float *)((__uint64_t)(localplayer) + 0x348);

            float x_diff = target_pos_x - position_x;
            float y_diff = target_pos_y - position_y;
            float z_diff = target_pos_z - position_z;
            float hypo = sqrt(x_diff * x_diff + y_diff * y_diff);
            float xy_angle = atan2(y_diff, x_diff) * 180 / M_PI;
            float hz_angle = atan2(z_diff, hypo) * 180 / M_PI;

            // viewangle_x is roll
            float viewangles_y = *(float *)((__uint64_t)(user_cmd) + 0x10);
            float viewangles_z = *(float *)((__uint64_t)(user_cmd) + 0x14);

            float angle_score = sqrt(((xy_angle - viewangles_z) * (xy_angle - viewangles_z)) + ((viewangles_y - hz_angle) * (viewangles_y - hz_angle)));
            // float pos_score = abs(x_diff) + abs(y_diff) + abs(z_diff);
            // float pos_modifier = 0.5f;
            // float ent_score = (pos_score * pos_modifier) + angle_score;

            if (angle_score < lowest_score)
            {
                lowest_score = angle_score;
                target_xy_angle = xy_angle;
                target_hz_angle = hz_angle;
                target_index = ent_index;
            }
        }

        if (target_index != 0)
        {
            log_msg("Target %d score %d\n", target_index, lowest_score);
            *(float *)((__uint64_t)(user_cmd) + 0x14) = target_xy_angle;
            *(float *)((__uint64_t)(user_cmd) + 0x10) = -target_hz_angle;
        }
    }

    if ((flags & 1) == 0)
    {
        *(__int32_t *)((__uint64_t)(user_cmd) + 0x28) &= ~2;
    }

    return rc;
}

bool init_hooks()
{
    /*
     * https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/game/client/cdll_client_int.cpp#L1255
     * CHLClient::HudProcessInput is just a call to g_pClientMode->ProcessInput. Globals are stored as effective addresses.
     * Effective addresses are 4 byte offsets, offset from the instruction pointer (address of next instruction).
     * Manually calculate the effective address of g_pClientMode and dereference it to get the interface.
    */
    void **client_vtable = *(void ***)client_interface;
    void *hud_process_input_addr = client_vtable[10];
    __uint32_t client_mode_eaddr = *(__uint32_t *)((__uint64_t)(hud_process_input_addr) + 3);
    void *client_mode_next_instruction = (void *)((__uint64_t)(hud_process_input_addr) + 7);
    void *client_mode_interface = *(void **)((__uint64_t)(client_mode_next_instruction) + client_mode_eaddr);
    
    client_mode_vtable = *(void ***)client_mode_interface;
    create_move_original = client_mode_vtable[22];
    log_msg("CreateMove found at %p\n", create_move_original);

    if (!write_to_table(client_mode_vtable, 22, create_move_hook))
    {
        log_msg("Failed to hook CreateMove\n");
        return false;
    }

    log_msg("We should be hooked...\n");
    log_msg("Original: %p, Hook: %p, [22]: %p\n", create_move_original, create_move_hook, client_mode_vtable[22]);

    return true;
}

void restore_hooks()
{
    if (!client_mode_vtable)
    {
        log_msg("ClientMode vtable not found, no restore necessary\n");
        return;
    }
    
    if (write_to_table(client_mode_vtable, 22, create_move_original))
    {
        log_msg("Failed to restore CreateMove\n");
    }
}