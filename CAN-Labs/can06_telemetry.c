
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CAN_MAX_DATA 8U
#define TELEMETRY_ID 0x120U
#define STD_ID_MAX 0x7FFU

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[CAN_MAX_DATA];
} CanFrame;

typedef struct {
    float temperature_c;
    float voltage_v;
    float current_a;
    uint8_t status;
    uint8_t sequence;
} Telemetry;

static void write_u16_le(uint8_t *out, uint16_t value)
{
    out[0] = (uint8_t)(value & 0xFFU);
    out[1] = (uint8_t)(value >> 8);
}

static uint16_t read_u16_le(const uint8_t *in)
{
    return (uint16_t)((uint16_t)in[0] |
                      ((uint16_t)in[1] << 8));
}

static void write_i16_le(uint8_t *out, int16_t value)
{
    uint16_t bits;
    memcpy(&bits, &value, sizeof bits);
    write_u16_le(out, bits);
}

static int16_t read_i16_le(const uint8_t *in)
{
    uint16_t bits = read_u16_le(in);
    int16_t value;

    memcpy(&value, &bits, sizeof value);
    return value;
}

static bool telemetry_encode(const Telemetry *t,
                             CanFrame *frame)
{
    if (t == NULL || frame == NULL) {
        return false;
    }

    /* Scale physical values into integer representation. */
    float temp_scaled = t->temperature_c * 100.0f;
    float voltage_scaled = t->voltage_v * 100.0f;
    float current_scaled = t->current_a * 100.0f;

    if (temp_scaled < -32768.0f ||
        temp_scaled > 32767.0f ||
        voltage_scaled < 0.0f ||
        voltage_scaled > 65535.0f ||
        current_scaled < -32768.0f ||
        current_scaled > 32767.0f) {
        return false;
    }

    int16_t temp_raw = (int16_t)temp_scaled;
    uint16_t voltage_raw = (uint16_t)voltage_scaled;
    int16_t current_raw = (int16_t)current_scaled;

    frame->id = TELEMETRY_ID;
    frame->dlc = 8U;

    write_i16_le(&frame->data[0], temp_raw);
    write_u16_le(&frame->data[2], voltage_raw);
    write_i16_le(&frame->data[4], current_raw);
    frame->data[6] = t->status;
    frame->data[7] = t->sequence;

    return true;
}

static bool telemetry_decode(const CanFrame *frame,
                             Telemetry *t)
{
    if (frame == NULL || t == NULL ||
        frame->id != TELEMETRY_ID ||
        frame->id > STD_ID_MAX ||
        frame->dlc != 8U) {
        return false;
    }

    int16_t temp_raw = read_i16_le(&frame->data[0]);
    uint16_t voltage_raw = read_u16_le(&frame->data[2]);
    int16_t current_raw = read_i16_le(&frame->data[4]);

    t->temperature_c = (float)temp_raw / 100.0f;
    t->voltage_v = (float)voltage_raw / 100.0f;
    t->current_a = (float)current_raw / 100.0f;
    t->status = frame->data[6];
    t->sequence = frame->data[7];

    return true;
}

static void print_frame(const CanFrame *frame)
{
    printf("CAN ID: 0x%03X, DLC: %u\n",
           (unsigned int)frame->id,
           (unsigned int)frame->dlc);

    printf("Payload: ");
    for (uint8_t i = 0U; i < frame->dlc; i++) {
        printf("%02X ", (unsigned int)frame->data[i]);
    }
    printf("\n");
}

int main(void)
{
    const Telemetry tx = {
        .temperature_c = 25.50f,
        .voltage_v = 12.34f,
        .current_a = -1.25f,
        .status = 0x05U,
        .sequence = 42U
    };

    CanFrame frame = {0};
    Telemetry rx = {0};

    printf("=== CAN-06: Telemetry Format ===\n\n");

    if (!telemetry_encode(&tx, &frame)) {
        printf("ERROR: telemetry encoding failed\n");
        return 1;
    }

    print_frame(&frame);

    if (!telemetry_decode(&frame, &rx)) {
        printf("ERROR: telemetry decoding failed\n");
        return 1;
    }

    printf("\nDecoded telemetry:\n");
    printf("Temperature: %.2f C\n", rx.temperature_c);
    printf("Voltage:     %.2f V\n", rx.voltage_v);
    printf("Current:     %.2f A\n", rx.current_a);
    printf("Status:      0x%02X\n", (unsigned int)rx.status);
    printf("Sequence:    %u\n", (unsigned int)rx.sequence);

    return 0;
}
