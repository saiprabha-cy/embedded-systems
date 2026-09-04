#include <stdio.h>
#include "error.h"

Status sensor_read(int *temperature)
{
    if (temperature == NULL)
    {
        return ERROR_INVALID_PARAMETER;
    }

    /*
     * Simulate successful sensor communication
     */

    *temperature = 27;

    return STATUS_OK;
}