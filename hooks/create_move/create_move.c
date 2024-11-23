#include "../../utils/utils.h"
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
    
    if (user_cmd->tick_count > 1)
    {
        clear_render_queue();
        aim_at_best_target(localplayer, user_cmd);
    }

    // If player is not on ground unset jump button flag (breaks scout double jump)
    if ((get_ent_flags(localplayer) & 1) == 0)
    {
        user_cmd->buttons &= ~2;
    }

    if (silent_aim)
    {
        return false;
    } else
    {
        return rc;
    }
}
