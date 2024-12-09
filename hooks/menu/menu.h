#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "../../libs/nuklear/nuklear.h"

#include <SDL2/SDL.h>

void swap_window_hook(void *window);
int poll_event_hook(SDL_Event *event);

void watermark(struct nk_context *ctx);
void draw_menu(struct nk_context *ctx);