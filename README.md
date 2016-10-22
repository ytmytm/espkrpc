kRPC client for ESP8266
=======================
Maciej Witkowiak <mwitkowiak@gmail.com>, <ytm@elysium.pl>

This is [Kerbal Space Program](https://kerbalspaceprogram.com) mod [kRPC](https://krpc.github.io/krpc/index.html) client.
It allows you to build Internet-of-Kerbals tiny clients for remote monitoring and control of Kerbalkind aerospace vehicles.

This is a reimplementation of kRPC C++ client stripped down and adapted to ESP8266 environment with just 80K of RAM available
for application code.

Instead of reference Google Protobuf library I used [Nanopb](https://github.com/nanopb/nanopb). Nanopb is very friendly for RAM-starved platforms.

This version is based on kRPC-0.3.6 protocol.

# Possible usage scenarios

- remote monitoring of KSP flight parameters to show on custom physical displays (LEDs, LCDs, custom meters, etc.)
- remote control of KSP flight with custom physical controls (joysticks, buttons, knobs, etc.)
- interface between higher-level kRPC protocol over WiFi and simple serial communication with AVR Arduinos (KSP Serial IO on steroids)

# Requirements

## Software
- Arduino IDE 1.6.x
- ESP8266 core for Arduino
- Kerbal Space Program game with kRPC mod installed

## Hardware

This was developed and tested on [Wemos D1 mini](](https://www.wemos.cc/product/d1-mini.html)) board.
You can find more information how to get started with Arduino on [Wemos pages](https://www.wemos.cc/tutorial/get-started-arduino.html).

# Current status

At the moment this is just a proof of concept that it works at all. The example code is a direct reimplementation of kRPC tutorial script. 
This code sends a rocket to space, until it reaches 100000m apoapsis and brings back safely to Kerbin.

I implemented only those RPC calls that are actually used. There should be enough of flash space left to put majority of calls from all possible services defined by kRPC.

My C++ wrapper over Nanopb is clumsy, but at least it hides all the ugly callbacks for each of the possible variable types.

The object model for SpaceCenter service is a joke, it is just a thin wrapper over kRPC calls. It's your responsibility to keep track of vessel, control, flight, orbit references.

I learn as I code, so I would welcome any suggestions how to improve the code also with C++11 features in mind.
