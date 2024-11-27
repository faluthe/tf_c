#include "hooks/hooks.h"
#include "hooks/paint_traverse/paint_traverse.h"
#include "interfaces/interfaces.h"
#include "source_sdk/estimate_abs_velocity/estimate_abs_velocity.h"
#include "utils/utils.h"

#include <stdio.h>
#include <stdlib.h>

__attribute__((destructor)) void unload()
{
    restore_hooks();
    destroy_render_queue();
    log_msg("tf_c unloaded\n");
    close_log();
}

__attribute__((constructor)) void init()
{
    log_msg("Initializing tf_c...\n");

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
