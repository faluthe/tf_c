#include "../utils/utils.h"
#include "interfaces.h"

#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef void *(*CreateInterfaceFn)(char *, __int32_t *);

CreateInterfaceFn get_factory(char *lib_name)
{
    char path[256] = "~/.steam/debian-installation/steamapps/common/Team Fortress 2/tf/bin/linux64/";
    strcat(path, lib_name);

    void *lib_handle = dlopen(path, RTLD_NOLOAD | RTLD_NOW);
    if (!lib_handle)
    {
        log_msg("Failed to load %s\n", lib_name);
        return NULL;
    }

    log_msg("client.so loaded at %p\n", lib_handle);

    __int64_t (*create_interface)(char *, __int32_t *) = dlsym(lib_handle, "CreateInterface");
    if (!create_interface)
    {
        log_msg("Failed to get CreateInterface\n");
        return NULL;
    }

    log_msg("%s factory found at %p\n", lib_name, create_interface);

    return create_interface;
}

bool init_interfaces()
{
    CreateInterfaceFn client_factory = get_factory("client.so");
    if (!client_factory)
    {
        return false;
    }

    client_interface = client_factory("VClient017", NULL);
    if (!client_interface)
    {
        log_msg("Failed to get VClient017 interface\n");
        return false;
    }

    log_msg("VClient017 interface found at %p\n", client_interface);

    return true;
}