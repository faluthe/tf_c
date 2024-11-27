#include "../math/vec3.h"

#include <stdbool.h>

extern void (*estimate_abs_velocity)(void *, struct vec3_t *);

bool init_estimate_abs_velocity();