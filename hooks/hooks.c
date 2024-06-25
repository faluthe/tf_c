#include "../interfaces/interfaces.h"
#include "../source_sdk/engine_client/engine_client.h"
#include "../utils/utils.h"
#include "hooks.h"

#include <stdbool.h>
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
    
    if ((flags & 1) == 0)
    {
        *(__int32_t *)((__uint64_t)(user_cmd) + 0x28) &= !2;
    }

    // TBD: Recreate UserCmd struct

    // __int32_t tick_count = *(__int32_t *)((__uint64_t)(user_cmd) + 0x8);
    // if (tick_count == 0)
    // {
    //     return rc;
    // }

    // log_msg("Tick count: %d\n", tick_count);
    // __int32_t buttons = *(__int32_t *)((__uint64_t)(user_cmd) + 0x28);
    // log_msg("Buttons: %d\n", buttons);

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