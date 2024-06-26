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

__int32_t get_max_clients()
{
    // void **vtable = *(void ***)interface;

    // log_msg("vtable: %p\n", vtable);

    // __int32_t (*func)(void *) = vtable[20];

    return 32;
}