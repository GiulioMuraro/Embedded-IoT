# Embedded-IoT
This repository contains the code, documentation, and hardware design files for an Embedded IoT Multisensor System using the MSP432P401R microcontroller. The system is designed to monitor and control various environmental parameters in home application

| Members        | Mail |
|--|--|
| Giulio Muraro | giulio.muraro@studenti.unitn.it |
| Tommaso Santus | tommaso.santos@studenti.unitn.it |

<p align="center">
<img src="./docImages/???" height="500px">
</p>

## Table of contents
- [About this project](#about-this-project)
- [Documentation](#documentation)
  - [Libraries documentation](#libraries-documentation)
  - [Debugging](#debugging)
  - [Web Server](#web-server)
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


### Debugging


### TelegramBot server


<p align="right">(<a href="#top">Back to top</a>)</p>

## Known issues


<p align="right">(<a href="#top">Back to top</a>)</p>

## Project Layout

```
├── Code                    # PlatformIO project folder for the ATmega2560
|   ├── include             # All the header files
|   └── src                 # All the source code files
├── Script                   # Python script
└── TelegramBot              # Arduino IDE project folder
```
<p align="right">(<a href="#top">Back to top</a>)</p>

## Requirements
### Hardware Requirements
#### MCUs
* MSP432P401R:
* ESP8266: 

#### Sensors
* TMP006
* TMP36
* OPT3001
* Joystick

#### Actuators
* LED
* LCD
* Fan/ADC motor(simulate the cooling system)
* Buzzer(simulate the heating system)

#### Other components used
* Resistors(220 Ω)
* Transistor MOSFET(for the ADC motor)
* Battery(9V)
* Capacitors
* Breadboard, wires

### Software requirements
The software requirements are different for the two MCUs:

#### MSP432P401R:


#### ESP8266:


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

<p align="right">(<a href="#top">Back to top</a>)</p>

## Schematic

<p align="right">(<a href="#top">Back to top</a>)</p>

## Software architecture and Working Scheme
Once again, I need to distinguish the two MCUs as the software architecture is different:

### MSP432P401R


### ESP8266


<p align="right">(<a href="#top">Back to top</a>)</p>

## How to build burn and run
### ESP8266: 

### MSP432P4011R: 
