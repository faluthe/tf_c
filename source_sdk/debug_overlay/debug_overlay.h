#include "../math/vec3.h"

#include <stdbool.h>

void set_debug_overlay_interface(void *debug_overlay_interface);
void add_box_overlay(struct vec3_t *origin, struct vec3_t *min, struct vec3_t *max, struct vec3_t *orientation, int r, int g, int b, int a, float duration);
void add_line_overlay(struct vec3_t *origin, struct vec3_t *dest, int r, int g, int b, bool no_depth_test, float duration);
int screen_position(struct vec3_t *point, struct vec3_t *screen);