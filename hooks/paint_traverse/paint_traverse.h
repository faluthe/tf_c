#include <stdint.h>

void paint_traverse_hook(void *this, void *panel, __int8_t force_repaint, __int8_t allow_force);
extern __int64_t (*paint_traverse_original)(void *, void *, __int8_t, __int8_t);
void draw_player_esp();