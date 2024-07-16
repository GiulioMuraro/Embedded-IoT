/*
 * led.c
 *
 *  Created on: 4 Jul 2024
 *      Author: tomma
 */

#include "led.h"

void LED_init(void) {
    // Configure pin P2.4 as output
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);
}

void LED_on(void) {
    // Turn on the LED (set the pin to high level)
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

void LED_off(void) {
    // Turn off the LED (set the pin to low level)
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

void LED_toggle(void) {
    // Toggle the LED state
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

