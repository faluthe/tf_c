#include "../../../source_sdk/math/vec3.h"

#include <stdbool.h>

struct bounding_box
{
    float left, top, right, bottom;
};

struct bounding_box get_ent_2d_box(void *entity);
bool is_good_box(struct bounding_box box);