#include <stdio.h>
#include "sensor.h"

int main(void)
{
    float temperature;

    Sensor_Status status =
        sensor_read_temperature(&temperature);

    if (status == SENSOR_OK)
    {
        printf("Temperature: %.2f C\n", temperature);
    }
    else
    {
        printf("Sensor read failed: %d\n", status);
    }

    return 0;
}