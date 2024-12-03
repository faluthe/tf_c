#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/surface/surface.h"
#include "../../utils/utils.h"
#include "esp_util/esp_util.h"
#include "paint_traverse.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>

const char *get_class_name(enum class_id class_id)
{
    // TBD: Check model name for more accurate class name
    switch (class_id)
    {
        case AMMO_OR_HEALTH_PACK:
            return "AMMO/HP";
        case DISPENSER:
            return "DISPENSER";
        case SENTRY:
            return "SENTRY";
        case TELEPORTER:
            return "TELEPORTER";
        case ARROW:
            return "ARROW";
        case ROCKET:
            return "ROCKET";
        case PILL_OR_STICKY:
            return "NADE";
        case FLARE:
            return "FLARE";
        case CROSSBOW_BOLT:
            return "BOLT";
        default:
            return NULL;
    }
}

void draw_entity_esp()
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

    for (int ent_index = 1; ent_index <= get_max_entities(); ent_index++)
    {
        void *entity = get_client_entity(ent_index);

        if (entity == NULL || is_ent_dormant(entity))
        {
            continue;
        }

        int class_id = get_class_id(entity);
        int team = get_ent_team(entity);
        int local_team = get_ent_team(localplayer);
        const char *class_name = get_class_name(class_id);
        
        if (class_name == NULL || 
            (class_id == AMMO_OR_HEALTH_PACK && team != 0) || 
            (class_id == SENTRY && team == local_team) || 
            (class_id == ARROW && team == local_team) || 
            (class_id == ROCKET && team == local_team) || 
            (class_id == PILL_OR_STICKY && team == local_team) || 
            (class_id == FLARE && team == local_team))
        {
            continue;
        }

        struct bounding_box box = get_ent_2d_box(entity);

        if (!is_good_box(box))
        {
            continue;
        }
        
        if (team == 2)
        {
            draw_set_color(184, 56, 59, 255);
            draw_set_text_color(184, 56, 59, 255);
        }
        else if (team == 3)
        {
            draw_set_color(88, 133, 162, 255);
            draw_set_text_color(88, 133, 162, 255);
        }
        else
        {
            draw_set_color(255, 255, 255, 255);
            draw_set_text_color(255, 255, 255, 255);
        }

        draw_filled_rect(box.left, box.top, box.right, box.bottom);

        wchar_t class_name_w[64];
        size_t len = mbstowcs(class_name_w, class_name, 64);
        if (len != (size_t)-1)
        {
            draw_set_text_pos(box.left, box.top - 20);
            draw_print_text(class_name_w, wcslen(class_name_w));
        }

        draw_bbox_decorators(box.right + 20, box.top, entity);
    }
}