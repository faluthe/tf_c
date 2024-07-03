#include "../interfaces/interfaces.h"
#include "../utils/utils.h"
#include "create_move/create_move.h"
#include "paint_traverse/paint_traverse.h"
#include "hooks.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static const int create_move_index = 22;
static const int paint_traverse_index = 42;

bool init_hooks()
{
    create_move_original = client_mode_vtable[create_move_index];
    log_msg("CreateMove found at %p\n", create_move_original);

    if (!write_to_table(client_mode_vtable, create_move_index, create_move_hook))
    {
        log_msg("Failed to hook CreateMove\n");
        return false;
    }

    paint_traverse_original = vgui_panel_vtable[paint_traverse_index];
    log_msg("PaintTraverse found at %p\n", paint_traverse_original);

    if (!write_to_table(vgui_panel_vtable, paint_traverse_index, paint_traverse_hook))
    {
        log_msg("Failed to hook PaintTraverse\n");
        return false;
    }

    log_msg("We should be hooked...\n");
    log_msg("Original: %p, Hook: %p, [%d]: %p\n", create_move_original, create_move_hook, create_move_index, client_mode_vtable[create_move_index]);
    log_msg("Original: %p, Hook: %p, [%d]: %p\n", paint_traverse_original, paint_traverse_hook, paint_traverse_index, vgui_panel_vtable[paint_traverse_index]);

    return true;
}

void restore_hooks()
{
    if (!client_mode_vtable)
    {
        log_msg("ClientMode vtable not found, no restore necessary\n");
        return;
    }

    if (!vgui_panel_vtable)
    {
        log_msg("VGuiPanel vtable not found, no restore necessary\n");
        return;
    }
    
    if (!write_to_table(client_mode_vtable, create_move_index, create_move_original))
    {
        log_msg("Failed to restore CreateMove\n");
    }

    if (!write_to_table(vgui_panel_vtable, paint_traverse_index, paint_traverse_original))
    {
        log_msg("Failed to restore PaintTraverse\n");
    }
}