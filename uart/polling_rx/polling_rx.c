#include <stdio.h>

static int uart_receive_char(void)
{
    /*
     * PC simulation: getchar() represents one received character.
     * EOF represents the end of input.
     */
    return getchar();
}

int main(void)
{
    printf("UART-02: Enter characters, then press Enter.\n");
    printf("Received: ");

    int ch;

    while ((ch = uart_receive_char()) != EOF) {
        putchar(ch);

        if (ch == '\n') {
            break;
        }
    }

    printf("\nReception complete.\n");
    return 0;
}