#include "config.h"

#include <SDL2/SDL.h>
#include <stdio.h>

// TBD: Allow multiple configs and custom names
static const char *config_path = "./config.tf_c.bin";
// extern
struct config config;
// Orange Box orange
static const struct nk_colorf default_color = { 207.0f / 255.0f, 115.0f / 255.0f, 54.0f / 255.0f, 0.25f };

bool init_config()
{
    FILE *config_file = fopen(config_path, "rb");

    if (config_file == NULL || !fread(&config, sizeof(struct config), 1, config_file))
    {
        config = (struct config){ 0 };

        config.aimbot.fov_color = default_color;
        config.aimbot.projectile_time_step = 0.01f;
        config.aimbot.projectile_max_time = 1.0f;
        config.aimbot.projectile_tolerance_time = 0.05f;
        config.aimbot.key.binding = (struct key_binding){ INPUT_MOUSE, SDL_BUTTON_X2, false };
        config.aimbot.projectile_preview.line_color = default_color;
        config.aimbot.projectile_preview.box_color = default_color;
        config.aimbot.projectile_preview.previous_shot_line_color = default_color;
        config.aimbot.projectile_preview.previous_shot_box_color = default_color;
        config.aimbot.projectile_preview.timer_color = default_color;
        config.aimbot.projectile_preview.entity_prediction_color = (struct nk_colorf){ 1.0f, 1.0f, 1.0f, 1.0f };
        config.esp.esp_color = (struct nk_colorf){ 1.0f, 1.0f, 1.0f, 1.0f};

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
