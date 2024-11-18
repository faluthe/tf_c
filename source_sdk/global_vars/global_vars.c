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


// typedef struct {
//     float realtime;
//     int framecount;
//     float absoluteframetime;
//     float curtime;
//     float frametime;
//     int maxClients;
//     int tickcount;
//     float interval_per_tick;
//     float interpolation_amount;
//     int simTicksThisFrame;
//     int network_protocol;
//     void* pSaveData; /* CSaveRestoreData* */
//     bool m_bClient;
//     int nTimestampNetworkingBase;
//     int nTimestampRandomizeWindow;
// } CGlobalVars;

struct global_vars *global_vars = NULL;

void set_global_vars_ptr(void *global_vars_ptr)
{
    global_vars = (struct global_vars *)global_vars_ptr;
}

float get_global_vars_curtime()
{
    return global_vars->curtime;
}

float get_global_vars_interval_per_tick()
{
    return global_vars->interval_per_tick;
}
