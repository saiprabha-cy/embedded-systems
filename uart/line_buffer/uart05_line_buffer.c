#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define LINE_CAPACITY 64

typedef struct {
    char data[LINE_CAPACITY];
    size_t length;
    bool ready;
    bool overflow;
} LineBuffer;

static void line_buffer_init(LineBuffer *line)
{
    memset(line, 0, sizeof(*line));
}

/* Feed one byte at a time, just as UART RX would deliver it. */
static void line_buffer_feed(LineBuffer *line, char ch)
{
    if (line->ready) {
        return; /* Consumer must take the completed line first. */
    }

    if (line->overflow) {
        /* Discard until the line terminator, then recover. */
        if (ch == '\n') {
            line->length = 0;
            line->overflow = false;
        }
        return;
    }

    if (ch == '\r') {
        /* Wait for LF to complete the required CRLF sequence. */
        return;
    }

    if (ch == '\n') {
        if (line->length > 0 && line->data[line->length - 1] == '\r') {
            line->data[--line->length] = '\0';
        }
        line->data[line->length] = '\0';
        line->ready = true;
        return;
    }

    if (line->length < LINE_CAPACITY - 1) {
        line->data[line->length++] = ch;
    } else {
        line->overflow = true;
    }
}

int main(void)
{
    LineBuffer line;
    line_buffer_init(&line);

    const char input[] = "STATUS\r\n";

    for (size_t i = 0; i < sizeof(input) - 1; i++) {
        line_buffer_feed(&line, input[i]);
    }

    if (line.ready) {
        printf("Complete line: [%s]\n", line.data);
        printf("Length: %zu\n", line.length);
    }

    return 0;
}