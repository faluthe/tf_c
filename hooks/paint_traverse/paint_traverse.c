#include "../../config/config.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/surface/surface.h"
#include "../../source_sdk/panel/panel.h"
#include "../../utils/utils.h"
#include "paint_traverse.h"

#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

// extern
__int64_t (*paint_traverse_original)(void *, void *, __int8_t, __int8_t) = NULL;

// TBD: Fix for scoped weapons
void draw_aimbot_fov()
{
    if (!config.aimbot.draw_fov || !is_in_game())
    {
        return;
    }

    void *localplayer = get_localplayer();

    if (!localplayer || get_ent_lifestate(localplayer) != true)
    {
        return;
    }

    int w, h;
    get_screen_size(&w, &h);

    int radius = tan(config.aimbot.fov / 180 * M_PI) / tan((90.0f / 2) / 180 * M_PI) * (w / 2);

    draw_set_color(
        config.aimbot.fov_color.r * 255.0f,
        config.aimbot.fov_color.g * 255.0f, 
        config.aimbot.fov_color.b * 255.0f, 
        config.aimbot.fov_color.a * 255.0f
    );
    draw_circle(w / 2, h /2, radius, 255);
}

void paint_traverse_hook(void *this, void *panel, __int8_t force_repaint, __int8_t allow_force)
{
    paint_traverse_original(this, panel, force_repaint, allow_force);
    
    static bool hooked = false;
    static unsigned long esp_font = 0;

    if (!hooked)
    {
        esp_font = text_create_font();
        text_set_font_glyph_set(esp_font, "ProggySquare", 14, 400, 0, 0, 0x0);

        init_render_queue();

        hooked = true;
    }

    const char *panel_name = get_panel_name(panel);

    if (strcmp(panel_name, "FocusOverlayPanel") != 0)
    {
        return;
    }

    draw_set_text_font(esp_font);

    draw_player_esp();
    draw_entity_esp();
    draw_render_queue();
    draw_aimbot_fov();
}