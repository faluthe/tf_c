#include "../utils/utils.h"
#include "interfaces.h"

#include "../source_sdk/debug_overlay/debug_overlay.h"
#include "../source_sdk/engine_client/engine_client.h"
#include "../source_sdk/engine_trace/engine_trace.h"
#include "../source_sdk/entity_list/entity_list.h"
#include "../source_sdk/global_vars/global_vars.h"
#include "../source_sdk/panel/panel.h"
#include "../source_sdk/surface/surface.h"

#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef void *(*CreateInterfaceFn)(const char *, __int32_t *);

// Globals
void **client_vtable = NULL;
void **client_mode_vtable = NULL;
void **vgui_panel_vtable = NULL;

// Locals
static const char *client_version = "VClient017";
static const char *debug_overlay_version = "VDebugOverlay003";
static const char *engine_client_version = "VEngineClient014";
static const char *engine_trace_version = "EngineTraceClient003";
static const char *entity_list_version = "VClientEntityList003";
static const char *surface_version = "VGUI_Surface030";
static const char *vgui_panel_version = "VGUI_Panel009";

static const char *tfbin_path = "./tf/bin/linux64/";
static const char *bin_path = "./bin/linux64/";

CreateInterfaceFn get_factory(const char *base_path, char *lib_name)
{
    char lib_path[256];
    strcpy(lib_path, base_path);
    strcat(lib_path, lib_name);

    void *lib_handle = dlopen(lib_path, RTLD_NOLOAD | RTLD_NOW);
    if (!lib_handle)
    {
        log_msg("Failed to load %s\n", lib_path);
        return NULL;
    }

    log_msg("%s loaded at %p\n", lib_path, lib_handle);

    CreateInterfaceFn create_interface = dlsym(lib_handle, "CreateInterface");
    if (!create_interface)
    {
        log_msg("Failed to get CreateInterface\n");
        return NULL;
    }

    log_msg("%s factory found at %p\n", lib_path, create_interface);

    return create_interface;
}

void *get_interface(CreateInterfaceFn factory, const char *version)
{
    void *interface = factory(version, NULL);
    
    if (!interface)
    {
        log_msg("Failed to get %s interface\n", version);
        return NULL;
    }

    log_msg("%s interface found at %p\n", version, interface);
    
    return interface;
}

bool init_interfaces()
{
    CreateInterfaceFn client_factory = get_factory(tfbin_path, "client.so");
    CreateInterfaceFn engine_factory = get_factory(bin_path, "engine.so");
    CreateInterfaceFn surface_factory = get_factory(bin_path, "vguimatsurface.so");
    CreateInterfaceFn vgui_factory = get_factory(bin_path, "vgui2.so");

    if (!client_factory || !engine_factory || !surface_factory || !vgui_factory)
    {
        log_msg("Failed to get all factories\n");
        return false;
    }

    void *client_interface = get_interface(client_factory, client_version);
    void *debug_overlay_interface = get_interface(engine_factory, debug_overlay_version);
    void *engine_client_interface = get_interface(engine_factory, engine_client_version);
    void *engine_trace_interface = get_interface(engine_factory, engine_trace_version);
    void *entity_list_interface = get_interface(client_factory, entity_list_version);
    void *surface_interface = get_interface(surface_factory, surface_version);
    void *vgui_panel_interface = get_interface(vgui_factory, vgui_panel_version);

    if (!client_interface || !engine_client_interface || !entity_list_interface || !surface_interface || !vgui_panel_interface || !debug_overlay_interface || !engine_trace_interface)
    {
        log_msg("Failed to get all interfaces\n");
        return false;
    }

    set_engine_client_interface(engine_client_interface);
    set_entity_list_interface(entity_list_interface);
    set_surface_interface(surface_interface);
    set_panel_interface(vgui_panel_interface);
    set_debug_overlay_interface(debug_overlay_interface);
    set_engine_trace_interface(engine_trace_interface);

    vgui_panel_vtable = *(void ***)vgui_panel_interface;

    /*
     * https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/game/client/cdll_client_int.cpp#L1255
     * CHLClient::HudProcessInput is just a call to g_pClientMode->ProcessInput. Globals are stored as effective addresses.
     * Effective addresses are 4 byte offsets, offset from the instruction pointer (address of next instruction).
     * Manually calculate the effective address of g_pClientMode and dereference it to get the interface.
    */
    client_vtable = *(void ***)client_interface;
    void *hud_process_input_addr = client_vtable[10];
    __uint32_t client_mode_eaddr = *(__uint32_t *)((__uint64_t)(hud_process_input_addr) + 0x3);
    void *client_mode_next_instruction = (void *)((__uint64_t)(hud_process_input_addr) + 0x7);
    void *client_mode_interface = *(void **)((__uint64_t)(client_mode_next_instruction) + client_mode_eaddr);

    // TBD remove unnecessary casts
    void *hud_update = client_vtable[11];
    __uint32_t global_vars_eaddr = *(__uint32_t *)((__uint64_t)(hud_update) + 0x16);
    void *global_vars_next_instruction = (void *)((__uint64_t)(hud_update) + 0x1A);
    set_global_vars_ptr(*(void **)((__uint64_t)(global_vars_next_instruction) + global_vars_eaddr));
    
    client_mode_vtable = *(void ***)client_mode_interface;

    return true;
}
