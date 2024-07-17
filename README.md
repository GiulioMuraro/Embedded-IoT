# Embedded-IoT
This repository contains the code, documentation, and hardware design files for an Embedded IoT Multisensor System using the MSP432P401R microcontroller. The system is designed to monitor and control various environmental parameters in home application

| Members        | Mail |
|--|--|
| Giulio Muraro | giulio.muraro@studenti.unitn.it | 218544
| Tommaso Santus | tommaso.santos@studenti.unitn.it | 226666

<p align="center">
<img src="./docImages/???" height="500px">
</p>

## Table of contents
- [About this project](#about-this-project)
- [Documentation](#documentation)
  - [Libraries documentation](#libraries-documentation)
  - [Telegram bot Server](#web-server)
- [Known issues](#known-issues)
- [Project Layout](#project-layout)
- [Requirements](#requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software requirements](#software-requirements)
- [Schematic](#schematic)
- [Software architecture and Working Scheme](#software-architecture-and-working-scheme)
- [How to build, burn and run](#How-to-build-burn-and-run)

## About this project
The MSP432P401R microcontroller will be used in the Embedded IoT Multisensor System project to monitor and control a variety of environmental conditions. With its numerous sensors for measuring temperature (both internal and external), brightness, and noise levels, the system offers a complete solution for preserving ideal environmental conditions in a domestic setting. This project, which draws inspiration from the ideas of thermal comfort,which is the condition of mind that expresses satisfaction with the thermal environment and is influenced by various factors such as temperature, humidity, and air movement. Maintaining a comfortable indoor climate is essential for well-being, productivity, and energy efficiency.

The system's central component, the MSP432P401R microcontroller, interfaces with a range of sensors and actuators to collect information and react to changes in the surrounding environment. The ESP8266 WiFi module uses wireless to comunicate with the APIs of Telegram and manages the TelegramBot, that help in monitoring and controlling your home. The data should pass through a serial port, from the MSP432 to the ESP8266.

The [PowerPoint presentation](./presentationProject.pdf) for the oral exam is in the main folder.

The pitch video demonstration can be found at <a href="">here</a>.
<p align="right">(<a href="#top">Back to top</a>)</p>

## Documentation
### Libraries documentation
The [documentation](./doc.md) can be found in the main folder

See [Software architecture/Scheme](#software-architecture-and-working-scheme) for the most important information.


### TelegramBot server
The Telegram bot server folder contains the main code to run the bot("tgUniversalBot.ino") and a library of useful methods("utils-tg-bot.h/ino").
The WiFi credentials are directly hard coded inside the tgUniversalBot.ino. Obviously is not secure enough and it should be changed. Just using a separate file for the credentials will do.

<p align="right">(<a href="#top">Back to top</a>)</p>

## Known issues
- Communication problems to establish stable connection between between the MSP432 and the ESP8266:
  * Firstly the idea was to create a system of two boards connected to a WLAN and let them comunicate using HTTP request to exchange data. We ordered two unsoldered ESP8266, so the signal connection via UART(with TX, RX pins) couldn't happen durably, since the boards keep disconnecting from the WiFi.
  * Secondly, we decided to move ourself to a more simple way of transmission. We tried to use the serial port of the MSP to let it comunicate with its ESP8266(Wifi Module). The link was working just for the ESP, but from and to the MSP nothing passed.
  * Currently the project doesn't permit the two boards to comunicate, but the code of the ESP8266 is suitable for communication(API and Serial)
- Buttons flickering: We noted that, sometimes when we press a button(B1, B2) the Interrupt handler is called multiple times. So the button state is changed different times and the result is inconsistent
- 
<p align="right">(<a href="#top">Back to top</a>)</p>

## Project Layout

```
├── Code                    # PlatformIO project folder for the ATmega2560
|   ├── include             # All the header files
|   └── src                 # All the source code files
├── Script                   # Python script
└── tgBotUniversal              # Arduino IDE project folder
    ├── tgBotUniversal.ino             # the telegram bot server code
    ├── utils-tg-bot.h                 # Header file of utils
    └── utils-tg-bot.h                 # Source file of utils
```
<p align="right">(<a href="#top">Back to top</a>)</p>

## Requirements
### Hardware Requirements
#### MCUs
* MSP432P401R: We used the MSP432P401R board with the additional Educational BoosterPackXL MKII
* ESP8266: Generic ESP module

#### Sensors
* TMP006
* TMP36
* OPT3001
* Joystick

#### Actuators
* LED
* LCD
* Fan/DC motor(simulate the cooling system)
* Buzzer(simulate the heating system)

#### Other components used
* Resistors(220 Ω)
* Transistor MOSFET(for the DC motor)
* Battery(9V)
* Capacitors
* Breadboard, wires

### Software requirements
The software requirements are different for the two MCUs:

#### MSP432P401R:
We used CCS to code the MSP board without Virtual Machine. The libraries needs to be added to the Compiler/Linker options in the "Project"->"Show build settings..." and insert the following libraries to "Include options" of the compiler and "Search file path" for the ARM linker.

We used the following libraries:
```
#include <msp.h>
#include <driverlib.h>
#include <grlib.h>

#include <stdio.h>
#include "stdint.h"

#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include "lcd.h"
#include "led.h"
#include "HAL_OPT3001.h"
#include "HAL_TMP006.h"
#include "HAL_I2C.h"
#include "TMP36.h"
#include "VENTOLA.h"
#include "BUZZER.h"
#include "BUTTONS.h"
```
The file location of the same libraries could be different, pay attention.


#### ESP8266:
We used Arduino IDE for the ESP, since I needed to use the libraries to host the web server and to comunicate with "api.telegram.com". First of all, to add the ESP boards to the IDE it is necessary to add this URL to the settings on the "Additional boards manager URLs": http://arduino.esp8266.com/stable/package_esp8266com_index.json.

We used the following libraries:
```
// Manage the Web Server
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <UniversalTelegramBot.h>

// Manage acquiring date and time
#include <NTPClient.h>
#include <WiFiUdp.h>

// Manage the filesystem of the ESP8266
#include <FS.h>
#include <LittleFS.h>
```

<p align="right">(<a href="#top">Back to top</a>)</p><    

## Software architecture and Working Scheme
Once again, I need to distinguish the two MCUs as the software architecture is different:

### MSP432P401R


### ESP8266
First, it initializes by setting up serial communication for debugging and mounting the LittleFS file system to store data. To ensure a clean start for testing, it clears the JSON file contents.

Next, it connects to the WiFi network using predefined credentials (WIFI_SSID and WIFI_PASSWORD). This connection is crucial for enabling communication with Telegram’s servers.

For secure communication, the server configures a secured_client with the fingerprint of the Telegram API server (FINGERPRINT_TG). This step ensures that data exchanges with Telegram remain safe and reliable.

To handle incoming data and commands, the bot starts an HTTP web server on port 80. It defines a specific handler (handleDataReadings) for processing POST requests sent to /upload, where it reads JSON data, parses it, and stores it in a file adding the current Date and time(so the MSP can just focus on sending the raw readings).

In the main loop, it updates the time from an NTP server and listens for incoming HTTP requests. Periodically, it checks for new messages from Telegram. When new messages arrive, it validates the sender’s chat_id against a list of permitted IDs (chat_id_permitted). If authorized, it responds to commands like /help, /getAllData, /getTodayInfo, /testFan, /testServo, /testAlarm, /testLight, and /start.


<p align="right">(<a href="#top">Back to top</a>)</p>
