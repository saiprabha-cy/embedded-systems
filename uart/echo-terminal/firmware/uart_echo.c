#include <stdio.h>
#include <stdint.h>

static uint8_t simulated_rx_data[] = {
    'H', 'E', 'L', 'L', 'O', '\n'
};

static uint32_t rx_index = 0;

uint8_t UART_ReadByte(void)
{
    return simulated_rx_data[rx_index++];
}

void UART_SendByte(uint8_t data)
{
    printf("%c", data);
}

void UART_Echo(void)
{
    uint8_t data;

    data = UART_ReadByte();

    UART_SendByte(data);
}

int main(void)
{
    for (int i = 0; i < 6; i++)
    {
        UART_Echo();
    }

    return 0;
}