#include "../../source_sdk/user_cmd.h"

#include <stdint.h>

__int64_t create_move_hook(void *this, float sample_time, struct user_cmd *user_cmd);
extern __int64_t (*create_move_original)(void *, float, void *);

/* AIMBOT */
void aimbot(void *localplayer, struct user_cmd *user_cmd);
void hitscan_aimbot(void *localplayer, struct user_cmd *user_cmd);
void projectile_aimbot(void *localplayer, struct user_cmd *user_cmd, int weapon_id);


/* MOVEMENT */
void bhop(void *localplayer, struct user_cmd *user_cmd);
void autostrafe(void *localplayer, struct user_cmd *user_cmd);