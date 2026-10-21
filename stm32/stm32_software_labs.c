
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * STM32-01 to STM32-10
 * Host-side teaching simulator.
 *
 * IMPORTANT:
 * These are mock registers and simulated timer/interrupt events.
 * This program does not access physical STM32 hardware.
 */

/* ---------- Mock GPIO register block ---------- */

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
} MockGPIO;

static MockGPIO GPIOA = {0};

/* ---------- STM32-01: Register-level GPIO ---------- */

static void lab01_gpio_registers(void)
{
    /* PA5 mode: 01 = general-purpose output. */
    GPIOA.MODER &= ~(3U << (5U * 2U));
    GPIOA.MODER |=  (1U << (5U * 2U));

    /* Simulate writing BSRR to set PA5. */
    GPIOA.BSRR = (1U << 5U);
    GPIOA.ODR |= (1U << 5U);

    printf("\nSTM32-01 Register GPIO\n");
    printf("MODER = 0x%08lX\n", (unsigned long)GPIOA.MODER);
    printf("ODR after set = 0x%08lX\n",
           (unsigned long)GPIOA.ODR);

    /* Simulate writing BSRR upper half to reset PA5. */
    GPIOA.BSRR = (1U << (5U + 16U));
    GPIOA.ODR &= ~(1U << 5U);

    printf("ODR after reset = 0x%08lX\n",
           (unsigned long)GPIOA.ODR);
}

/* ---------- STM32-02: GPIO input ---------- */

static bool lab02_button_read(void)
{
    return (GPIOA.IDR & (1U << 0U)) != 0U;
}

static void lab02_gpio_input(void)
{
    /* Simulate PA0 input low, then input high. */
    GPIOA.IDR &= ~(1U << 0U);
    printf("\nSTM32-02 Button: %s\n",
           lab02_button_read() ? "PRESSED" : "RELEASED");

    GPIOA.IDR |= (1U << 0U);
    printf("Button: %s\n",
           lab02_button_read() ? "PRESSED" : "RELEASED");
}

/* ---------- STM32-03: Button debounce state machine ---------- */

typedef enum {
    BUTTON_RELEASED,
    BUTTON_DEBOUNCING_PRESS,
    BUTTON_PRESSED,
    BUTTON_DEBOUNCING_RELEASE
} ButtonState;

typedef struct {
    ButtonState state;
    uint32_t transition_ms;
    uint32_t debounce_ms;
} Button;

static void button_init(Button *b, uint32_t debounce_ms)
{
    b->state = BUTTON_RELEASED;
    b->transition_ms = 0U;
    b->debounce_ms = debounce_ms;
}

static void button_update(Button *b, bool raw_pressed, uint32_t now_ms)
{
    switch (b->state) {
    case BUTTON_RELEASED:
        if (raw_pressed) {
            b->state = BUTTON_DEBOUNCING_PRESS;
            b->transition_ms = now_ms;
        }
        break;

    case BUTTON_DEBOUNCING_PRESS:
        if (!raw_pressed) {
            b->state = BUTTON_RELEASED;
        } else if ((uint32_t)(now_ms - b->transition_ms)
                   >= b->debounce_ms) {
            b->state = BUTTON_PRESSED;
            puts("  Button press accepted");
        }
        break;

    case BUTTON_PRESSED:
        if (!raw_pressed) {
            b->state = BUTTON_DEBOUNCING_RELEASE;
            b->transition_ms = now_ms;
        }
        break;

    case BUTTON_DEBOUNCING_RELEASE:
        if (raw_pressed) {
            b->state = BUTTON_PRESSED;
        } else if ((uint32_t)(now_ms - b->transition_ms)
                   >= b->debounce_ms) {
            b->state = BUTTON_RELEASED;
            puts("  Button release accepted");
        }
        break;
    }
}

static void lab03_button_fsm(void)
{
    Button b;
    button_init(&b, 30U);

    puts("\nSTM32-03 Debounce FSM");

    /* Raw input remains high long enough to be accepted. */
    for (uint32_t t = 0U; t <= 50U; t += 10U) {
        button_update(&b, t >= 10U, t);
    }

    /* Raw input becomes low and remains low. */
    for (uint32_t t = 60U; t <= 100U; t += 10U) {
        button_update(&b, false, t);
    }
}

/* ---------- STM32-04 and STM32-05: Simulated timer ---------- */

typedef struct {
    volatile uint32_t CNT;
} MockTimer;

static MockTimer TIM2 = {0};

static uint32_t timer_now_ms(void)
{
    return TIM2.CNT;
}

static void simulated_timer_advance(uint32_t ms)
{
    /*
     * The simulator advances time explicitly.
     * On hardware, a peripheral timer advances independently.
     */
    TIM2.CNT += ms;
}

static void lab04_timer_delay(void)
{
    TIM2.CNT = 0U;

    uint32_t start = timer_now_ms();
    uint32_t delay_ms = 100U;

    /* Simulate the timer reaching its deadline. */
    simulated_timer_advance(delay_ms);

    if ((uint32_t)(timer_now_ms() - start) >= delay_ms) {
        puts("\nSTM32-04 Timer deadline reached: 100 ms");
    }
}

/* Simulated periodic timer interrupt callback. */
static volatile uint32_t system_tick_ms = 0U;

static void TIM2_IRQHandler_simulated(void)
{
    system_tick_ms++;
}

static void lab05_periodic_interrupt(void)
{
    system_tick_ms = 0U;

    puts("\nSTM32-05 Periodic timer interrupt");

    for (unsigned i = 0U; i < 5U; i++) {
        /* Simulate one hardware timer interrupt per millisecond. */
        TIM2_IRQHandler_simulated();
        printf("Tick = %lu ms\n",
               (unsigned long)system_tick_ms);
    }
}

/* ---------- STM32-06: PWM parameter calculation ---------- */

typedef struct {
    uint32_t timer_clock_hz;
    uint32_t prescaler_div;
    uint32_t counter_period;
    uint32_t duty_percent;
} PWMConfig;

static bool pwm_configure(PWMConfig *pwm,
                          uint32_t timer_clock_hz,
                          uint32_t target_counter_hz,
                          uint32_t target_pwm_hz,
                          uint32_t duty_percent)
{
    if (pwm == NULL ||
        timer_clock_hz == 0U ||
        target_counter_hz == 0U ||
        target_pwm_hz == 0U ||
        duty_percent > 100U ||
        timer_clock_hz % target_counter_hz != 0U ||
        target_counter_hz % target_pwm_hz != 0U) {
        return false;
    }

    uint32_t prescaler_div = timer_clock_hz / target_counter_hz;
    uint32_t period_counts = target_counter_hz / target_pwm_hz;

    if (prescaler_div == 0U || period_counts == 0U) {
        return false;
    }

    pwm->timer_clock_hz = timer_clock_hz;
    pwm->prescaler_div = prescaler_div;
    pwm->counter_period = period_counts;
    pwm->duty_percent = duty_percent;

    return true;
}

static void lab06_pwm(void)
{
    PWMConfig pwm;

    puts("\nSTM32-06 PWM configuration");

    if (pwm_configure(&pwm, 84000000U, 1000000U,
                      1000U, 25U)) {
        printf("Timer clock: %lu Hz\n",
               (unsigned long)pwm.timer_clock_hz);
        printf("Prescaler register PSC: %lu\n",
               (unsigned long)(pwm.prescaler_div - 1U));
        printf("Auto-reload register ARR: %lu\n",
               (unsigned long)(pwm.counter_period - 1U));
        printf("Duty cycle: %lu%%\n",
               (unsigned long)pwm.duty_percent);
    } else {
        puts("Invalid PWM configuration");
    }
}

/* ---------- STM32-07: PWM duty-cycle command ---------- */

static void lab07_pwm_command(void)
{
    PWMConfig pwm;

    puts("\nSTM32-07 PWM command controller");

    if (!pwm_configure(&pwm, 84000000U, 1000000U,
                       1000U, 25U)) {
        puts("PWM initialization failed");
        return;
    }

    /* Simulate receiving a command: DUTY 60 */
    uint32_t requested_duty = 60U;

    if (requested_duty <= 100U) {
        pwm.duty_percent = requested_duty;

        uint32_t compare =
            (pwm.counter_period * pwm.duty_percent) / 100U;

        printf("Command accepted: DUTY %lu\n",
               (unsigned long)requested_duty);
        printf("Simulated CCR = %lu\n",
               (unsigned long)compare);
    }
}

/* ---------- STM32-08: Interrupt-driven event queue ---------- */

#define EVENT_QUEUE_SIZE 8U

typedef enum {
    EVENT_BUTTON_PRESSED = 1,
    EVENT_UART_COMMAND = 2,
    EVENT_TIMER_TICK = 3
} EventType;

typedef struct {
    EventType items[EVENT_QUEUE_SIZE];
    volatile unsigned head;
    volatile unsigned tail;
    volatile unsigned dropped;
} EventQueue;

static EventQueue events = {0};

/*
 * Host simulation only. On a real target, the ISR producer and
 * main-loop consumer need target-appropriate synchronization.
 */
static bool event_post_from_isr(EventQueue *q, EventType event)
{
    unsigned next = (q->head + 1U) % EVENT_QUEUE_SIZE;

    if (next == q->tail) {
        q->dropped++;
        return false;
    }

    q->items[q->head] = event;
    q->head = next;
    return true;
}

static bool event_get(EventQueue *q, EventType *event)
{
    if (q->head == q->tail) {
        return false;
    }

    *event = q->items[q->tail];
    q->tail = (q->tail + 1U) % EVENT_QUEUE_SIZE;
    return true;
}

static void lab08_event_system(void)
{
    EventType event;

    puts("\nSTM32-08 Event system");

    /* Simulate events raised by interrupt handlers. */
    (void)event_post_from_isr(&events, EVENT_BUTTON_PRESSED);
    (void)event_post_from_isr(&events, EVENT_UART_COMMAND);
    (void)event_post_from_isr(&events, EVENT_TIMER_TICK);

    /* Application handles events outside interrupt context. */
    while (event_get(&events, &event)) {
        switch (event) {
        case EVENT_BUTTON_PRESSED:
            puts("Application: button event processed");
            break;
        case EVENT_UART_COMMAND:
            puts("Application: UART command processed");
            break;
        case EVENT_TIMER_TICK:
            puts("Application: timer event processed");
            break;
        default:
            puts("Application: unknown event");
            break;
        }
    }

    printf("Dropped events: %u\n", events.dropped);
}

/* ---------- STM32-09: Register abstraction ---------- */

#define GPIO_PIN(n) (1UL << (n))

static void gpio_set_pin(MockGPIO *gpio, unsigned pin)
{
    if (gpio != NULL && pin < 16U) {
        gpio->BSRR = GPIO_PIN(pin);
        gpio->ODR |= GPIO_PIN(pin);
    }
}

static void gpio_clear_pin(MockGPIO *gpio, unsigned pin)
{
    if (gpio != NULL && pin < 16U) {
        gpio->BSRR = GPIO_PIN(pin + 16U);
        gpio->ODR &= ~GPIO_PIN(pin);
    }
}

static bool gpio_read_pin(const MockGPIO *gpio, unsigned pin)
{
    if (gpio == NULL || pin >= 16U) {
        return false;
    }

    return (gpio->IDR & GPIO_PIN(pin)) != 0U;
}

static void lab09_register_abstraction(void)
{
    puts("\nSTM32-09 Register abstraction");

    gpio_set_pin(&GPIOA, 5U);
    printf("PA5 = %s\n",
           (GPIOA.ODR & GPIO_PIN(5U)) ? "HIGH" : "LOW");

    gpio_clear_pin(&GPIOA, 5U);
    printf("PA5 = %s\n",
           (GPIOA.ODR & GPIO_PIN(5U)) ? "HIGH" : "LOW");

    GPIOA.IDR |= GPIO_PIN(0U);
    printf("PA0 input = %s\n",
           gpio_read_pin(&GPIOA, 0U) ? "HIGH" : "LOW");
}

/* ---------- STM32-10: Bare-metal mini-framework ---------- */

typedef struct {
    bool initialized;
    uint32_t loop_count;
    uint32_t application_heartbeat;
} Firmware;

static void framework_init(Firmware *fw)
{
    memset(fw, 0, sizeof(*fw));

    /* Initialization order belongs here in a real application. */
    TIM2.CNT = 0U;
    system_tick_ms = 0U;
    events.head = 0U;
    events.tail = 0U;
    events.dropped = 0U;

    fw->initialized = true;
}

static void framework_step(Firmware *fw)
{
    if (fw == NULL || !fw->initialized) {
        return;
    }

    fw->loop_count++;

    /* Simulate asynchronous timer progression. */
    simulated_timer_advance(1U);
    TIM2_IRQHandler_simulated();

    /* A low-rate heartbeat task. */
    if ((system_tick_ms % 10U) == 0U) {
        fw->application_heartbeat++;
    }

    /* Drain events without doing heavy work in an ISR. */
    EventType event;

    while (event_get(&events, &event)) {
        switch (event) {
        case EVENT_BUTTON_PRESSED:
            puts("Framework: handle button event");
            break;
        case EVENT_UART_COMMAND:
            puts("Framework: handle UART command");
            break;
        case EVENT_TIMER_TICK:
            /* Periodic tick already accounted for. */
            break;
        default:
            break;
        }
    }
}

static void lab10_framework(void)
{
    Firmware fw;
    framework_init(&fw);

    (void)event_post_from_isr(&events, EVENT_BUTTON_PRESSED);
    (void)event_post_from_isr(&events, EVENT_UART_COMMAND);

    for (unsigned i = 0U; i < 30U; i++) {
        framework_step(&fw);
    }

    puts("\nSTM32-10 Mini-framework");
    printf("Initialized: %s\n",
           fw.initialized ? "yes" : "no");
    printf("Main-loop steps: %lu\n",
           (unsigned long)fw.loop_count);
    printf("Timer tick: %lu ms\n",
           (unsigned long)system_tick_ms);
    printf("Heartbeat count: %lu\n",
           (unsigned long)fw.application_heartbeat);
}

int main(void)
{
    puts("STM32 SOFTWARE LABS: 01-10");
    puts("Host-side mock simulation; not physical hardware.");

    lab01_gpio_registers();
    lab02_gpio_input();
    lab03_button_fsm();
    lab04_timer_delay();
    lab05_periodic_interrupt();
    lab06_pwm();
    lab07_pwm_command();
    lab08_event_system();
    lab09_register_abstraction();
    lab10_framework();

    puts("\nAll ten demonstrations executed.");
    return 0;
}
