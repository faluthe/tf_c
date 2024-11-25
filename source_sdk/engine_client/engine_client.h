// See https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/engine/cdll_engine_int.cpp#L334

#include "../entity/entity.h"

#include <stdint.h>

void set_engine_client_interface(void *engine_client_interface);
__int32_t get_localplayer_index();
void *get_net_channel_info();
__int32_t get_max_clients();
void get_screen_size(int *width, int *height);
__int8_t is_in_game();
__int8_t get_player_info(int ent_index, struct player_info_t *pinfo);