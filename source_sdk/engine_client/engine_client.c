#include "../../utils/utils.h"
#include "engine_client.h"

__int32_t get_localplayer_index(void *interface)
{
    void **vtable = *(void ***)interface;
    __int32_t (*func)(void *) = vtable[12];

    log_msg("CEngineClient->GetLocalPlayer found at %p\n", func);

    return func(interface);
}