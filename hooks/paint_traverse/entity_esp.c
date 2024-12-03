#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/surface/surface.h"
#include "../../utils/utils.h"
#include "esp_util/esp_util.h"

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
            return "Ammo/Health Pack";
        case DISPENSER:
            return "Dispenser";
        case SENTRY:
            return "Sentry";
        case TELEPORTER:
            return "Teleporter";
        case ARROW:
            return "Arrow";
        case ROCKET:
            return "Rocket";
        case PILL_OR_STICKY:
            return "Pill/Sticky";
        case FLARE:
            return "Flare";
        case CROSSBOW_BOLT:
            return "Crossbow Bolt";
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
            draw_set_color(255, 0, 0, 255);
            draw_set_text_color(255, 0, 0, 255);
        }
        else if (team == 3)
        {
            draw_set_color(0, 0, 255, 255);
            draw_set_text_color(0, 0, 255, 255);
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
            draw_set_text_pos(box.right + 20, box.top);
            draw_print_text(class_name_w, wcslen(class_name_w));
        }
    }
}