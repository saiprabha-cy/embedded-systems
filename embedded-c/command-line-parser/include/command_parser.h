#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>

#define COMMAND_MAX_LENGTH 32

typedef enum
{
    CMD_UNKNOWN,
    CMD_SET_LED,
    CMD_GET_TEMP,
    CMD_RESET
} CommandType;

typedef struct
{
    CommandType type;
    int value;
} Command;

int command_parse(const char *input, Command *cmd);

#endif /* COMMAND_PARSER_H */