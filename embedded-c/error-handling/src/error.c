#include "error.h"

const char *status_to_string(Status status)
{
    switch (status)
    {
        case STATUS_OK:
            return "OK";

        case ERROR_INVALID_PARAMETER:
            return "INVALID_PARAMETER";

        case ERROR_SENSOR_NOT_FOUND:
            return "SENSOR_NOT_FOUND";

        case ERROR_TIMEOUT:
            return "TIMEOUT";

        case ERROR_CRC_FAILED:
            return "CRC_FAILED";

        case ERROR_BUFFER_FULL:
            return "BUFFER_FULL";

        case ERROR_COMMUNICATION:
            return "COMMUNICATION_ERROR";

        default:
            return "UNKNOWN_ERROR";
    }
}