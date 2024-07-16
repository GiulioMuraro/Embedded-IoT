/*
 * led.h
 *
 *  Created on: 4 lug 2024
 *      Author: tomma
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void LED_init(void);
void LED_on(void);
void LED_off(void);
void LED_toggle(void);

#endif /* LED_H_ */
