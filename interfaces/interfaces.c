#include "../utils/utils.h"
#include "interfaces.h"

#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef void *(*CreateInterfaceFn)(const char *, __int32_t *);

// Globals
void *client_interface;
void *engine_interface;
void *entity_list_interface;

// Locals
const char *client_interface_version = "VClient017";
const char *engine_interface_version = "VEngineClient014";
const char *entity_list_interface_version = "VClientEntityList003";

// Arch consts
const char arch_tfbin_path[256] = "/home/pat/.local/share/Steam/steamapps/common/Team Fortress 2/tf/bin/linux64/";
const char arch_bin_path[256] = "/home/pat/.local/share/Steam/steamapps/common/Team Fortress 2/bin/linux64/";

// Debian consts
const char ubuntu_tfbin_path[256] = "/home/pat/.steam/debian-installation/steamapps/common/Team Fortress 2/tf/bin/linux64/";
const char ubuntu_bin_path[256] = "/home/pat/.steam/debian-installation/steamapps/common/Team Fortress 2/bin/linux64/";

CreateInterfaceFn get_factory(const char base_path[256], char *lib_name)
{
    // TDB: verify path exists
    char path[256];
    strcpy(path, base_path);
    strcat(path, lib_name);

    void *lib_handle = dlopen(path, RTLD_NOLOAD | RTLD_NOW);
    if (!lib_handle)
    {
        log_msg("Failed to load %s\n", lib_name);
        return NULL;
    }

    log_msg("client.so loaded at %p\n", lib_handle);

    CreateInterfaceFn create_interface = dlsym(lib_handle, "CreateInterface");
    if (!create_interface)
    {
        log_msg("Failed to get CreateInterface\n");
        return NULL;
    }

    log_msg("%s factory found at %p\n", lib_name, create_interface);

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
    CreateInterfaceFn client_factory = get_factory(arch_tfbin_path, "client.so");
    CreateInterfaceFn engine_factory = get_factory(arch_bin_path, "engine.so");

    if (!client_factory || !engine_factory)
    {
        log_msg("Failed to get factories\n");
        return false;
    }

    client_interface = get_interface(client_factory, client_interface_version);
    engine_interface = get_interface(engine_factory, engine_interface_version);
    entity_list_interface = get_interface(client_factory, entity_list_interface_version);

    if (!client_interface || !engine_interface || !entity_list_interface)
    {
        // Message in get_interface
        return false;
    }

    return true;
}