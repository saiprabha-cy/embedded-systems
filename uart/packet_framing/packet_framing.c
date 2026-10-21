#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define SYNC1 0xAA
#define SYNC2 0x55
#define MAX_PAYLOAD 32
#define HEADER_SIZE 4
#define CRC_SIZE 2
#define MAX_FRAME (HEADER_SIZE + MAX_PAYLOAD + CRC_SIZE)

static uint16_t crc16_ccitt_false(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (unsigned bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (uint16_t)((crc << 1) ^ 0x1021);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}

/*
 * Frame:
 * [SYNC1][SYNC2][TYPE][LENGTH][PAYLOAD...][CRC_LO][CRC_HI]
 *
 * CRC covers TYPE, LENGTH, and PAYLOAD, not the sync bytes.
 */
static bool packet_encode(
    uint8_t type,
    const uint8_t *payload,
    uint8_t payload_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len)
{
    if (out == NULL || out_len == NULL ||
        payload_len > MAX_PAYLOAD ||
        (payload_len > 0 && payload == NULL)) {
        return false;
    }

    size_t total = HEADER_SIZE + payload_len + CRC_SIZE;

    if (out_capacity < total) {
        return false;
    }

    out[0] = SYNC1;
    out[1] = SYNC2;
    out[2] = type;
    out[3] = payload_len;

    for (size_t i = 0; i < payload_len; i++) {
        out[HEADER_SIZE + i] = payload[i];
    }

    uint16_t crc = crc16_ccitt_false(&out[2], 2U + payload_len);

    out[HEADER_SIZE + payload_len] = (uint8_t)(crc & 0xFF);
    out[HEADER_SIZE + payload_len + 1] = (uint8_t)(crc >> 8);

    *out_len = total;
    return true;
}

int main(void)
{
    const uint8_t payload[] = {0x10, 0x20, 0x30};
    uint8_t frame[MAX_FRAME];
    size_t frame_len = 0;

    if (!packet_encode(
            0x01, payload, sizeof(payload),
            frame, sizeof(frame), &frame_len)) {
        puts("ERROR: packet encoding failed");
        return 1;
    }

    printf("Encoded frame (%zu bytes):\n", frame_len);

    for (size_t i = 0; i < frame_len; i++) {
        printf("%02X ", frame[i]);
    }

    putchar('\n');
    return 0;
}