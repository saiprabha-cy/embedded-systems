#include <stdint.h>

uint16_t clamp_battery_mv(uint32_t input_mv)
{
    if (input_mv > 10000U) {
        return 10000U;
    }

    return (uint16_t)input_mv;
}
