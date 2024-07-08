#include <stdatomic.h>
#include <stdbool.h>

bool init_hooks();
void restore_hooks();

// Shared state between hooks
extern atomic_int g_target_x;
extern atomic_int g_target_y;
extern atomic_int g_target_index;