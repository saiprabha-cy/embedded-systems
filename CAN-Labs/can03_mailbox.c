
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define CAN_MAX_DATA 8U
#define MAILBOX_COUNT 3U
#define STD_ID_MAX 0x7FFU

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[CAN_MAX_DATA];
} CanFrame;

typedef enum {
    MAILBOX_FREE,
    MAILBOX_PENDING,
    MAILBOX_COMPLETE
} MailboxState;

typedef struct {
    MailboxState state;
    CanFrame frame;
} CanMailbox;

typedef struct {
    CanMailbox mailboxes[MAILBOX_COUNT];
} CanController;

static void controller_init(CanController *controller)
{
    for (size_t i = 0U; i < MAILBOX_COUNT; i++) {
        controller->mailboxes[i].state = MAILBOX_FREE;
    }
}

static int can_submit(CanController *controller,
                      const CanFrame *frame)
{
    if (controller == NULL || frame == NULL ||
        frame->id > STD_ID_MAX ||
        frame->dlc > CAN_MAX_DATA) {
        return -1;
    }

    for (size_t i = 0U; i < MAILBOX_COUNT; i++) {
        CanMailbox *box = &controller->mailboxes[i];

        if (box->state == MAILBOX_FREE ||
            box->state == MAILBOX_COMPLETE) {
            box->frame = *frame;
            box->state = MAILBOX_PENDING;

            printf("Frame 0x%03X queued in mailbox %u\n",
                   (unsigned int)frame->id,
                   (unsigned int)i);

            return (int)i;
        }
    }

    printf("Mailbox full: frame 0x%03X rejected\n",
           (unsigned int)frame->id);

    return -1;
}

static bool can_transmit_next(CanController *controller)
{
    for (size_t i = 0U; i < MAILBOX_COUNT; i++) {
        CanMailbox *box = &controller->mailboxes[i];

        if (box->state == MAILBOX_PENDING) {
            printf("Transmitting frame 0x%03X from mailbox %u\n",
                   (unsigned int)box->frame.id,
                   (unsigned int)i);

            box->state = MAILBOX_COMPLETE;
            return true;
        }
    }

    printf("No pending frames\n");
    return false;
}

static void show_mailboxes(const CanController *controller)
{
    printf("\nMailbox status:\n");

    for (size_t i = 0U; i < MAILBOX_COUNT; i++) {
        const char *name = "UNKNOWN";

        switch (controller->mailboxes[i].state) {
        case MAILBOX_FREE:     name = "FREE"; break;
        case MAILBOX_PENDING:  name = "PENDING"; break;
        case MAILBOX_COMPLETE: name = "COMPLETE"; break;
        }

        printf("  Mailbox %u: %s\n",
               (unsigned int)i, name);
    }
}

int main(void)
{
    CanController controller;
    controller_init(&controller);

    const CanFrame frames[] = {
        { .id = 0x100U, .dlc = 1U, .data = { 0xA1U } },
        { .id = 0x110U, .dlc = 1U, .data = { 0xB2U } },
        { .id = 0x120U, .dlc = 1U, .data = { 0xC3U } },
        { .id = 0x130U, .dlc = 1U, .data = { 0xD4U } }
    };

    printf("=== CAN-03: Mailbox Simulation ===\n");

    for (size_t i = 0U; i < 4U; i++) {
        (void)can_submit(&controller, &frames[i]);
    }

    show_mailboxes(&controller);

    printf("\n--- Complete one transmission ---\n");
    (void)can_transmit_next(&controller);

    printf("\n--- Submit the previously rejected frame ---\n");
    (void)can_submit(&controller, &frames[3]);

    show_mailboxes(&controller);

    return 0;
}
