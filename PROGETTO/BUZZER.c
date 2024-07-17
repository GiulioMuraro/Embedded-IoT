/*
 * BUZZER.c
 *
 *  Created on: 13 lug 2024
 *      Author: tomma
 */
#include "BUZZER.h"

/* Timer_A Compare Configuration Parameter  (PWM) */
Timer_A_CompareModeConfig compareConfig_PWM = {
        TIMER_A_CAPTURECOMPARE_REGISTER_4,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        0                                        // 0% Duty Cycle initially
        };

/* Timer_A Up Configuration Parameter */
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 250 KhZ
        20000,                                  // 40 ms tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };

/* Tempo of the song in BPM */
int tempo = 120;

void _buzzerInit()
{
    /* Configures P2.7 to PM_TA0.4 for using Timer PWM to control the buzzer */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7,
                                                GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Timer_A0 for Up Mode and starting */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Initialize compare registers to generate PWM */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM); // For P2.7
}

void delay_ms(uint32_t ms)
{
    // Initialize Timer32
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);

    // Set the timer load value for the desired delay (in milliseconds)
    uint32_t loadValue = (3000 * ms); // 3 MHz clock
    Timer32_setCount(TIMER32_0_BASE, loadValue);

    // Start the timer
    Timer32_startTimer(TIMER32_0_BASE, true);

    // Wait until the timer expires
    while (Timer32_getValue(TIMER32_0_BASE) != 0);

    // Stop the timer
    Timer32_haltTimer(TIMER32_0_BASE);
}

void playTone(uint16_t frequency, uint16_t duration_ms)
{
    uint32_t timerPeriod = 250000 / frequency;
    compareConfig_PWM.compareValue = timerPeriod / 2;

    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM);
    TIMER_A0->CCR[0] = timerPeriod; // Set the period for Timer_A

    // Delay for the duration of the tone
    delay_ms(duration_ms);
}

void playSong()
{
    int melody[] = {
        NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 4, //1
        NOTE_E5, -4, NOTE_FS5, -4, NOTE_A5, 16, NOTE_G5, 16, NOTE_FS5, 8,
        NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 2,
        NOTE_A4, 16, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 8, NOTE_D5, 16,
        NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 4, //repeat from 1
        NOTE_E5, -4, NOTE_FS5, -4, NOTE_A5, 16, NOTE_G5, 16, NOTE_FS5, 8,
        NOTE_D5, -4, NOTE_E5, -4, NOTE_A4, 2,
        NOTE_A4, 16, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 8, NOTE_D5, 16,
        REST, 4, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_D5, 8, NOTE_E5, 8, NOTE_CS5, -8,
        NOTE_B4, 16, NOTE_A4, 2, REST, 4,

        REST, 8, NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 4, NOTE_A4, 8, //7
        NOTE_A5, 8, REST, 8, NOTE_A5, 8, NOTE_E5, -4, REST, 4,
        NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_D5, 8, NOTE_E5, 8, REST, 8,
        REST, 8, NOTE_CS5, 8, NOTE_B4, 8, NOTE_A4, -4, REST, 4,
        REST, 8, NOTE_B4, 8, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8, NOTE_A4, 4,
        NOTE_E5, 8, NOTE_E5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 4, REST, 4,

        NOTE_D5, 2, NOTE_E5, 8, NOTE_FS5, 8, NOTE_D5, 8, //13
        NOTE_E5, 8, NOTE_E5, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, 4, NOTE_A4, 4,
        REST, 2, NOTE_B4, 8, NOTE_CS5, 8, NOTE_D5, 8, NOTE_B4, 8,
        REST, 8, NOTE_E5, 8, NOTE_FS5, 8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
        NOTE_FS5, -8, NOTE_FS5, -8, NOTE_E5, -4, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,

        NOTE_E5, -8, NOTE_E5, -8, NOTE_D5, -8, NOTE_CS5, 16, NOTE_B4, -8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16, //18
        NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 8, NOTE_A4, 8, NOTE_A4, 8,
        NOTE_E5, 4, NOTE_D5, 2, NOTE_A4, -8, NOTE_A4, 16, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
        NOTE_A4, 4, REST, 2, NOTE_D5, 8, NOTE_D5, 16, NOTE_D5, 16, NOTE_E5, 16, NOTE_D5, 16,
        NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 8, NOTE_CS5, 16, NOTE_B4, -8, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16, //23
        NOTE_D5, 4, NOTE_E5, 8, NOTE_CS5, -8, NOTE_B4, 16, NOTE_A4, 8, NOTE_A4, 8, NOTE_A4, 8,
        NOTE_E5, 4, NOTE_D5, 2, NOTE_A4, -8, NOTE_A4, 16, NOTE_A4, 16, NOTE_B4, 16, NOTE_D5, 16, NOTE_B4, 16,
        NOTE_A4, 4, REST, 2
    };

    int noteDurations[] = {
        4, 4, 4, //1
        4, 4, 16, 16, 8,
        4, 4, 2,
        16, 16, 16, 8, 16,
        4, 4, 4, //repeat from 1
        4, 4, 16, 16, 8,
        4, 4, 2,
        16, 16, 16, 8, 16,
        4, 8, 8, 8, 8, 8, -8,
        16, 2, 4,

        8, 8, 8, 8, 8, 4, 8, //7
        8, 8, 8, -4, 4,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, -4, 4,
        8, 8, 8, 8, 8, 8, 4,
        8, 8, 8, 8, 4, 4,

        2, 8, 8, 8, //13
        8, 8, 8, 8, 4, 4,
        2, 8, 8, 8, 8,
        8, 8, 8, -4, 16, 16, 16, 16,
        -8, -8, -4, 16, 16, 16, 16,

        -8, -8, -8, 16, -8, 16, 16, 16, 16, //18
        4, 8, -8, 16, 8, 8, 8,
        4, 2, -8, 16, 16, 16, 16, 16,
        4, 2, 8, 16, 16, 16, 16,
        4, 4, 8, 16, -8, 16, 16, 16, 16, //23
        4, 8, -8, 16, 8, 8, 8,
        4, 2, -8, 16, 16, 16, 16, 16,
        4, 2
    };

    int numNotes = sizeof(melody) / sizeof(melody[0]);
    int i = 0;
    for (i = 0; i < numNotes; i++)
    {
        int noteDuration = 60000 / (tempo * noteDurations[i]);
        playTone(melody[i], noteDuration);

        // Wait for the duration of the note plus a short pause between notes
        int pauseBetweenNotes = noteDuration * 1.30;
        delay_ms(pauseBetweenNotes);
    }
}
