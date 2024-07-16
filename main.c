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

/* Variable for storing temperature value returned from TMP006 */
float tempParete;
/* Variable for storing lux value returned from OPT3001 */
float lux;
/* Variable for storing temperature value returned from TMP36 */
volatile float tempAmbiente;
/* Variable for LED control*/
bool lightON;
/* Variable for fan control*/
bool fanON;
/* Cycles counter without control*/
int jumpCounter;
/* Variable for Buzzer*/
bool buzzerON;
/* State Variables for Buttons*/
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

/*
 * Main function -------------------------------------------------------------------------------------------------------------------------------
 */
int main(void)
{
    // Stop watchdog timer
    WDT_A_holdTimer();
    //Hardware init
    _hwInit();
    Interrupt_enableMaster();
    //Init variables
    lightON = false;
    fanON = false;
    jumpCounter = 0;
    buzzerON = false;
    stateB1 = 0;
    stateB2 = 0;

    Graphics_drawStringCentered(&g_sContext, (int8_t *)"Temperature:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    while (1)
    {
        /* Obtain temperature value from TMP006 */
        tempParete = (TMP006_getTemp()- 32) * 5 / 9;

        /* Obtain temperature value from TMP36 */
        tempAmbiente = TMP36_getTemp();

        /* Obtain lux value from OPT3001 */
        lux = OPT3001_getLux();

        /* Verify command status, otherwise controls data*/
        switch (stateB1) {
            case 0:
                // Caso in cui stateB1 è 0
                /* Set color based on temperature */
                if (tempParete < 36-tempAmbiente || tempAmbiente<16)//COLD
                {
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                    fanON=false;
                    buzzerON=true;
                }
                else if (tempParete < 50-tempAmbiente || tempAmbiente<32)//COMFORT
                {
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
                    fanON=false;
                    buzzerON=false;
                }
                else//HOT
                {
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
                    fanON=true;
                    buzzerON=false;
                }
                break;
            case 1:
                // Caso in cui stateB1 è 1
                fanON=true;
                buzzerON=false;
                break;
            default:
                // Caso in cui stateB1 non è 0 o 1
                fanON=false;
                buzzerON=true;
                break;
        }
        switch (stateB2) {
            case 0:
                if(lux<125.00){
                     lightON=true; //Bright
                }else{
                    lightON=false; //Dark
                }
                break;
            case 1:
                // Caso in cui stateB2 è 1
                lightON=true;
                break;
            default:
                // Caso in cui stateB2 non è 0 o 1
                lightON=false;
                break;
        }

        controlTemp();
        controlLight();
        controlBuzzer();

        /* Clear only the area where the temperature is displayed */
        Graphics_Rectangle rect = {40, 60, 120, 80};
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        Graphics_fillRectangle(&g_sContext, &rect);

        /* Reset foreground color for text */
        if (tempParete < 36-tempAmbiente || tempAmbiente<16)
        {
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        }
        else if (tempParete < 50-tempAmbiente || tempAmbiente<32)
        {
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
        }
        else
        {
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        }

        /* Display temperature */
        char string[10];
        sprintf(string, "%.1f", tempAmbiente);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)string, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

        /* Display "C" */
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"C", AUTO_STRING_LENGTH, 100, 70, OPAQUE_TEXT);

        /* Clear only the area where the lux is displayed */
        Graphics_Rectangle luxRect = {40, 90, 120, 110};
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        Graphics_fillRectangle(&g_sContext, &luxRect);

        /* Display lux in black */
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

        char luxString[10];
        sprintf(luxString, "%.1f", lux);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)luxString, AUTO_STRING_LENGTH, 64, 100, OPAQUE_TEXT);

        /* Display "lux" */
        Graphics_drawStringCentered(&g_sContext, (int8_t *)"lux", AUTO_STRING_LENGTH, 100, 100, OPAQUE_TEXT);


        //Print values
        printSensorValues(lux, tempParete, tempAmbiente, 0 );

        /* Small delay to avoid flickering */
        __delay_cycles(3000000);

    }
}
/*
 * End Main --------------------------------------------------------------------------------------------------------------------------------------------------------
 */


void _lightSensorInit()
{
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();

    /* Initialize OPT3001 digital ambient light sensor */
    OPT3001_init();

    __delay_cycles(100000);

}

void _temperatureSensorInit()
{
    /* Temperature Sensor initialization */
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();
    /* Initialize TMP006 temperature sensor */
    TMP006_init();
    __delay_cycles(100000);
}

void _hwInit()
{
    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1 */
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
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
    initVentola(); //Initialize the fan
    //micInit();
    configureButtons();
}

void printSensorValues()
{
    // Formatta i valori in una stringa
    printf("Lux: %.2f, TempPARETE: %.2f C, TempAMBIENTE: %.2f C, B1: %d, B2: %d\n", lux, tempParete, tempAmbiente, stateB1, stateB2);
}

void controlTemp(){
    //Controls Fan
    if(fanON){
        startVentola();
    }else{
        stopVentola();
    }
}

void controlLight(){
    //Controls LED
    if(lightON){
         LED_on();
    }else{
        LED_off();
    }
}

void controlBuzzer(){
    if(buzzerON){
        compareConfig_PWM.compareValue = 784;
    }else{
        compareConfig_PWM.compareValue = 0;
    }
    /* Update buzzer PWM */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM);
}

void PORT5_IRQHandler()
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    printf("INTERRUPT P5\n");
    if (status & GPIO_PIN1) {
        stateB1++;
        if(stateB1>=3){
            stateB1=0;
        }
    }
}

void PORT3_IRQHandler()
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    printf("INTERRUPT P3\n");
    if (status & GPIO_PIN5) {
        stateB2++;
        if(stateB2>=3){
            stateB2=0;
        }
    }
}
