#include <stdint.h>

struct client_prediction
{
    void **vtable;
    __uint32_t last_ground;
    __uint8_t in_prediction;
    __uint8_t first_time_predicted;
    __uint8_t old_cl_predict_value;
    __uint8_t engine_paused;
};

void set_client_prediction_interface();
__uint8_t get_in_prediction();
void set_in_prediction(__uint8_t in_prediction);
__uint8_t get_first_time_predicted();
void set_first_time_predicted(__uint8_t first_time_predicted);
__uint8_t get_engine_paused();