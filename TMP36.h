/*
 * TMP36.h
 *
 *  Created on: 10 Jul 2024
 *      Author: tomma
 */

#ifndef TMP36_H_
#define TMP36_H_

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>
#include <stdint.h>

// PWM Configuration Structure
extern Timer_A_PWMConfig pwmConfig;

// Initialization functions
void initADC(void);
void initTempSensor(void);

// Reading functions
uint16_t readADC(void);
float getAverageADC(uint8_t samples);
float TMP36_getTemp(void);

#endif /* TMP36_H_ */
