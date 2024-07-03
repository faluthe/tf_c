#include "../../source_sdk/debug_overlay/debug_overlay.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/surface/surface.h"
#include "../../source_sdk/panel/panel.h"
#include "../../utils/utils.h"

#include "paint_traverse.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// extern
__int64_t (*paint_traverse_original)(void *, void *, __int8_t, __int8_t) = NULL;

void paint_traverse_hook(void *this, void *panel, __int8_t force_repaint, __int8_t allow_force)
{
    static bool hooked = false;
    static unsigned long esp_font = 0;

    if (!hooked)
    {
        esp_font = text_create_font();
        text_set_font_glyph_set(esp_font, "Courier New", 15, 300, 0, 0, 0x210);

        log_msg("esp_font: %lu\n", esp_font);
        hooked = true;
    }

    paint_traverse_original(this, panel, force_repaint, allow_force);

    const char *panel_name = get_panel_name(panel);

    if (strcmp(panel_name, "FocusOverlayPanel") != 0)
    {
        return;
    }

    draw_set_text_font(esp_font);
    draw_set_text_color(255, 255, 255, 255);
    draw_set_text_pos(50, 50);
    draw_print_text(L"Hello, world!", 13);

    void *localplayer = get_localplayer();

    if (localplayer == NULL)
    {
        return;
    }

    draw_set_color(255, 0, 0, 255);

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

        struct vec3_t ent_pos = get_bone_pos(entity, 6);
        struct vec3_t ent2d;

        if (screen_position(&ent_pos, &ent2d) != 0)
        {
            continue;
        }

        draw_filled_rect((int)(ent2d.x - 5.0f), (int)(ent2d.y - 5.0f), (int)(ent2d.x + 5.0f), (int)(ent2d.y + 5.0f));
    }
}