# Project Documentation

# buzzer.c

## Introduction
This documentation covers the implementation of a simple buzzer controller using Timer_A and Timer32 modules. The main functionalities include initializing the buzzer, generating delays, and playing tones at specified frequencies and durations.

## Data Structures

### `Timer_A_CompareModeConfig`
A configuration structure used to set up the compare mode for Timer_A.

- **Fields**:
  - `uint16_t register`: Specifies the capture/compare register to use.
  - `uint16_t interruptEnable`: Enables or disables the CCR interrupt.
  - `uint16_t outputMode`: Sets the output mode.
  - `uint16_t compareValue`: Initial duty cycle (percentage of timer period).

### `Timer_A_UpModeConfig`
A configuration structure used to set up Timer_A in up mode.

- **Fields**:
  - `uint16_t clockSource`: Clock source for the timer.
  - `uint16_t clockSourceDivider`: Divider for the clock source.
  - `uint16_t timerPeriod`: Period for the timer ticks.
  - `uint16_t timerInterruptEnable_TAIE`: Enables or disables the Timer_A interrupt.
  - `uint16_t captureCompareInterruptEnable_CCR0_CCIE`: Enables or disables the CCR0 interrupt.
  - `uint16_t timerClear`: Clears the timer value.

## Functions

### `void _buzzerInit()`
Initializes the buzzer by configuring the relevant GPIO pin and setting up Timer_A for PWM control.

- **Description**:
  - Configures P2.7 to PM_TA0.4 for using Timer PWM to control the buzzer.
  - Configures Timer_A0 for Up Mode and starts it.
  - Initializes compare registers to generate PWM for P2.7.

### `void delay_ms(uint32_t ms)`
Generates a delay for a specified number of milliseconds using Timer32.

- **Parameters**:
  - `uint32_t ms`: Number of milliseconds to delay.

- **Description**:
  - Initializes Timer32.
  - Sets the timer load value for the desired delay.
  - Starts the timer and waits until it expires.
  - Stops the timer.

### `void playTone(uint16_t frequency, uint16_t duration_ms)`
Plays a tone at a specified frequency for a specified duration.

- **Parameters**:
  - `uint16_t frequency`: Frequency of the tone to play.
  - `uint16_t duration_ms`: Duration of the tone in milliseconds.

- **Description**:
  - Calculates the timer period based on the frequency.
  - Sets the compare value for the desired duty cycle.
  - Sets the period for Timer_A.
  - Delays for the duration of the tone.




# TMP36.h
## Timer_A_PWMConfig pwmConfig
A configuration structure for the Timer_A module to generate PWM signals.

### Fields
- **clockSource**: The clock source for Timer_A. Example: `TIMER_A_CLOCKSOURCE_SMCLK`
- **clockSourceDivider**: Divider for the clock source. Example: `TIMER_A_CLOCKSOURCE_DIVIDER_1`
- **timerPeriod**: Period of the timer, determining the PWM frequency. Example: `48000` (500ms period)
- **compareRegister**: Capture/Compare register. Example: `TIMER_A_CAPTURECOMPARE_REGISTER_1`
- **compareOutputMode**: Output mode for PWM. Example: `TIMER_A_OUTPUTMODE_SET_RESET`
- **dutyCycle**: Initial duty cycle. Example: `4800` (10% duty cycle)

## void initADC(void)
Initializes the Analog-to-Digital Converter (ADC) for the MSP432P401R to read data from the TMP36 sensor.

### Description
- Enables the ADC module.
- Configures the ADC module clock source to MCLK.
- Sets the sample hold trigger.
- Configures pin P5.5 as an analog input (A0).
- Configures the ADC for single sample mode.
- Sets up the ADC memory for conversion.
- Enables interrupts for ADC conversion.
- Enables ADC conversions.

## uint16_t readADC(void)
Starts an ADC conversion and returns the read value.

### Description
- Triggers an ADC conversion.
- Waits for the conversion to complete.
- Returns the ADC conversion result.

### Returns
- **uint16_t**: The result of the ADC conversion.

## float getAverageADC(uint8_t samples)
Calculates the average value of multiple ADC readings.

### Parameters
- **samples**: The number of samples to average.

### Description
- Takes multiple ADC readings.
- Calculates and returns the average of the readings.

### Returns
- **float**: The average ADC value from the specified number of samples.

## void initTempSensor(void)
Initializes the TMP36 temperature sensor.

### Description
- Initializes the ADC.
- Configures PWM using Timer_A for a 500ms period with an initial 10% duty cycle.

## float TMP36_getTemp(void)
Gets the temperature from the TMP36 sensor.

### Description
- Reads the average of 10 ADC samples.
- Converts the ADC value to voltage.
- Calculates and returns the temperature in degrees Celsius.

### Returns
- **float**: The temperature in degrees Celsius.


# fan.c

## Introduction
This documentation covers the implementation of a fan controller using GPIO pins and clock signals on the MSP432 platform. The main functionalities include initializing the fan setup, starting the fan, and stopping the fan.

## Files
- **VENTOLA.c**: Contains the implementation of the fan control functions.

## Functions

### `void setup()`
Configures the GPIO pins for the fan motor and the debug LED as output pins and initializes them to a low state.

- **Description**:
  - Configures P6.0 as an output pin for controlling the fan motor.
  - Configures P1.0 as an output pin for controlling the on-board debug LED.
  - Sets the initial state of both pins to low (off).

### `void initVentola()`
Initializes the system clock and calls the `setup` function to configure the GPIO pins.

- **Description**:
  - Sets the system clock to 3 MHz.
  - Configures the main clock (MCLK) and the sub-main clock (SMCLK) to use the DCO (Digitally Controlled Oscillator) at 3 MHz with no clock divider.
  - Calls the `setup` function to configure the GPIO pins for the fan motor and the debug LED.

### `void startVentola()`
Turns on the fan motor and the debug LED.

- **Description**:
  - Sets the output state of P6.0 to high, turning on the fan motor.
  - Sets the output state of P1.0 to high, turning on the debug LED.

### `void stopVentola()`
Turns off the fan motor and the debug LED.

- **Description**:
  - Sets the output state of P6.0 to low, turning off the fan motor.
  - Sets the output state of P1.0 to low, turning off the debug LED.



# led.c

## Introduction
This documentation covers the implementation of LED control using GPIO pins on the MSP432 platform. The main functionalities include initializing the LED, turning it on, turning it off, and toggling its state.

## Files
- **led.c**: Contains the implementation of the LED control functions.

## Functions

### `void LED_init(void)`
Initializes the GPIO pin for controlling the LED as an output pin.

- **Description**:
  - Configures pin P2.4 as an output pin to control the LED.

### `void LED_on(void)`
Turns on the LED by setting the GPIO pin high.

- **Description**:
  - Sets the output state of pin P2.4 to high, turning on the LED.

### `void LED_off(void)`
Turns off the LED by setting the GPIO pin low.

- **Description**:
  - Sets the output state of pin P2.4 to low, turning off the LED.

### `void LED_toggle(void)`
Toggles the state of the LED.

- **Description**:
  - Toggles the output state of pin P2.4, changing the LED state from on to off or vice versa.
