#include <stdio.h>

#include "command_parser.h"

static void execute_command(const Command *cmd)
{
    switch (cmd->type)
    {
        case CMD_SET_LED:
            printf("Executing: SET LED %d\n", cmd->value);
            break;

        case CMD_GET_TEMP:
            printf("Executing: GET TEMP\n");
            break;

        case CMD_RESET:
            printf("Executing: RESET\n");
            break;

        default:
            printf("Executing: UNKNOWN COMMAND\n");
            break;
    }
}

int main(void)
{
    const char *commands[] =
    {
        "SET LED 1",
        "GET TEMP",
        "RESET",
        "INVALID"
    };

    for (size_t i = 0; i < 4; i++)
    {
        Command cmd;

        printf("\nInput: %s\n", commands[i]);

        if (command_parse(commands[i], &cmd) == 0)
        {
            printf("Command parsed successfully\n");
            execute_command(&cmd);
        }
        else
        {
            printf("Command parse failed\n");
        }
    }

    return 0;
}