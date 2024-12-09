#include "config.h"

#include <SDL2/SDL.h>

struct config config;

bool init_config()
{
    config.aimbot.key.is_pressed = false;

    // TBD: Read config from file
    config.aimbot.aimbot_enabled = 1;
    config.aimbot.draw_fov = 1;
    config.aimbot.fov = 10.0f;
    config.aimbot.fov_color = (struct nk_colorf){ 207.0f / 255.0f, 115.0f / 255.0f, 54.0f / 255.0f, 0.25f };
    config.aimbot.key.use_key = 1;
    config.aimbot.key.code = SDL_BUTTON_X2;
    config.aimbot.key.is_mouse_btn = true;
    config.aimbot.key.is_pressed = false;
    config.esp.player_health_bar = 1;
    config.esp.player_bounding_box = 1;
    config.esp.player_name = 1;
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

    return true;
}