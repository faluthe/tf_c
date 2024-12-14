#ifndef USER_CMD_H
#define USER_CMD_H
#include "math/vec3.h"

#include <stdbool.h>

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
    unsigned char impulse;
    int weapon_select;
    int weapon_subtype;
    int random_seed;
    short mouse_dx;
    short mouse_dy;
    bool has_been_predicted;
};

#endif // USER_CMD_H