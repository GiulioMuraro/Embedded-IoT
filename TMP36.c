/*
 * TMP36.c
 *
 *  Created on: 10 Jul 2024
 *      Author: tomma
 */

#include "TMP36.h"

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,
    TIMER_A_CLOCKSOURCE_DIVIDER_1,
    48000, // 500ms period
    TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A_OUTPUTMODE_SET_RESET,
    4800 // Initial 10% duty cycle
};

// Function to initialize the ADC
void initADC(void) {
    // Enable power to the ADC
    ADC14_enableModule();

    // Configure the ADC module (MCLK/1/1)
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    // Configure the sample hold trigger
    ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);

    // Configure pin P5.5 as analog input (A0)
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    // Configure the ADC for single sample mode
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);

    // Enable interrupt for ADC_MEM0
    ADC14_enableInterrupt(ADC_INT0);

    // Enable conversions
    ADC14_enableConversion();
}

// Function to start an ADC conversion and return the read value
uint16_t readADC(void) {
    // Start an ADC conversion
    ADC14_toggleConversionTrigger();

    // Wait for the conversion to complete
    while (!(ADC14_getInterruptStatus() & ADC_INT0));

    // Read the converted value
    return ADC14_getResult(ADC_MEM0);
}

// Function to calculate the average of ADC readings
float getAverageADC(uint8_t samples) {
    uint32_t sum = 0;
    uint8_t i = 0;
    for (i = 0; i < samples; i++) {
        sum += readADC();
        __delay_cycles(3000); // Short delay between readings
    }
    return (float)sum / samples;
}

// Function to initialize the temperature sensor
void initTempSensor(void) {
    // Configure the ADC
    initADC();

    // Configure PWM using Timer_A
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}

// Function to get the temperature from the TMP36 sensor
float TMP36_getTemp(void) {
    float adcValue = getAverageADC(10); // Read the average of 10 ADC samples
    //printf("adcValue: %.2f\n", adcValue);

    // Convert the ADC value to voltage (V)
    float voltage = (adcValue * 3.3) / 16384.0;
    //printf("voltage: %.3f\n", voltage);

    // Calculate the temperature in degrees Celsius
    float temperature = (voltage - 0.5) * 100;
    //printf("temperature: %.2f\n", temperature);

    return temperature;
}
