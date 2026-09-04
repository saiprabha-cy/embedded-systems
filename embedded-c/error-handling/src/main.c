#include <stdio.h>
#include "error.h"

Status sensor_read(int *temperature);

int main(void)
{
    int temperature;

    Status status = sensor_read(&temperature);

    if (status == ERROR_TIMEOUT)
    {
        printf("Sensor temperature: %d C\n", temperature);
    }
    else
    {
        printf("Sensor read failed: %s\n",
               status_to_string(status));
    }

    return 0;
}