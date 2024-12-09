#include "../../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../../source_sdk/entity/entity.h"
#include "../../../source_sdk/math/vec3.h"
#include "../../../source_sdk/surface/surface.h"
#include "esp_util.h"

#include <stdbool.h>

struct bounding_box get_ent_2d_box(void *entity)
{
    struct vec3_t ent_origin_pos = get_ent_origin(entity);
    struct vec3_t *ent_mins = get_collideable_mins(entity);
    struct vec3_t *ent_maxs = get_collideable_maxs(entity);

    // Bounding box points
    struct vec3_t frt = { ent_origin_pos.x + ent_maxs->x, ent_origin_pos.y + ent_maxs->y, ent_origin_pos.z + ent_maxs->z };
    struct vec3_t blb = { ent_origin_pos.x + ent_mins->x, ent_origin_pos.y + ent_mins->y, ent_origin_pos.z + ent_mins->z };
    struct vec3_t brt = { ent_origin_pos.x + ent_maxs->x, ent_origin_pos.y + ent_mins->y, ent_origin_pos.z + ent_maxs->z };
    struct vec3_t flb = { ent_origin_pos.x + ent_mins->x, ent_origin_pos.y + ent_maxs->y, ent_origin_pos.z + ent_mins->z };
    struct vec3_t brb = { ent_origin_pos.x + ent_maxs->x, ent_origin_pos.y + ent_mins->y, ent_origin_pos.z + ent_mins->z };
    struct vec3_t flt = { ent_origin_pos.x + ent_mins->x, ent_origin_pos.y + ent_maxs->y, ent_origin_pos.z + ent_maxs->z };
    struct vec3_t blt = { ent_origin_pos.x + ent_mins->x, ent_origin_pos.y + ent_mins->y, ent_origin_pos.z + ent_maxs->z };
    struct vec3_t frb = { ent_origin_pos.x + ent_maxs->x, ent_origin_pos.y + ent_maxs->y, ent_origin_pos.z + ent_mins->z };

    struct vec3_t frt_2d, blb_2d, brt_2d, flb_2d, brb_2d, flt_2d, blt_2d, frb_2d;
    if (screen_position(&frt, &frt_2d) != 0 || screen_position(&blb, &blb_2d) != 0 || screen_position(&brt, &brt_2d) != 0 || screen_position(&flb, &flb_2d) != 0 || screen_position(&brb, &brb_2d) != 0 || screen_position(&flt, &flt_2d) != 0 || screen_position(&blt, &blt_2d) != 0 || screen_position(&frb, &frb_2d) != 0)
    {
        return (struct bounding_box){ 0, 0, 0, 0 };
    }

    // Get best 2d bounding box
    struct vec3_t *points[] = { &frt_2d, &blb_2d, &brt_2d, &flb_2d, &brb_2d, &flt_2d, &blt_2d, &frb_2d };
    float left = points[0]->x;
    float bottom = points[0]->y;
    float right = points[0]->x;
    float top = points[0]->y;

    for (int i = 1; i < 8; i++)
    {
        if (left > points[i]->x)
        {
            left = points[i]->x;
        }

        if (bottom < points[i]->y)
        {
            bottom = points[i]->y;
        }

        if (right < points[i]->x)
        {
            right = points[i]->x;
        }

        if (top > points[i]->y)
        {
            top = points[i]->y;
        }
    }

    struct bounding_box box = { left, top, right, bottom };

    return box;
}

bool is_good_box(struct bounding_box box)
{
    return box.left != 0 && box.top != 0 && box.right != 0 && box.bottom != 0;
}

void draw_outlined_box(struct bounding_box box, int r, int g, int b, int a)
{
    draw_set_color(0, 0, 0, 255);
    draw_outlined_rect(box.left - 1, box.top - 1, box.right + 1, box.bottom + 1);
    draw_outlined_rect(box.left + 1, box.top + 1, box.right - 1, box.bottom - 1);
    draw_set_color(r, g, b, a);
    draw_outlined_rect(box.left, box.top, box.right, box.bottom);
}