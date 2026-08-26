#include <stdio.h>

typedef void (*callback_t)(void);

void sensor_event(void)
{
    printf("Sensor event occurred\n");
}

void telemetry_event(void)
{
    printf("Telemetry event occurred\n");
}

void register_callback(callback_t callback)
{
    callback();
}

int main(void)
{
    register_callback(sensor_event);
    register_callback(telemetry_event);

    return 0;
}