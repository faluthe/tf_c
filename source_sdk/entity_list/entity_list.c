#include "../../utils/utils.h"
#include "entity_list.h"

void *get_client_entity(void *interface, __int32_t index)
{
    void **vtable = *(void ***)interface;
    void *(*func)(void *, __int32_t) = vtable[3];

    log_msg("CClientEntityList->GetClientEntity found at %p\n", func);

    return func(interface, index);
}