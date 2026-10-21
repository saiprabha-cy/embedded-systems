#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define RX_BUFFER_SIZE 8

typedef struct {
    uint8_t data[RX_BUFFER_SIZE];
    volatile unsigned head;
    volatile unsigned tail;
    volatile unsigned dropped;
} RingBuffer;

static RingBuffer rx = {0};

static bool ring_push(RingBuffer *rb, uint8_t byte)
{
    unsigned next = (rb->head + 1U) % RX_BUFFER_SIZE;

    if (next == rb->tail) {
        rb->dropped++;
        return false;  /* Full: drop the new byte. */
    }

    rb->data[rb->head] = byte;
    rb->head = next;
    return true;
}

static bool ring_pop(RingBuffer *rb, uint8_t *byte)
{
    if (rb->tail == rb->head) {
        return false;  /* Empty. */
    }

    *byte = rb->data[rb->tail];
    rb->tail = (rb->tail + 1U) % RX_BUFFER_SIZE;
    return true;
}

int main(void)
{
    const char incoming[] = "ABC";

    /* Simulate bytes arriving from a UART interrupt. */
    for (unsigned i = 0; incoming[i] != '\0'; i++) {
        (void)ring_push(&rx, (uint8_t)incoming[i]);
    }

    /* Simulate application code consuming received bytes. */
    uint8_t byte;
    printf("Application received: ");

    while (ring_pop(&rx, &byte)) {
        putchar(byte);
    }

    printf("\nDropped bytes: %u\n", (unsigned)rx.dropped);
    return 0;
}