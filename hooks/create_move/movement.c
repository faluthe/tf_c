#include "../../config/config.h"
#include "../../utils/utils.h"
#include "../../source_sdk/entity/entity.h"
#include "../../source_sdk/math/vec3.h"
#include "../../source_sdk/user_cmd.h"
#include "create_move.h"

void bhop(void *localplayer, struct user_cmd *user_cmd){

    /* scouts double jump only works sometimes, very inconsistent
     * i haven't figured out why (FROM TESTING: i belive the on ground flag sometimes is not getting set correctly)
     * in any case, i'll just disable bhop on scout until i figure it out */

    if(get_player_class(localplayer) == TF_CLASS_SCOUT){
        return;
    }

    static bool was_jumping = false;
    bool on_ground = (get_ent_flags(localplayer) & FL_ONGROUND);

    if(user_cmd->buttons & IN_JUMP){
        if(!was_jumping && !on_ground){
            user_cmd->buttons &= ~IN_JUMP;
        }
        else if(was_jumping){
            was_jumping = false;
        }
    } else if(!was_jumping){
        was_jumping = true;
    }
}

void autostrafe(void *localplayer, struct user_cmd *user_cmd){

    // TBD: figure out how to implement directional / rage autostrafe
    // i don't think this kind of autostrafe does anything

    if(get_player_class(localplayer) == TF_CLASS_SCOUT){
        return;
    }

    float cl_sidespeed = 450.0f; //assume default values 

    bool on_ground = get_ent_flags(localplayer) & FL_ONGROUND;

    if(on_ground){
        return;
    }

    if(user_cmd->mouse_dx < 0)
    {
        user_cmd->sidemove = -cl_sidespeed; 
    } else if(user_cmd->mouse_dx > 0)
    {
        user_cmd->sidemove = cl_sidespeed;
    }
}
