
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define CAN_MAX_DATA 8U
#define STD_ID_MAX 0x7FFU
#define MAX_FAILURES 3U

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[CAN_MAX_DATA];
} CanFrame;

typedef enum {
    CAN_STATE_ACTIVE,
    CAN_STATE_BUS_OFF
} CanState;

typedef struct {
    CanState state;
    unsigned int consecutive_failures;
} CanDriver;

static bool frame_valid(const CanFrame *frame)
{
    return frame != NULL &&
           frame->id <= STD_ID_MAX &&
           frame->dlc <= CAN_MAX_DATA;
}

/* The caller supplies the result of a simulated transmission. */
static bool can_transmit_result(CanDriver *driver,
                                const CanFrame *frame,
                                bool simulated_success)
{
    if (driver == NULL || frame == NULL) {
        printf("ERROR: null argument\n");
        return false;
    }

    if (driver->state == CAN_STATE_BUS_OFF) {
        printf("REJECTED: controller is BUS-OFF\n");
        return false;
    }

    if (!frame_valid(frame)) {
        printf("ERROR: invalid frame; transmission rejected\n");
        return false;
    }

    if (simulated_success) {
        driver->consecutive_failures = 0U;
        printf("TX SUCCESS: ID=0x%03X\n",
               (unsigned int)frame->id);
        return true;
    }

    driver->consecutive_failures++;

    printf("TX FAILURE: ID=0x%03X, failure count=%u\n",
           (unsigned int)frame->id,
           driver->consecutive_failures);

    if (driver->consecutive_failures >= MAX_FAILURES) {
        driver->state = CAN_STATE_BUS_OFF;
        printf("STATE CHANGE: simulated BUS-OFF\n");
    }

    return false;
}

static void simulated_recovery(CanDriver *driver)
{
    if (driver == NULL) {
        return;
    }

    /*
     * Demonstration only. Real recovery must follow
     * the MCU/CAN controller's documented requirements.
     */
    driver->state = CAN_STATE_ACTIVE;
    driver->consecutive_failures = 0U;

    printf("SIMULATED RECOVERY: controller ACTIVE\n");
}

int main(void)
{
    CanDriver driver = {
        .state = CAN_STATE_ACTIVE,
        .consecutive_failures = 0U
    };

    const CanFrame good_frame = {
        .id = 0x120U,
        .dlc = 2U,
        .data = { 0x11U, 0x22U }
    };

    const CanFrame bad_frame = {
        .id = 0x800U,
        .dlc = 2U,
        .data = { 0x11U, 0x22U }
    };

    printf("=== CAN-05: Error Scenarios ===\n\n");

    (void)can_transmit_result(&driver, &bad_frame, true);

    printf("\n--- Simulate transmission failures ---\n");
    (void)can_transmit_result(&driver, &good_frame, false);
    (void)can_transmit_result(&driver, &good_frame, false);
    (void)can_transmit_result(&driver, &good_frame, false);

    printf("\n--- Attempt transmission while bus-off ---\n");
    (void)can_transmit_result(&driver, &good_frame, true);

    printf("\n--- Simulate recovery ---\n");
    simulated_recovery(&driver);

    printf("\n--- Verify operation after recovery ---\n");
    (void)can_transmit_result(&driver, &good_frame, true);

    return 0;
}
