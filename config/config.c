#include "config.h"

#include <SDL2/SDL.h>
#include <stdio.h>

struct config config;

// TBD: Allow multiple configs and custom names
static const char *config_path = "./config.tf_c.bin";

bool init_config()
{
    FILE *config_file = fopen(config_path, "rb");

    if (config_file == NULL || !fread(&config, sizeof(struct config), 1, config_file))
    {
        config.aimbot.key.is_pressed = false;

        config.misc.bunny_hop = 1;
        config.misc.legit_autostrafe = 0;
        config.misc.rage_autostrafe = 1;
        config.misc.do_thirdperson = false;
        config.aimbot.aimbot_enabled = 1;
        config.aimbot.draw_fov = 1;
        config.aimbot.fov = 10.0f;
        config.aimbot.fov_color = (struct nk_colorf) {207.0f / 255.0f, 115.0f / 255.0f, 54.0f / 255.0f, 0.25f};
        config.aimbot.projectile_time_step = 0.01f;
        config.aimbot.projectile_max_time = 1.0f;
        config.aimbot.projectile_tolerance_time = 0.05f;
        config.aimbot.key.use_key = 1;
        config.aimbot.key.binding = (struct key_binding) {INPUT_MOUSE, SDL_BUTTON_X2, false};
        config.aimbot.key.is_pressed = false;
        config.aimbot.projectile_preview.draw_line = 1;
        config.aimbot.projectile_preview.line_color = (struct nk_colorf) {1.0f, 0.0f, 0.0f, 1.0f};
        config.aimbot.projectile_preview.draw_box = 1;
        config.aimbot.projectile_preview.box_color = (struct nk_colorf) {1.0f, 0.0f, 0.0f, 1.0f};
        config.aimbot.projectile_preview.only_draw_if_target = 1;
        config.aimbot.projectile_preview.previous_shot_line = 1;
        config.aimbot.projectile_preview.previous_shot_line_color = (struct nk_colorf) {1.0f, 0.0f, 0.0f, 1.0f};
        config.aimbot.projectile_preview.previous_shot_box = 1;
        config.aimbot.projectile_preview.previous_shot_box_color = (struct nk_colorf) {1.0f, 0.0f, 0.0f, 1.0f};
        config.aimbot.projectile_preview.draw_timer = 1;
        config.aimbot.projectile_preview.timer_color = (struct nk_colorf) {1.0f, 0.0f, 0.0f, 1.0f};
        config.aimbot.projectile_preview.previous_shot_linger_time = 0.5f;
        config.aimbot.projectile_preview.draw_entity_prediction = 1;
        config.aimbot.projectile_preview.entity_prediction_color = (struct nk_colorf) {1.0f, 1.0f, 1.0f, 1.0f};
        config.esp.player_health_bar = 1;
        config.esp.player_bounding_box = 1;
        config.esp.player_name = 1;
        config.esp.team_color = 1;
        config.esp.esp_color = (struct nk_colorf) {1.0f, 1.0f, 1.0f, 1.0f};
        config.esp.ammo_hp_ents_name = 1;
        config.esp.ammo_hp_ents_bounding_box = 1;
        config.esp.sentry_name = 1;
        config.esp.sentry_bounding_box = 1;
        config.esp.teleporter_name = 1;
        config.esp.teleporter_bounding_box = 1;
        config.esp.dispenser_name = 1;
        config.esp.dispenser_bounding_box = 1;
        config.esp.friendly_dispenser_name = 1;
        config.esp.friendly_dispenser_bounding_box = 1;

        return save_config();
    }

    fclose(config_file);
    return true;
}

bool save_config()
{
    FILE *config_file = fopen(config_path, "wb");

    if (config_file == NULL)
    {
        return false;
    }

    if (!fwrite(&config, sizeof(struct config), 1, config_file))
    {
        fclose(config_file);
        return false;
    }

    fclose(config_file);
    return true;
}
