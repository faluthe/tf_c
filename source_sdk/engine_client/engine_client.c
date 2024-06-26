#include "../../utils/utils.h"
#include "engine_client.h"

#include <stddef.h>

static void *interface = NULL;

void set_engine_client_interface(void *engine_client_interface)
{
    interface = engine_client_interface;
}

__int32_t get_localplayer_index()
{
    void **vtable = *(void ***)interface;
    __int32_t (*func)(void *) = vtable[12];

    return func(interface);
}

void *get_net_channel_info()
{
    void **vtable = *(void ***)interface;
    void *(*func)(void *) = vtable[72];

    return func(interface);
}

__int32_t get_max_clients()
{
    // Wrong index, using const for now

    // void **vtable = *(void ***)interface;
    // __int32_t (*func)(void *) = vtable[20];

    return 32;
}