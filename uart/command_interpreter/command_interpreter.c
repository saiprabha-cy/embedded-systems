#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static bool led_on = false;

static void trim_and_uppercase(char *s)
{
    size_t start = 0;
    while (isspace((unsigned char)s[start])) {
        start++;
    }

    if (start > 0) {
        memmove(s, s + start, strlen(s + start) + 1);
    }

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }

    for (size_t i = 0; s[i] != '\0'; i++) {
        s[i] = (char)toupper((unsigned char)s[i]);
    }
}

static void process_command(char *command)
{
    trim_and_uppercase(command);

    if (strcmp(command, "LED ON") == 0) {
        led_on = true;
        puts("OK: LED ON");
    } else if (strcmp(command, "LED OFF") == 0) {
        led_on = false;
        puts("OK: LED OFF");
    } else if (strcmp(command, "STATUS") == 0) {
        printf("STATUS: LED=%s\n", led_on ? "ON" : "OFF");
    } else if (strcmp(command, "RESET") == 0) {
        puts("RESET REQUEST RECEIVED (simulation only)");
    } else if (command[0] == '\0') {
        puts("ERROR: EMPTY COMMAND");
    } else {
        puts("ERROR: UNKNOWN COMMAND");
    }
}

int main(void)
{
    char command[64];

    puts("UART-06 Command Interpreter");
    puts("Commands: LED ON, LED OFF, STATUS, RESET");
    puts("Enter one command per line.");

    while (fgets(command, sizeof(command), stdin) != NULL) {
        /* Remove trailing CR/LF characters. */
        command[strcspn(command, "\r\n")] = '\0';
        process_command(command);
    }

    return 0;
}