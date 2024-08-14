#include <stdint.h>
#include <wchar.h>

#define RENDER_QUEUE_SIZE 100

void paint_traverse_hook(void *this, void *panel, __int8_t force_repaint, __int8_t allow_force);
extern __int64_t (*paint_traverse_original)(void *, void *, __int8_t, __int8_t);
void draw_player_esp();
void init_render_queue();
void add_to_render_queue(const wchar_t *text, int x, int y);
void draw_render_queue();
void clear_render_queue();
void destroy_render_queue();