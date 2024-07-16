/*
 * lcd.h
 *
 *  Created on: 4 lug 2024
 *      Author: tomma
 */

#ifndef LCD_H
#define LCD_H

#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include "stdint.h"

extern Graphics_Context g_sContext;

void graphicsInit();

#endif // LCD_H
