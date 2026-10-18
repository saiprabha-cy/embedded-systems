#ifndef SENSOR_H
#define SENSOR_H

#include "hal_i2c.h"

typedef enum
{
    SENSOR_OK = 0,
    SENSOR_ERROR,
    SENSOR_INVALID_PARAMETER
} Sensor_Status;

Sensor_Status sensor_read_temperature(float *temperature);

#endif