#include "../utils/utils.h"
#include "interfaces.h"

#include "../source_sdk/engine_client/engine_client.h"
#include "../source_sdk/entity_list/entity_list.h"

#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef void *(*CreateInterfaceFn)(const char *, __int32_t *);

// Globals
void *client_interface;

// Locals
static const char *client_version = "VClient017";
static const char *engine_client_version = "VEngineClient014";
static const char *entity_list_version = "VClientEntityList003";

// Arch consts
static const char arch_tfbin_path[256] = "/home/pat/.local/share/Steam/steamapps/common/Team Fortress 2/tf/bin/linux64/";
static const char arch_bin_path[256] = "/home/pat/.local/share/Steam/steamapps/common/Team Fortress 2/bin/linux64/";

// Debian consts
// static const char ubuntu_tfbin_path[256] = "/home/pat/.steam/debian-installation/steamapps/common/Team Fortress 2/tf/bin/linux64/";
// static const char ubuntu_bin_path[256] = "/home/pat/.steam/debian-installation/steamapps/common/Team Fortress 2/bin/linux64/";

CreateInterfaceFn get_factory(const char base_path[256], char *lib_name)
{
    char path[256];
    strcpy(path, base_path);
    strcat(path, lib_name);

    struct stat buffer;
    if (stat(path, &buffer) != 0)
    {
        log_msg("Failed to find %s\n", lib_name);
        return NULL;
    }

    void *lib_handle = dlopen(path, RTLD_NOLOAD | RTLD_NOW);
    if (!lib_handle)
    {
        log_msg("Failed to load %s\n", lib_name);
        return NULL;
    }

    log_msg("%s loaded at %p\n", lib_name, lib_handle);

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
        log_msg("Failed to get all factories\n");
        return false;
    }

    client_interface = get_interface(client_factory, client_version);
    void *engine_client_interface = get_interface(engine_factory, engine_client_version);
    void *entity_list_interface = get_interface(client_factory, entity_list_version);

    if (!client_interface || !engine_client_interface || !entity_list_interface)
    {
        log_msg("Failed to get all interfaces\n");
        return false;
    }

    set_engine_client_interface(engine_client_interface);
    set_entity_list_interface(entity_list_interface);

    return true;
}