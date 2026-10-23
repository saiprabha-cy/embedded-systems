#include <assert.h>
#include <stdio.h>
#include "telemetry.h"

int main(void)
{
    assert(clamp_battery_mv(7420U) == 7420U);
    assert(clamp_battery_mv(10000U) == 10000U);
    assert(clamp_battery_mv(12000U) == 10000U);
    assert(clamp_battery_mv(0U) == 0U);

    puts("PASS: normal battery reading");
    puts("PASS: maximum boundary");
    puts("PASS: upper clamp");
    puts("PASS: zero input");

    puts("ALL TESTS PASSED");
    return 0;
}
