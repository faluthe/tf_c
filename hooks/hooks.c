#include "../interfaces/interfaces.h"
#include "../source_sdk/engine_client/engine_client.h"
#include "../source_sdk/entity/entity.h"
#include "../source_sdk/entity_list/entity_list.h"
#include "../source_sdk/math/vec3.h"
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
    static void *net_chan = NULL;

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
        net_chan = get_net_channel_info();
        log_msg("netchannelinfo: %p\n", net_chan);
        hooked = true;
    }

    __int32_t flags = get_ent_flags(localplayer);
    __int32_t team = get_ent_team(localplayer);
    __int32_t buttons = *(__int32_t *)((__uint64_t)(user_cmd) + 0x28);

    __int32_t tick_count = *(__int32_t *)((__uint64_t)(user_cmd) + 0x8);

    if (tick_count < 1)
    {
        return rc;
    }

    if ((buttons & 8192) == 0)
    {
        void **net_chan_vtable = *(void ***)net_chan;
        float (*get_avg_packets)(void *, int) = net_chan_vtable[14];
        // FLOW_OUTGOING = 0, FLOW_INCOMING = 1
        float avg_packet_in = get_avg_packets(net_chan, 1);

        float smallest_angle = 999999.0f;
        float target_yaw_angle = 0.0f;
        float target_pitch_angle = 0.0f;
        struct vec3_t target_pos = {0.0f, 0.0f, 0.0f};
        int target_index = 0;

        for (int ent_index = 1; ent_index <= get_max_clients(); ent_index++)
        {
            void *entity = get_client_entity(ent_index);

            if (entity == NULL || entity == localplayer)
            {
                continue;
            }

            __int32_t ent_health = get_ent_health(entity);
            __int32_t ent_team = get_ent_team(entity);

            // TBD: Check if entity is alive
            if (ent_health <= 1 || ent_health > 450 || ent_team == team)
            {
                continue;
            }

            struct vec3_t ent_pos = get_ent_eye_pos(entity);
            struct vec3_t ent_vel = get_ent_velocity(entity);
            struct vec3_t local_pos = get_ent_eye_pos(localplayer);
            struct vec3_t local_vel = get_ent_velocity(localplayer);

            // Add hammer units per tick to local position
            local_pos.x += (local_vel.x / avg_packet_in);
            local_pos.y += (local_vel.y / avg_packet_in);
            local_pos.z += (local_vel.z / avg_packet_in);

            // Add hammer units per tick to entity position
            ent_pos.x += (ent_vel.x / avg_packet_in);
            ent_pos.y += (ent_vel.y / avg_packet_in);
            ent_pos.z += (ent_vel.z / avg_packet_in);
            
            float x_diff = ent_pos.x - local_pos.x;
            float y_diff = ent_pos.y - local_pos.y;
            float z_diff = ent_pos.z - local_pos.z;
            float hypo = sqrt(x_diff * x_diff + y_diff * y_diff);
            float yaw_angle = atan2(y_diff, x_diff) * 180 / M_PI;
            float pitch_angle = atan2(z_diff, hypo) * 180 / M_PI;

            // viewangle_x is roll
            float current_pitch = *(float *)((__uint64_t)(user_cmd) + 0x10);
            float current_yaw = *(float *)((__uint64_t)(user_cmd) + 0x14);

            float delta_pitch = pitch_angle - current_pitch;
            float delta_yaw = yaw_angle - current_yaw;
            float angle_to_ent = sqrt(delta_pitch * delta_pitch + delta_yaw * delta_yaw);

            if (angle_to_ent < smallest_angle)
            {          
                smallest_angle = angle_to_ent;
                target_yaw_angle = yaw_angle;
                target_pitch_angle = pitch_angle;
                target_pos = ent_pos;
                target_index = ent_index;
            }
        }

        if (target_index != 0)
        {
            *(float *)((__uint64_t)(user_cmd) + 0x14) = target_yaw_angle;
            // Negative pitch is up
            *(float *)((__uint64_t)(user_cmd) + 0x10) = -target_pitch_angle;
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