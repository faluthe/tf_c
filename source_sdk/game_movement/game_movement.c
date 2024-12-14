#include "../../utils/utils.h"
#include "game_movement.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void *interface = NULL;

void set_game_movement_interface(void *game_movement_interface)
{
    interface = game_movement_interface;
}

void process_movement(void *player, struct move_data *move_data)
{
    // static void (*func)(void *, void *, struct move_data *) = NULL;

    // if (func == NULL)
    // {
    //     // pattern: 55 48 89 E5 41 56 41 55 49 89 FD 48 89 F7 41 54 4C 8D 25 ? ? ? ?
    //     __uint8_t pattern2[] = { 0x55, 0x48, 0x89, 0xe5, 0x41, 0x56, 0x41, 0x55, 0x49, 0x89, 0xfd, 0x48, 0x89, 0xf7, 0x41, 0x54, 0x4c, 0x8d, 0x25, 0x0, 0x0, 0x0, 0x0 };
    //     void *scan_result2 = pattern_scan("client.so", pattern2, "xxxxxxxxxxxxxxxxxxx????");

    //     if (!scan_result2)
    //     {
    //         log_msg("Failed to find proccess_movement!\n");
    //     }
    //     else
    //     {
    //         log_msg("Found proccess_movement at %p\n", scan_result2);
    //     }

    //     log_msg("Found process_movement at %p\n", scan_result2);

    //     func = scan_result2;
    // }
    
    // func(interface, player, move_data);

    void **vtable = *(void ***)interface;

    void (*func)(void *, void *, struct move_data *) = vtable[2];

    return func(interface, player, move_data);
}

void start_track_prediction_errors(void *player)
{
    void **vtable = *(void ***)player;

    void (*func)(void *) = vtable[3];

    return func(player);
}

void finish_track_prediction_errors(void *player)
{
    void **vtable = *(void ***)player;

    void (*func)(void *) = vtable[4];

    return func(player);
}