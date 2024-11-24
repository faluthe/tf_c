#include "../../utils/utils.h"
#include "engine_client.h"
#include"../entity/entity.h"

#include <stdint.h>
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
    // __int32_t (*func)(void *) = vtable[21];

    return 32;
}

void get_screen_size(int *width, int *height)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, int *, int *) = vtable[5];

    func(interface, width, height);
}

__int8_t is_in_game()
{
    void **vtable = *(void ***)interface;
    __int8_t (*func)(void *) = vtable[26];

    return func(interface);
}

__int8_t get_player_info(int ent_index, player_info_t *pinfo)
{
    void **vtable = *(void ***)interface;
    __int8_t (*func)(void *, int, player_info_t *) = vtable[8];

    return func(interface, ent_index, pinfo);
}
