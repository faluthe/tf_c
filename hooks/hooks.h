#include <stdatomic.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

bool init_hooks();
void restore_hooks();

extern void (*swap_window_original)(void *);
extern int (*poll_event_original)(SDL_Event *);