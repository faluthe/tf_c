#include "../interfaces/interfaces.h"
#include "../source_sdk/engine_client/engine_client.h"
#include "../source_sdk/entity_list/entity_list.h"
#include "utils.h"

#include <stdint.h>

void *get_localplayer()
{
    __int32_t index = get_localplayer_index(engine_interface);
    log_msg("Localplayer index: %d\n", index);

    return get_client_entity(entity_list_interface, index);
}