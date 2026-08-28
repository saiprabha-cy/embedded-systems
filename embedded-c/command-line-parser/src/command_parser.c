#include "command_parser.h"

#include <stdio.h>
#include <string.h>

int command_parse(const char *input, Command *cmd)
{
    /*if (input == NULL || cmd == NULL)
    {
        return -1;
    }*/

    //cmd->type = CMD_UNKNOWN;
    //cmd->value = 0;

    /* SET LED <value> */
    if (sscanf(input, "SET LED %d", &cmd->value) == 1)
    {
        cmd->type = CMD_SET_LED;
        return 0;
    }

    /* GET TEMP */
    if (strcmp(input, "GET TEMP") == 0)
    {
        cmd->type = CMD_GET_TEMP;
        return 0;
    }

    /* RESET */
    if (strcmp(input, "RESET") == 0)
    {
        cmd->type = CMD_RESET;
        return 0;
    }

    return -1;
}