#include <stdio.h>

static void uart_send_char(char ch)
{
    /* PC simulation: stdout represents the UART TX terminal. */
    putchar((unsigned char)ch);
}

static void uart_send_string(const char *str)
{
    while (*str != '\0') {
        uart_send_char(*str);
        str++;
    }
}

int main(void)
{
    uart_send_string("UART-01: Polling TX\r\n");
    uart_send_string("Flight computer booted.\r\n");
    uart_send_string("Telemetry link ready.\r\n");

    return 0;
}