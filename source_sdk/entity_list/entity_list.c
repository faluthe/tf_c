#include "../../utils/utils.h"
#include "entity_list.h"

#include <stddef.h>

static void *interface = NULL;

void set_entity_list_interface(void *entity_list_interface)
{
    interface = entity_list_interface;
}

void *get_client_entity(__int32_t index)
{
    void **vtable = *(void ***)interface;

    void *(*func)(void *, __int32_t) = vtable[3];
    
    return func(interface, index);
}