#include "global_vars.h"

#include "../../utils/utils.h"

#include <stddef.h>

struct global_vars
{
    float realtime;
    int framecount;
    float absoluteframetime;
    float curtime;
    float frametime;
    int maxclients;
    int tickcount;
    float interval_per_tick;
    float interpolation_amount;
    int sim_ticks_this_frame;
    int network_protocol;
};

struct global_vars *global_vars = NULL;

void set_global_vars_ptr(void *global_vars_ptr)
{
    global_vars = (struct global_vars *)global_vars_ptr;
}

float get_global_vars_curtime()
{
    return global_vars->curtime;
}

void set_global_vars_curtime(float curtime)
{
    global_vars->curtime = curtime;
}

float get_global_vars_interval_per_tick()
{
    return global_vars->interval_per_tick;
}

float get_global_vars_frametime()
{
    return global_vars->frametime;
}

void set_global_vars_frametime(float frametime)
{
    global_vars->frametime = frametime;
}

int get_global_vars_tickcount()
{
    return global_vars->tickcount;
}

void set_global_vars_tickcount(int tickcount)
{
    global_vars->tickcount = tickcount;
}