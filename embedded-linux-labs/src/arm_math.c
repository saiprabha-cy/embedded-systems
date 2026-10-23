#include <stdint.h>

volatile uint32_t result;

int main(void)
{
    uint32_t a = 120U;
    uint32_t b = 30U;

    result = a + b;
    result = result * 2U;

    while (1) {
        /* Embedded main loop placeholder. */
    }
}
