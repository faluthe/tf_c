#include "config/config.h"
#include "hooks/hooks.h"
#include "hooks/paint_traverse/paint_traverse.h"
#include "interfaces/interfaces.h"
#include "source_sdk/estimate_abs_velocity/estimate_abs_velocity.h"
#include "source_sdk/engine_client/engine_client.h"
// #include "source_sdk/entity/entity.h"
#include "utils/utils.h"

#include <stdio.h>
#include <stdlib.h>

__attribute__((destructor)) void unload()
{
    restore_hooks();
    destroy_render_queue();
    log_msg("tf_c unloaded\n");
}

__attribute__((constructor)) void init()
{
    if (!init_config())
    {
        log_msg("Failed to initialize config\n");
        return;
    }

    if (!init_interfaces())
    {
        log_msg("Failed to initialize interfaces\n");
        return;
    }
    
    if (!init_estimate_abs_velocity())
    {
        log_msg("Failed to initialize estimate_abs_velocity\n");
        return;
    }

    if (!init_hooks())
    {
        log_msg("Failed to initialize hooks\n");
        return;
    }
}
