#include "../../utils/utils.h"
#include "../math/vec3.h"
#include "estimate_abs_velocity.h"

#include <stdint.h>
#include <stdlib.h>

void (*estimate_abs_velocity)(void *, struct vec3_t *) = NULL;

bool init_estimate_abs_velocity()
{
    /* 0xe8 is the call opcode, the next 4 bytes are the relative address.
       This pattern is a call to EstimateAbsVelocity */
    __uint8_t pattern[] = { 0xe8, 0x0, 0x0, 0x0, 0x0, 0x49, 0x8b, 0x7f, 0x18 };
    void *scan_result = pattern_scan("client.so", pattern, "x????xxxx");

    if (!scan_result)
    {
        log_msg("Failed to find EstimateAbsVelocity pattern\n");
        return false;
    }

    __uint32_t rel_addr = *(__uint32_t *)((__uint64_t)scan_result + 1);
    __uint64_t abs_addr = (__uint64_t)scan_result + 5 + rel_addr;

    estimate_abs_velocity = (__typeof__(estimate_abs_velocity))abs_addr;

    log_msg("EstimateAbsVelocity found at %p\n", estimate_abs_velocity);
    return true;
}