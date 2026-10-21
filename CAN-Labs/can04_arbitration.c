
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define STD_ID_MAX 0x7FFU
#define MAX_NODES 8U

typedef struct {
    const char *name;
    uint16_t id;
    bool active;
} CanNode;

static int arbitrate(CanNode nodes[], size_t count)
{
    for (size_t i = 0U; i < count; i++) {
        if (nodes[i].id > STD_ID_MAX) {
            printf("Invalid standard ID for node %s\n",
                   nodes[i].name);
            return -1;
        }
        nodes[i].active = true;
    }

    printf("Bit-by-bit arbitration (bit 10 to bit 0)\n\n");

    for (int bit = 10; bit >= 0; bit--) {
        unsigned int bus_bit = 1U;

        /* Any active node sending dominant 0 makes bus = 0. */
        for (size_t i = 0U; i < count; i++) {
            if (nodes[i].active &&
                ((nodes[i].id >> bit) & 1U) == 0U) {
                bus_bit = 0U;
            }
        }

        printf("Bit %2d: bus=%u | ", bit, bus_bit);

        for (size_t i = 0U; i < count; i++) {
            if (!nodes[i].active) {
                continue;
            }

            unsigned int sent_bit =
                (nodes[i].id >> bit) & 1U;

            printf("%s sends %u; ",
                   nodes[i].name, sent_bit);

            if (sent_bit == 1U && bus_bit == 0U) {
                nodes[i].active = false;
                printf("%s loses; ", nodes[i].name);
            }
        }

        printf("\n");
    }

    for (size_t i = 0U; i < count; i++) {
        if (nodes[i].active) {
            printf("\nWinner: %s, ID=0x%03X\n",
                   nodes[i].name,
                   (unsigned int)nodes[i].id);
            return (int)i;
        }
    }

    return -1;
}

int main(void)
{
    CanNode nodes[MAX_NODES] = {
        { "FlightComputer", 0x300U, false },
        { "PowerMonitor",   0x100U, false },
        { "ThermalUnit",    0x180U, false }
    };

    const size_t count = 3U;

    printf("=== CAN-04: Arbitration Simulation ===\n\n");
    (void)arbitrate(nodes, count);

    return 0;
}
