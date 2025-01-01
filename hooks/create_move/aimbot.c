#include "../../config/config.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/entity/weapon_entity.h"
#include "../../source_sdk/entity_list/entity_list.h"
#include "../../source_sdk/user_cmd.h"
#include "create_move.h"

#include <stddef.h>

void aimbot(void *localplayer, struct user_cmd *user_cmd)
{
    if (!config.aimbot.aimbot_enabled)
    {
        return;
    }

    void *active_weapon = get_client_entity(get_active_weapon(localplayer));
    if (active_weapon == NULL)
    {
        return;
    }

    int weapon_id = get_weapon_id(active_weapon);
    bool is_projectile_class = get_player_class(localplayer) == TF_CLASS_SOLDIER;
    bool is_projectile_weapon = weapon_id == TF_WEAPON_ROCKETLAUNCHER || weapon_id == TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT;

    if (is_projectile_class && is_projectile_weapon)
    {
        projectile_aimbot(localplayer, user_cmd, weapon_id);
    }
    else
    {
        hitscan_aimbot(localplayer, user_cmd);
    }
}
