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

        draw_set_color(0, 0, 255, 255);

        struct vec3_t ent_eye_pos = get_bone_pos(entity, 6);
        struct vec3_t ent_eye_2d;

        if (screen_position(&ent_eye_pos, &ent_eye_2d) != 0)
        {
            continue;
        }

        float box_width = 10.0f;
        draw_filled_rect(ent_eye_2d.x - box_width, ent_eye_2d.y - box_width, ent_eye_2d.x + box_width, ent_eye_2d.y + box_width);
    }
}