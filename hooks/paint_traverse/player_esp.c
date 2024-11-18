#include "../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/surface/surface.h"
#include "../../utils/utils.h"

#include "../hooks.h"
#include <stdatomic.h>
#include <stddef.h>

void draw_player_esp()
{
    if (!is_in_game())
    {
        return;
    }

    void *localplayer = get_localplayer();

    if (localplayer == NULL)
    {
        return;
    }

    for (int ent_index = 1; ent_index <= get_max_clients(); ent_index++)
    {
        void *entity = get_client_entity(ent_index);

        if (entity == NULL || entity == localplayer)
        {
            continue;
        }

        if (is_ent_dormant(entity) || get_ent_lifestate(entity) != 1 || get_ent_team(entity) == get_ent_team(localplayer))
        {
            continue;
        }

        struct vec3_t ent_origin_pos = get_ent_origin(entity);
        struct vec3_t *ent_mins = get_collideable_mins(entity);
        struct vec3_t *ent_maxs = get_collideable_maxs(entity);

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
            continue;
        }

        struct vec3_t *points[] = { &frt_2d, &blb_2d, &brt_2d, &flb_2d, &brb_2d, &flt_2d, &blt_2d, &frb_2d };
        float left = points[0]->x;
        float top = points[0]->y;
        float right = points[0]->x;
        float bottom = points[0]->y;

        for (int i = 1; i < 8; i++)
        {
            if (left > points[i]->x)
            {
                left = points[i]->x;
            }

            if (top < points[i]->y)
            {
                top = points[i]->y;
            }

            if (right < points[i]->x)
            {
                right = points[i]->x;
            }

            if (bottom > points[i]->y)
            {
                bottom = points[i]->y;
            }
        }

        float height = top - bottom;
        left += height / 10;
        bottom += height / 10;
        right -= height / 10;
        draw_set_color(255, 255, 255, 255);
        draw_filled_rect(left, bottom, right, top);
        draw_set_color(0, 255, 0, 255);
        draw_filled_rect(left - 3, bottom, left - 2, top);
    }
}