#include "../../config/config.h"
#include "../../utils/utils.h"
#include "../../source_sdk/cvar/convar/convar.h"
#include "../../source_sdk/cvar/cvar.h"
#include "../../source_sdk/engine_client/engine_client.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/user_cmd.h"
#include "../paint_traverse/paint_traverse.h"
#include "create_move.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static bool silent_aim = true;

// extern
__int64_t (*create_move_original)(void *, float, void *) = NULL;

__int64_t create_move_hook(void *this, float sample_time, struct user_cmd *user_cmd)
{
    __int64_t rc = create_move_original(this, sample_time, user_cmd);
    static bool hooked = false;

    if (!hooked)
    {
        log_msg("CreateMove hooked!\n");
        hooked = true;
    }
    
    if (!is_in_game())
    {
        return rc;
    }

    void *localplayer = get_localplayer();

    if (!localplayer)
    {
        log_msg("localplayer is NULL\n");
        return rc;
    }

    if (config.misc.do_thirdperson == true)
    {
        set_thirdperson(localplayer, true);
    }
    else
    {
        set_thirdperson(localplayer, false);
    }
    
    if (user_cmd->tick_count > 1)
    {
        clear_render_queue();
        aimbot(localplayer, user_cmd);
        bunny_hop(localplayer, user_cmd);
        autostrafe(localplayer, user_cmd);
    }

    if (silent_aim)
    {
        return false;
    } 
    else
    {
        return rc;
    }
}
