#include "hal_i2c.h"
#include <stddef.h>

HAL_Status hal_i2c_read(uint8_t device_address,
                        uint8_t register_address,
                        uint8_t *data,
                        uint8_t length)
{
    (void)device_address;
    (void)register_address;

    if (data == NULL || length == 0)
    {
        return HAL_ERROR;
    }

    /*
     * Simulated hardware response.
     * Suppose the sensor returns raw value 2500.
     */

    data[0] = 0x09;
    data[1] = 0xC4;

    return HAL_OK;
}