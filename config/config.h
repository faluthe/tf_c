#include <stdbool.h>

struct config
{
    struct
    {
        // Player ESP
        int player_health_bar;
        int player_bounding_box;
        int player_name;
        // Other Entity ESP
        int ammo_hp_ents_name;
        int ammo_hp_ents_bounding_box;
        int sentry_name;
        int sentry_bounding_box;
        int teleporter_name;
        int teleporter_bounding_box;
        int dispenser_name;
        int dispenser_bounding_box;
        int friendly_dispenser_name;
        int friendly_dispenser_bounding_box;
    } esp;
};

extern struct config config;

bool init_config();