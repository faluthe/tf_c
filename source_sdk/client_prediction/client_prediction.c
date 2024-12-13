#include "client_prediction.h"

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