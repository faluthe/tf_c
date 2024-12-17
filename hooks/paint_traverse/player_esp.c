#include "../../config/config.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/surface/surface.h"
#include "../../utils/utils.h"
#include "esp_util/esp_util.h"
#include "paint_traverse.h"

#include "../hooks.h"
#include <stdatomic.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

void draw_2d_box(void *entity, int ent_index)
{
    struct bounding_box box = get_ent_2d_box(entity);

    if (!is_good_box(box))
    {
        return;
    }

    // Tighten bounding box
    float height = box.bottom - box.top;
    box.left += height / 10;
    box.top += height / 10; 
    box.right -= height / 10;
    height = box.bottom - box.top;


    int r = config.esp.esp_color.r * 255;
    int g = config.esp.esp_color.g * 255;
    int b = config.esp.esp_color.b * 255;
    int a = config.esp.esp_color.a * 255;


    if(config.esp.team_color){

        int team = get_ent_team(entity);
        
        if(team == 2) { //red
            r = 255;
            g = 0;
            b = 0;
            a = 255;
        }
        
        if(team == 3){ //blue
            r = 0;
            g = 0;
            b = 255;
            a = 255;
        }

    }
    
    if (config.esp.player_bounding_box)
    {
        draw_outlined_box(box, r, g, b, a);
    }

    if (config.esp.player_health_bar)
    {
        int max_health = get_ent_max_health(entity);
        int health = get_ent_health(entity) > max_health ? max_health : get_ent_health(entity);
        int over_heal = get_ent_health(entity) - max_health;
        float health_percentage = (float)health / (float)max_health;
        float bar_height = (float)height * health_percentage;
        
        // Health bar + background
        draw_set_color(0, 0, 0, 255 / 2);
        draw_filled_rect(box.right + 1, box.bottom - height, box.right + 4, box.bottom);
        int green = fmin(health_percentage * 2 * 255, 255);
        int red = fmin((1 - health_percentage) * 2 * 255, 255);
        draw_set_color(red, green, 0, 255);
        draw_filled_rect(box.right + 2, box.bottom - bar_height + 1, box.right + 3, box.bottom - 1);

        // Overheal + background
        if (over_heal > 0)
        {
            float over_heal_height = (float)height * ((float)over_heal / (float)max_health);
            draw_set_color(0, 0, 0, 255);
            draw_filled_rect(box.right + 5, box.bottom - over_heal_height, box.right + 8, box.bottom);
            draw_set_color(65, 105, 225, 255);
            draw_filled_rect(box.right + 6, box.bottom - over_heal_height + 1, box.right + 7, box.bottom - 1);
        }
    }

    if (config.esp.player_name)
    {
        struct player_info_t ent_info;
        if (!get_player_info(ent_index, &ent_info))
        {
            return;
        }

        //convert to wide char string
        wchar_t name[32];
        size_t len = mbstowcs(name, ent_info.name, 32);
        if (len != (size_t)-1) {
            draw_set_text_color(255, 255, 255, 255);
            draw_set_text_pos(box.left, box.top - 20);
            draw_print_text(name, wcslen(name));
        }
    }

    draw_bbox_decorators(box.right + 20, box.top, entity);
}

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

        draw_2d_box(entity, ent_index);
    }
}
