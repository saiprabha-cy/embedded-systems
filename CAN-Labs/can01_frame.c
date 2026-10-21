
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CAN_MAX_DATA 8U
#define CAN_STD_ID_MAX 0x7FFU
#define CAN_EXT_ID_MAX 0x1FFFFFFFU

typedef enum {
    CAN_STANDARD = 0,
    CAN_EXTENDED = 1
} CanIdFormat;

typedef struct {
    uint32_t id;
    CanIdFormat format;
    uint8_t dlc;
    uint8_t data[CAN_MAX_DATA];
} CanFrame;

static bool can_frame_valid(const CanFrame *frame)
{
    if (frame == NULL) {
        return false;
    }

    if (frame->format == CAN_STANDARD) {
        if (frame->id > CAN_STD_ID_MAX) {
            return false;
        }
    } else if (frame->format == CAN_EXTENDED) {
        if (frame->id > CAN_EXT_ID_MAX) {
            return false;
        }
    } else {
        return false;
    }

    if (frame->dlc > CAN_MAX_DATA) {
        return false;
    }

    return true;
}

static void can_frame_print(const CanFrame *frame)
{
    if (!can_frame_valid(frame)) {
        printf("ERROR: invalid CAN frame\n");
        return;
    }

    printf("Format: %s\n",
           frame->format == CAN_STANDARD
               ? "Standard (11-bit)"
               : "Extended (29-bit)");

    printf("ID:     0x%0*X\n",
           frame->format == CAN_STANDARD ? 3 : 8,
           (unsigned int)frame->id);

    printf("DLC:    %u\n", (unsigned int)frame->dlc);
    printf("Data:   ");

    for (uint8_t i = 0U; i < frame->dlc; i++) {
        printf("%02X ", (unsigned int)frame->data[i]);
    }

    if (frame->dlc == 0U) {
        printf("(no payload)");
    }

    printf("\n");
}

static CanFrame make_frame(uint32_t id,
                           CanIdFormat format,
                           const uint8_t *data,
                           uint8_t length)
{
    CanFrame frame = {0};

    frame.id = id;
    frame.format = format;
    frame.dlc = length;

    if (data != NULL && length <= CAN_MAX_DATA) {
        memcpy(frame.data, data, length);
    }

    return frame;
}

int main(void)
{
    const uint8_t telemetry[] = {
        0x2A, 0x01, 0xF4, 0x00
    };

    CanFrame standard = make_frame(
        0x120U, CAN_STANDARD,
        telemetry, sizeof telemetry);

    CanFrame extended = make_frame(
        0x18FF50E5U, CAN_EXTENDED,
        telemetry, sizeof telemetry);

    CanFrame invalid = make_frame(
        0x800U, CAN_STANDARD,
        telemetry, sizeof telemetry);

    printf("=== CAN-01: Frame Generator ===\n\n");

    can_frame_print(&standard);
    printf("\n");

    can_frame_print(&extended);
    printf("\n");

    can_frame_print(&invalid);

    return 0;
}
