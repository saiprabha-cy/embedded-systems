#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>

typedef enum
{
    HAL_OK = 0,
    HAL_ERROR,
    HAL_TIMEOUT
} HAL_Status;

HAL_Status hal_i2c_read(uint8_t device_address,
                        uint8_t register_address,
                        uint8_t *data,
                        uint8_t length);

#endif