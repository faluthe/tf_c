#include "../interfaces/interfaces.h"
#include "../utils/utils.h"
#include "create_move/create_move.h"
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

static void (*swap_window_original)(void *) = NULL;

bool init_sdl_hook(void *lib_handle, const char *func_name, void *hook, void **original)
{
    void *func = dlsym(lib_handle, func_name);

    if (!func)
    {
        log_msg("Failed to get %s\n", func_name);
        return false;
    }

    log_msg("%s found at %p\n", func_name, func);

    /* These functions are a `jmp` and then a SIGNED (?) offset relative to the next instruction.
     * They will jump to the actual SDL function. We replace this with a `jmp` to our hook.
     * We save the original function so we can call it in the hook.
     */
    int32_t *offset = (int32_t *)((uint64_t)func + 2);
    *original = (void *)((uint64_t)func + 6 + *offset);

    log_msg("Original %s jump at %p\n", func_name, *original);

    // Downcasting should be okay here
    int32_t hook_offset = (int32_t)((int64_t)hook - (int64_t)func - 6);

    log_msg("Hook offset: %x\n", hook_offset);

    // The page has execute permissions only
    const long page_size = sysconf(_SC_PAGESIZE);
    void *page = (void *)((uint64_t)func & ~(page_size - 1));

    if (mprotect(page, page_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
    {
        log_msg("mprotect failed to change page protection\n");
        return false;
    }

    *offset = hook_offset;

    if (mprotect(page, page_size, PROT_READ | PROT_EXEC) != 0)
    {
        log_msg("mprotect failed to reset page protection\n");
        return false;
    }

    return true;
}

void swap_window_hook(void *window)
{
    log_msg("SwapWindow called\n");

    swap_window_original(window);
}

bool init_hooks()
{
    void *lib_sdl_handle = dlopen("/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0", RTLD_LAZY);

    if (!lib_sdl_handle)
    {
        log_msg("Failed to load SDL2\n");
        return false;
    }

    log_msg("SDL2 loaded at %p\n", lib_sdl_handle);

    if (!init_sdl_hook(lib_sdl_handle, "SDL_GL_SwapWindow", &swap_window_hook, (void **)&swap_window_original))
    {
        log_msg("Failed to hook SDL_GL_SwapWindow\n");
        return false;
    }

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