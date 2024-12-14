#include "client_prediction.h"
#include "../../utils/utils.h"

#include <stddef.h>

static struct client_prediction *interface = NULL;

void set_client_prediction_interface(void *client_prediction_interface)
{
    interface = (struct client_prediction *)client_prediction_interface;
}

__uint8_t get_in_prediction()
{
    return interface->in_prediction;
}

void set_in_prediction(__uint8_t in_prediction)
{
    interface->in_prediction = in_prediction;
}

__uint8_t get_first_time_predicted()
{
    return interface->first_time_predicted;
}

void set_first_time_predicted(__uint8_t first_time_predicted)
{
    interface->first_time_predicted = first_time_predicted;
}

__uint8_t get_engine_paused()
{
    return interface->engine_paused;
}

void reset_instance_counters()
{
    static void (*reset_instance_counters)(void) = NULL;

    if (reset_instance_counters == NULL)
    {
        // Pattern: E8 ?? ?? ?? ?? 4C 89 A3 ?? ?? ?? ?? 4C 89 E7 E8 ?? ?? ?? ??
        __uint8_t pattern[] = { 0xE8, 0x0, 0x0, 0x0, 0x0, 0x4C, 0x89, 0xA3, 0x0, 0x0, 0x0, 0x0, 0x4C, 0x89, 0xE7, 0xE8, 0x0, 0x0, 0x0, 0x0 };
        void *scan_result = pattern_scan("client.so", pattern, "x????xxxx????xxxx????");

        if (!scan_result)
        {
            log_msg("Failed to find ResetInstanceCounters pattern\n");
            return;
        }

        __uint32_t rel_addr = *(__uint32_t *)((__uint64_t)scan_result + 1);
        __uint64_t abs_addr = (__uint64_t)scan_result + 5 + rel_addr;

        reset_instance_counters = (__typeof__(reset_instance_counters))abs_addr;

        log_msg("ResetInstanceCounters found at %p\n", reset_instance_counters);
    }

    reset_instance_counters();
}

// void setup_move(void *player, struct user_cmd *user_cmd, struct move_data *move_data, void *move_helper)
// {
//     void **vtable = *(void ***)interface;

//     void (*func)(void *, void *, struct user_cmd *, struct move_data *, void *) = vtable[19];

//     func(interface, player, user_cmd, move_data, move_helper);
// }

void setup_move(void *player, struct user_cmd *user_cmd, void *move_helper, struct move_data *move_data)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, void *, struct user_cmd *, void *, struct move_data *) = vtable[19];

    func(interface, player, user_cmd, move_helper, move_data);
}

void finish_move(void *player, struct user_cmd *user_cmd, struct move_data *move_data)
{
    void **vtable = *(void ***)interface;

    void (*func)(void *, void *, struct user_cmd *, struct move_data *) = vtable[20];

    func(interface, player, user_cmd, move_data);
}
