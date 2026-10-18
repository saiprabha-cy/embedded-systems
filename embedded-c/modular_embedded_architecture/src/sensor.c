#include "sensor.h"
#include <stddef.h>

#define SENSOR_I2C_ADDRESS       0x48
#define SENSOR_TEMP_REGISTER     0x00

Sensor_Status sensor_read_temperature(float *temperature)
{
    uint8_t raw_data[2];

    if (temperature == NULL)
    {
        return SENSOR_INVALID_PARAMETER;
    }

    HAL_Status status =
        hal_i2c_read(
            SENSOR_I2C_ADDRESS,
            SENSOR_TEMP_REGISTER,
            raw_data,
            2
        );

    if (status != HAL_OK)
    {
        return SENSOR_ERROR;
    }

    uint16_t raw_value =
        ((uint16_t)raw_data[0] << 8) |
        raw_data[1];

    *temperature = raw_value / 100.0f;

    return SENSOR_OK;
}