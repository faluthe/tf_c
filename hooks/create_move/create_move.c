#include "../../utils/utils.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/user_cmd.h"
#include "create_move.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// extern
__int64_t (*create_move_original)(void *, float, void *) = NULL;

__int64_t create_move_hook(void *this, float sample_time, struct user_cmd *user_cmd)
{
    static bool hooked = false;

    void *localplayer = get_localplayer();

    if (!localplayer)
    {
        return false;
    }

    if (!hooked)
    {
        log_msg("CreateMove hooked! Localplayer: %p\n", localplayer);
        hooked = true;
    }

    if (user_cmd->tick_count < 1)
    {
        return false;
    }

    log_msg("buttons %d\n", user_cmd->buttons);

    if ((user_cmd->buttons & 2048) != 0)
    {
        aim_at_best_target(localplayer, user_cmd);
    }

    if ((get_ent_flags(localplayer) & 1) == 0)
    {
        user_cmd->buttons &= ~2;
    }

    __int64_t rc = create_move_original(this, sample_time, user_cmd);

    return rc;
}