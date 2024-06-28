#ifndef USER_CMD_H
#define USER_CMD_H
#include "math/vec3.h"

struct user_cmd
{
    void *something;
    int command_number;
    int tick_count;
    struct vec3_t viewangles;
    float forwardmove;
    float sidemove;
    float upmove;
    int buttons;
};

#endif // USER_CMD_H