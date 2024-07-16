/*
 * VENTOLA.c
 *
 *  Created on: 11 Jul 2024
 *      Author: tomma
 */
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>
#include <VENTOLA.h>
#include "stdint.h"

void setup() {
    // Configure the pin P6.0 as output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);

    // Configure the debug LED on pin P1.0 (on-board LED on the MSP432 board)
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void initVentola(){
    // Set the system clock to 3 MHz
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    setup();
}

void startVentola(){
    // Turn on the motor and the debug LED
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN0);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void stopVentola(){
    // Turn off the motor and the debug LED
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
