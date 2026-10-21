
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define STD_ID_MAX 0x7FFU

typedef struct {
    uint32_t id;
    uint32_t mask;
} CanFilter;

static bool can_filter_accepts(const CanFilter *filter,
                               uint32_t received_id)
{
    if (filter == NULL ||
        filter->id > STD_ID_MAX ||
        received_id > STD_ID_MAX ||
        filter->mask > STD_ID_MAX) {
        return false;
    }

    return (received_id & filter->mask) ==
           (filter->id & filter->mask);
}

int main(void)
{
    const CanFilter telemetry_filter = {
        .id = 0x120U,
        .mask = 0x7F0U
    };

    const uint32_t incoming_ids[] = {
        0x120U,
        0x123U,
        0x12FU,
        0x130U,
        0x200U,
        0x121U
    };

    const size_t count =
        sizeof incoming_ids / sizeof incoming_ids[0];

    printf("=== CAN-02: Identifier Filtering ===\n");
    printf("Filter ID = 0x%03X, Mask = 0x%03X\n\n",
           (unsigned int)telemetry_filter.id,
           (unsigned int)telemetry_filter.mask);

    for (size_t i = 0U; i < count; i++) {
        const uint32_t id = incoming_ids[i];

        printf("ID 0x%03X: %s\n",
               (unsigned int)id,
               can_filter_accepts(&telemetry_filter, id)
                   ? "ACCEPT"
                   : "REJECT");
    }

    return 0;
}
