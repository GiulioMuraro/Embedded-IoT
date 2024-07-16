#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include "lcd.h"
#include "led.h"
#include "HAL_OPT3001.h"
#include "HAL_TMP006.h"
#include "HAL_I2C.h"
#include "TMP36.h"
#include "VENTOLA.h"
#include "stdint.h"
#include "BUZZER.h"
#include "BUTTONS.h"
#include <assert.h>

// Variabili globali
float tempParete;
float lux;
volatile float tempAmbiente;
bool lightON;
bool fanON;
int jumpCounter;
bool buzzerON;
int stateB1, stateB2;

void _lightSensorInit();
void _temperatureSensorInit();
void _hwInit();
void printSensorValues();
void controlTemp();
void controlLight();
void controlBuzzer();
void PORT5_IRQHandler();
void PORT3_IRQHandler();

int main(void) {
    WDT_A_holdTimer();
    _hwInit();
    Interrupt_enableMaster();

    lightON = false;
    fanON = false;
    jumpCounter = 0;
    buzzerON = false;
    stateB1 = 0;
    stateB2 = 0;

    Graphics_drawStringCentered(&g_sContext, (int8_t *)"Temperature:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    // Valori di test fittizi
    float testTempPareteValues[] = {15.0, 25.0, 35.0, 45.0};
    float testTempAmbienteValues[] = {10.0, 20.0, 30.0, 40.0};
    float testLuxValues[] = {50.0, 150.0, 250.0, 350.0};
    int testIndex = 0;
    int testLength = 4;

    while (1) {
        tempParete = testTempPareteValues[testIndex];
        tempAmbiente = testTempAmbienteValues[testIndex];
        lux = testLuxValues[testIndex];

        // Verifiche con assert
        assert(tempParete >= -40 && tempParete <= 150);
        assert(tempAmbiente >= -40 && tempAmbiente <= 150);
        assert(lux >= 0);

        // Logica di controllo
        switch (stateB1) {
            case 0:
                if (tempParete < 36 - tempAmbiente || tempAmbiente < 16) {
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                    fanON = false;
                    buzzerON = true;
                } else if (tempParete < 50 - tempAmbiente || tempAmbiente < 32) {
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
                    fanON = false;
                    buzzerON = false;
                } else {
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
                    fanON = true;
                    buzzerON = false;
                }
                break;
            case 1:
                fanON = true;
                buzzerON = false;
                break;
            default:
                fanON = false;
                buzzerON = true;
                break;
        }

        switch (stateB2) {
            case 0:
                if (lux < 125.00) {
                    lightON = true;
                } else {
                    lightON = false;
                }
                break;
            case 1:
                lightON = true;
                break;
            default:
                lightON = false;
                break;
        }

        controlTemp();
        controlLight();
        controlBuzzer();

        Graphics_Rectangle rect = {40, 60, 120, 80};
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        Graphics_fillRectangle(&g_sContext, &rect);

        if (tempParete < 36 - tempAmbiente || tempAmbiente < 16) {
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        } else if (tempParete < 50 - tempAmbiente || tempAmbiente < 32) {
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        } else {
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        }

        char string[10];
        sprintf(string, "%.1f", tempAmbiente);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)string, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"C", AUTO_STRING_LENGTH, 100, 70, OPAQUE_TEXT);

        Graphics_Rectangle luxRect = {40, 90, 120, 110};
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        Graphics_fillRectangle(&g_sContext, &luxRect);

        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        char luxString[10];
        sprintf(luxString, "%.1f", lux);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)luxString, AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"lux", AUTO_STRING_LENGTH, 100, 100, OPAQUE_TEXT);

        printSensorValues();

        // Incremento l'indice di test per passare al set successivo di valori
        testIndex = (testIndex + 1) % testLength;

        __delay_cycles(3000000);
    }
}

void _lightSensorInit() {
    Init_I2C_GPIO();
    I2C_init();
    OPT3001_init();
    __delay_cycles(100000);
}

void _temperatureSensorInit() {
    Init_I2C_GPIO();
    I2C_init();
    TMP006_init();
    __delay_cycles(100000);
}

void _hwInit() {
    WDT_A_holdTimer();
    Interrupt_disableMaster();
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    graphicsInit();
    _temperatureSensorInit();
    _buzzerInit();
    _lightSensorInit();
    initTempSensor();
    LED_init();
    initVentola();
    configureButtons();
}

void printSensorValues() {
    printf("Lux: %.2f, TempPARETE: %.2f C, TempAMBIENTE: %.2f C, B1: %d, B2: %d\n", lux, tempParete, tempAmbiente, stateB1, stateB2);
}

void controlTemp() {
    if (fanON) {
        startVentola();
    } else {
        stopVentola();
    }
}

void controlLight() {
    if (lightON) {
        LED_on();
    } else {
        LED_off();
    }
}

void controlBuzzer() {
    if (buzzerON) {
        compareConfig_PWM.compareValue = 784;
    } else {
        compareConfig_PWM.compareValue = 0;
    }
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM);
}

void PORT5_IRQHandler() {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    if (status & GPIO_PIN1) {
        stateB1++;
        if (stateB1 >= 3) {
            stateB1 = 0;
        }
    }
}

void PORT3_IRQHandler() {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    if (status & GPIO_PIN5) {
        stateB2++;
        if (stateB2 >= 3) {
            stateB2 = 0;
        }
    }
}
