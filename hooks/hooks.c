#include "../interfaces/interfaces.h"
#include "../utils/utils.h"
#include "create_move/create_move.h"
#include "menu/menu.h"
#include "paint_traverse/paint_traverse.h"
#include "hooks.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

static const int create_move_index = 22;
static const int paint_traverse_index = 42;

void (*swap_window_original)(void *) = NULL;
int (*poll_event_original)(SDL_Event *) = NULL;

bool init_hooks()
{
    void *lib_sdl_handle = dlopen("/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);

    if (!lib_sdl_handle)
    {
        log_msg("Failed to load SDL2\n");
        return false;
    }

    log_msg("SDL2 loaded at %p\n", lib_sdl_handle);

    if (!init_sdl_hook(lib_sdl_handle, "SDL_PollEvent", &poll_event_hook, (void **)&poll_event_original))
    {
        log_msg("Failed to hook SDL_PollEvent\n");
        return false;
    }

    if (!init_sdl_hook(lib_sdl_handle, "SDL_GL_SwapWindow", &swap_window_hook, (void **)&swap_window_original))
    {
        log_msg("Failed to hook SDL_GL_SwapWindow\n");
        return false;
    }

    dlclose(lib_sdl_handle);

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

    void *lib_sdl_handle = dlopen("/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);

    if (!restore_sdl_hook(lib_sdl_handle, "SDL_GL_SwapWindow", swap_window_original))
    {
        log_msg("Failed to restore SDL_GL_SwapWindow\n");
    }
    
    if (!restore_sdl_hook(lib_sdl_handle, "SDL_PollEvent", poll_event_original))
    {
        log_msg("Failed to restore SDL_PollEvent\n");
    }

    dlclose(lib_sdl_handle);
}