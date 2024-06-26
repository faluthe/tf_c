#include "../source_sdk/engine_client/engine_client.h"
#include "../source_sdk/entity_list/entity_list.h"
#include "utils.h"

#include <stdint.h>

void *get_localplayer()
{
    __int32_t index = get_localplayer_index();

    return get_client_entity(index);
}