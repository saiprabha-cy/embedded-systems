#include <stdio.h>
#include <stdint.h>

#define TELEMETRY_VERSION 1U

int main(void)
{
    uint32_t sequence = 1U;
    int16_t temperature_centi_c = 2534;
    uint16_t battery_mv = 7420U;

    printf("Telemetry firmware simulator\n");
    printf("Version: %u\n", TELEMETRY_VERSION);
    printf("Sequence: %u\n", sequence);
    printf("Temperature: %d.%02d C\n",
           temperature_centi_c / 100,
           temperature_centi_c < 0
               ? -(temperature_centi_c % 100)
               : temperature_centi_c % 100);
    printf("Battery: %u mV\n", battery_mv);

    return 0;
}
