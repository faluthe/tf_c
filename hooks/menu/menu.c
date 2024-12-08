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

void draw_aim_tab(struct nk_context *ctx)
{
    nk_layout_row_dynamic(ctx, 20, 4);
    nk_label(ctx, "Aimbot FOV", NK_TEXT_LEFT);
    nk_slider_float(ctx, 1.0f, &config.aimbot_fov, 50.0f, 1.0f);
}

void draw_esp_tab(struct nk_context *ctx)
{
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

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Ammo & HP name", &config.esp.ammo_hp_ents_name);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Ammo & HP bounding box", &config.esp.ammo_hp_ents_bounding_box);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Sentry name", &config.esp.sentry_name);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Sentry bounding box", &config.esp.sentry_bounding_box);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Teleporter name", &config.esp.teleporter_name);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Teleporter bounding box", &config.esp.teleporter_bounding_box);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Dispenser name", &config.esp.dispenser_name);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Dispenser bounding box", &config.esp.dispenser_bounding_box);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Friendly dispenser name", &config.esp.friendly_dispenser_name);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_checkbox_label(ctx, "Friendly dispenser bounding box", &config.esp.friendly_dispenser_bounding_box);
}

void draw_menu(struct nk_context *ctx)
{
    if (nk_begin(ctx, "TF_C", nk_rect(200, 200, 500, 600), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
        {
            static int tab = 0;

            nk_layout_row_dynamic(ctx, 30, 3);
            if (nk_button_label(ctx, "Aim"))
            {
                log_msg("Button presed\n");
                tab = 0;
            }

            if (nk_button_label(ctx, "ESP"))
            {
                tab = 1;
            }

            if (nk_button_label(ctx, "Misc"))
            {
                tab = 2;
            }

            switch (tab)
            {
                case 0:
                    draw_aim_tab(ctx);
                    break;
                case 1:
                    draw_esp_tab(ctx);
                    break;
                case 2:
                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Misc tab", NK_TEXT_CENTERED);
                    break;
            }
        }
    nk_end(ctx);
}