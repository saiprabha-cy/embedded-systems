#ifndef ERROR_H
#define ERROR_H

typedef enum
{
    STATUS_OK = 0,

    ERROR_INVALID_PARAMETER,
    ERROR_SENSOR_NOT_FOUND,
    ERROR_TIMEOUT,
    ERROR_CRC_FAILED,
    ERROR_BUFFER_FULL,
    ERROR_COMMUNICATION

} Status;

const char *status_to_string(Status status);

#endif