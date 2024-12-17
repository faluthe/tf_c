#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "../libs/nuklear/nuklear.h"

#include <stdbool.h>

enum input_type
{
    INPUT_NONE,
    INPUT_KEY,
    INPUT_MOUSE,
};

struct key_binding
{
    enum input_type type;
    int code;
    bool editing;
};

struct config
{
    struct
    {
        int bunny_hop;
        bool do_thirdperson;
    } misc;
    struct
    {
        int aimbot_enabled;
        int draw_fov;
        float fov;
        struct nk_colorf fov_color;
        struct
        {
            int use_key;
            struct key_binding binding;
            bool is_pressed;
        } key;
        struct
        {
            int draw_line;
            struct nk_colorf line_color;
            int draw_box;
            struct nk_colorf box_color;
            int only_draw_if_target;
            int previous_shot_line;
            struct nk_colorf previous_shot_line_color;
            int previous_shot_box;
            struct nk_colorf previous_shot_box_color;
            int draw_timer;
            struct nk_colorf timer_color;
        } projectile_preview;
    } aimbot;
    struct
    {
        // Player ESP
        int player_health_bar;
        int player_bounding_box;
        int player_name;
        int team_color;
        struct nk_colorf esp_color;
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
bool save_config();
