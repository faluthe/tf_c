#include "../interfaces/interfaces.h"
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
    static unsigned long tick_count = 0ll;

    tick_count++;

    if (!hooked)
    {
        log_msg("Hi from hook function! time: %f ptr: %p og: %p\n", sample_time, user_cmd, create_move_original);
        hooked = true;
    }

    if (tick_count % 100 == 0)
    {
        log_msg("Hi again from hook function! time: %f ptr: %p og: %p\n", sample_time, user_cmd, create_move_original);
    }

    return create_move_original(this, sample_time, user_cmd);
}

bool init_hooks()
{
    // Deref vptr
    void **client_vtable = *(void ***)client_interface;
    log_msg("Client vfunc table found at %p\n", client_vtable);

    void *hud_process_input_addr = client_vtable[10];
    log_msg("hud_process_input found at %p\n", hud_process_input_addr);

    __uint32_t *client_mode_eaddr = (__uint32_t *)((__uint64_t)(hud_process_input_addr) + 3);
    log_msg("Ptr to ClientMode effective address (offset): %p\n", client_mode_eaddr);
    log_msg("ClientMode effective address (offset): %p\n", *client_mode_eaddr);

    void *client_mode_next_instruction = (void *)((__uint64_t)(hud_process_input_addr) + 7);
    log_msg("Next instruction after hud_process_input: %p\n", client_mode_next_instruction);

    void **client_mode_ptr = (void **)((__uint64_t)(client_mode_next_instruction) + *client_mode_eaddr);
    log_msg("Ptr to ClientMode: %p\n", client_mode_ptr);

    void *client_mode_interface = *client_mode_ptr;
    log_msg("ClientMode: %p\n", client_mode_interface);

    client_mode_vtable = *(void ***)client_mode_interface;
    log_msg("ClientMode vfunc table found at %p\n", client_mode_vtable);

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
    if (write_to_table(client_mode_vtable, 22, create_move_original))
    {
        log_msg("Failed to restore CreateMove\n");
    }
}