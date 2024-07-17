/*
 * BUZZER.h
 *
 *  Created on: 13 lug 2024
 *      Author: tomma
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>

/* Note frequencies in Hz */
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_A4 440
#define NOTE_FS5 740
#define NOTE_A5 880
#define NOTE_G5 784
#define NOTE_B4 494
#define NOTE_CS5 554
#define REST 0

/* Tempo of the song in BPM */
extern int tempo;

extern Timer_A_CompareModeConfig compareConfig_PWM;

void _buzzerInit();
void delay_ms(uint32_t ms);
void playTone(uint16_t frequency, uint16_t duration_ms);
void playSong();

#endif /* BUZZER_H_ */
