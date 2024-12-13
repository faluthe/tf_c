#ifndef MOVE_DATA_H
#include "math/vec3.h"

#include <stdbool.h>

struct move_data
{
    bool first_run_of_functions : 1;
    bool game_code_moved_player : 1;
    unsigned long player_handle;
    int impulse_command;
    struct vec3_t view_angles;
    struct vec3_t abs_view_angles;
    int buttons;
    int old_buttons;
    float forward_move;
    float old_forward_move;
    float side_move;
    float up_move;
    float max_speed;
    float client_max_speed;
    struct vec3_t velocity; 
    struct vec3_t angles;
    struct vec3_t old_angles;
    float out_step_height;
    struct vec3_t out_wish_vel;
    struct vec3_t out_jump_vel;
    struct vec3_t constraint_center;
    float constraint_radius;
    float constraint_width;
    float constraint_speed_factor;
    // TBD: Could be missing a few fields here...
    struct vec3_t abs_origin;
};
#endif