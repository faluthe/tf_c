#include "../../config/config.h"
#include "../../utils/utils.h"
#include "menu.h"

void watermark(struct nk_context *ctx)
{
    if (nk_begin(ctx, "watermark", nk_rect(10, 10, 150, 30), NK_WINDOW_BORDER | NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "TF_C by faluthe", NK_TEXT_CENTERED);
    }
    nk_end(ctx);
}

void multi_select_combo_box(struct nk_context *ctx, const char **options, int options_count, int **selections, int selection_count, char *preview_text, int preview_text_size)
{
    preview_text[0] = '\0';
    for (int k = 0; k < options_count; k++)
    {
        if (*selections[k])
        {
            if (strlen(preview_text) > 0)
            {
                strncat(preview_text, ", ", preview_text_size - strlen(preview_text) - 1);
            }
            strncat(preview_text, options[k], preview_text_size - strlen(preview_text) - 1);
        }
    }
    if (strlen(preview_text) == 0)
    {
        strncpy(preview_text, "None", preview_text_size - 1);
        preview_text[preview_text_size - 1] = '\0';
    }

    if (nk_combo_begin_label(ctx, preview_text, nk_vec2(nk_widget_width(ctx), 300)))
    {
        nk_layout_row_dynamic(ctx, 20, 1);
        for (int i = 0; i < options_count; i++)
        {
            if (nk_checkbox_label(ctx, options[i], selections[i]))
            {
                preview_text[0] = '\0';
                for (int k = 0; k < options_count; k++)
                {
                    if (*selections[k])
                    {
                        if (strlen(preview_text) > 0)
                        {
                            strncat(preview_text, ", ", preview_text_size - strlen(preview_text) - 1);
                        }
                        strncat(preview_text, options[k], preview_text_size - strlen(preview_text) - 1);
                    }
                }
                if (strlen(preview_text) == 0)
                {
                    strncpy(preview_text, "None", preview_text_size - 1);
                    preview_text[preview_text_size - 1] = '\0';
                }
            }
        }
        nk_combo_end(ctx);
    }
}

void draw_aim_tab(struct nk_context *ctx)
{
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_checkbox_label(ctx, "Aimbot enabled", &config.aimbot.aimbot_enabled);
    if (config.aimbot.aimbot_enabled)
    {
        nk_checkbox_label(ctx, "Use aim key", &config.aimbot.key.use_key);
    }

    nk_layout_row_dynamic(ctx, 20, 2);
    char fov_text[32];
    sprintf(fov_text, "Aimbot FOV: %.0f\xC2\xB0", config.aimbot.fov);
    nk_label(ctx, fov_text, NK_TEXT_LEFT);
    nk_slider_float(ctx, 1.0f, &config.aimbot.fov, 50.0f, 1.0f);
    
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_checkbox_label(ctx, "Draw FOV", &config.aimbot.draw_fov);

    nk_layout_row_dynamic(ctx, 20, 2);
    nk_label(ctx, "FOV color:", NK_TEXT_LEFT);
    if (nk_combo_begin_color(ctx, nk_rgb_cf(config.aimbot.fov_color), nk_vec2(nk_widget_width(ctx), 400))) {
        nk_layout_row_dynamic(ctx, 120, 1);
        config.aimbot.fov_color = nk_color_picker(ctx, config.aimbot.fov_color, NK_RGBA);
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_combo_end(ctx);
    }
}

void draw_esp_tab(struct nk_context *ctx)
{
    static const char *entity_esp_options[] = { "Name", "Bounding box" };

    nk_layout_row_dynamic(ctx, 30, 1);
    nk_label(ctx, "Enemy Player ESP", NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Health bar", &config.esp.player_health_bar);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Bounding box", &config.esp.player_bounding_box);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Name", &config.esp.player_name);

    nk_layout_row_dynamic(ctx, 30, 1);
    nk_label(ctx, "Entity ESP", NK_TEXT_LEFT);

    static int *ammo_hp_selections[] = { &config.esp.ammo_hp_ents_name, &config.esp.ammo_hp_ents_bounding_box };
    static char ammo_hp_preview_text[128] = "";
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_label(ctx, "Ammo/HP:", NK_TEXT_LEFT);
    multi_select_combo_box(ctx, entity_esp_options, 2, ammo_hp_selections, 2, ammo_hp_preview_text, sizeof(ammo_hp_preview_text));

    static int *sentry_selections[] = { &config.esp.sentry_name, &config.esp.sentry_bounding_box };
    static char sentry_preview_text[128] = "";
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_label(ctx, "Sentry:", NK_TEXT_LEFT);
    multi_select_combo_box(ctx, entity_esp_options, 2, sentry_selections, 2, sentry_preview_text, sizeof(sentry_preview_text));

    static int *teleporter_selections[] = { &config.esp.teleporter_name, &config.esp.teleporter_bounding_box };
    static char teleporter_preview_text[128] = "";
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_label(ctx, "Teleporter:", NK_TEXT_LEFT);
    multi_select_combo_box(ctx, entity_esp_options, 2, teleporter_selections, 2, teleporter_preview_text, sizeof(teleporter_preview_text));

    static int *dispenser_selections[] = { &config.esp.dispenser_name, &config.esp.dispenser_bounding_box };
    static char dispenser_preview_text[128] = "";
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_label(ctx, "Dispenser:", NK_TEXT_LEFT);
    multi_select_combo_box(ctx, entity_esp_options, 2, dispenser_selections, 2, dispenser_preview_text, sizeof(dispenser_preview_text));
    
    static int *friendly_dispenser_selections[] = { &config.esp.friendly_dispenser_name, &config.esp.friendly_dispenser_bounding_box };
    static char friendly_dispenser_preview_text[128] = "";
    nk_layout_row_dynamic(ctx, 20, 2);
    nk_label(ctx, "Friendly dispenser:", NK_TEXT_LEFT);
    multi_select_combo_box(ctx, entity_esp_options, 2, friendly_dispenser_selections, 2, friendly_dispenser_preview_text, sizeof(friendly_dispenser_preview_text));
}

void draw_misc_tab(struct nk_context *ctx)
{
    nk_layout_row_dynamic(ctx, 30, 1);
    nk_checkbox_label(ctx, "Bunny hop", &config.bunny_hop);
}

void draw_tab(struct nk_context *ctx, const char *name, int *tab, int index)
{
    if (*tab == index)
    {
        ctx->style.button.normal.data.color = nk_rgb(35, 35, 35);
    }
    else
    {
        ctx->style.button.normal.data.color = nk_rgb(50, 50, 50);
    }
    if (nk_button_label(ctx, name))
    {
        *tab = index;
    }
}

void draw_menu(struct nk_context *ctx)
{
    if (nk_begin(ctx, "TF_C", nk_rect(200, 200, 500, 600), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
        {
            static int tab = 0;

            nk_layout_row_dynamic(ctx, 30, 3);
            draw_tab(ctx, "Aim", &tab, 0);
            draw_tab(ctx, "ESP", &tab, 1);
            draw_tab(ctx, "Misc", &tab, 2);

            switch (tab)
            {
                case 0:
                    draw_aim_tab(ctx);
                    break;
                case 1:
                    draw_esp_tab(ctx);
                    break;
                case 2:
                    draw_misc_tab(ctx);
                    break;
            }
        }
    nk_end(ctx);
}