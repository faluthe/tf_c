#include "../interfaces/interfaces.h"
#include "../utils/utils.h"
#include "create_move/create_move.h"
#include "hooks.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool init_hooks()
{
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