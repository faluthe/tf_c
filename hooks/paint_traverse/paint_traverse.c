#include "../../source_sdk/global_vars/global_vars.h"
#include "../../source_sdk/surface/surface.h"
#include "../../source_sdk/panel/panel.h"
#include "paint_traverse.h"

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

// extern
__int64_t (*paint_traverse_original)(void *, void *, __int8_t, __int8_t) = NULL;

void paint_traverse_hook(void *this, void *panel, __int8_t force_repaint, __int8_t allow_force)
{
    paint_traverse_original(this, panel, force_repaint, allow_force);
    
    static bool hooked = false;
    static unsigned long esp_font = 0;

    if (!hooked)
    {
        esp_font = text_create_font();
        text_set_font_glyph_set(esp_font, "Tahoma", 15, 200, 0, 0, 0x210);

        init_render_queue();

        hooked = true;
    }

    const char *panel_name = get_panel_name(panel);

    if (strcmp(panel_name, "FocusOverlayPanel") != 0)
    {
        return;
    }

    draw_set_text_font(esp_font);
    draw_set_text_color(255, 255, 255, 255);
    draw_set_text_pos(50, 50);
    draw_print_text(L"tf_c", 4);

    float curtime = get_global_vars_curtime();
    wchar_t curtime_str[64];
    draw_set_text_pos(50, 110);
    draw_print_text(L"curtime: ", 9);
    swprintf(curtime_str, 64, L"%.5f", curtime);
    draw_set_text_pos(50, 130);
    draw_print_text(curtime_str, wcslen(curtime_str));

    draw_player_esp();
    draw_render_queue();
}