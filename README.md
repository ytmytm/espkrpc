kRPC client for ESP8266
=======================
Maciej Witkowiak <mwitkowiak@gmail.com>, <ytm@elysium.pl>

This is a client for [kRPC](https://krpc.github.io/krpc/index.html), a mod for [Kerbal Space Program](https://kerbalspaceprogram.com) game.

It allows you to build Internet-of-Kerbals with tiny WiFi-enabled clients for remote monitoring and control of Kerbalkind aerospace vehicles.

This is a reimplementation of kRPC C++ client adapted to ESP8266 environment with low RAM usage.

Instead of reference Google Protobuf library I used [Nanopb](https://github.com/nanopb/nanopb).

This version is based on kRPC-0.3.6 protocol.

# What is...

## Kerbal Space Program
An awesome game where you fail and fail again, and then you crash and burn.

## kRPC
A mod for KSP that allows to control what happens in the game from outside scripts written in popular languages like Python, Lua, C# and C++.
"RPC" stands for "Remote Procedure Call" and the connection is provided by a TCP/IP server running on computer with the game. The scripts may
run on remote machines and connect to the game over network.

## ESP8266
Incredible platform for custom amateur Internet-of-Things development. This is a very low cost WiFi chip with full TCP/IP stack and microcontroller.

# Possible usage scenarios

- remote monitoring of KSP flight parameters to show on custom physical displays (LEDs, LCDs, custom meters, etc.)
- remote control of KSP flight with custom physical controls (joysticks, buttons, knobs, etc.)
- interface between higher-level kRPC protocol over WiFi and simple serial communication with AVR Arduinos (KSP Serial IO on steroids)
- translator between Internet-of-Things (think MQTT protocol) and Kerbal-of-Things universe

# Requirements

## Software
- Arduino IDE 1.6.x
- [ESP8266 core for Arduino](https://github.com/esp8266/Arduino)
- Kerbal Space Program game with kRPC mod installed

## Hardware

This was developed and tested on [Wemos D1 mini](https://www.wemos.cc/product/d1-mini.html) board. Basically this is ESP-12F module put on a board
with integrated serial to USB interface.

You can find more information how to get started with Arduino on [Wemos pages](https://www.wemos.cc/tutorial/get-started-arduino.html).

# Current status

At the moment this is just a proof of concept - to show that it is possible to do at all. The example code is a direct reimplementation of kRPC tutorial script. 
This code sends a rocket to space, until it reaches 100000m apoapsis and brings back safely to Kerbin.

I implemented only those RPC calls that are actually used. There should be enough of flash space left to put majority of calls from all possible services defined by kRPC.

My C++ wrapper over Nanopb is clumsy, but at least it hides all the ugly callbacks for each of the possible variable types.

The object model for SpaceCenter service is a joke, it is just a thin wrapper over kRPC calls. It's your responsibility to keep track of vessel, control, flight, orbit references.

I learn as I code, so I would also welcome any suggestions how to improve the code also with C++11 features in mind.

# How to rebuld krpc.pb.[ch] from krpc.proto

Just follow instructions from Nanopb

Update the copy of [krpc.proto file from kRPC repository](https://github.com/krpc/krpc/blob/master/protobuf/krpc.proto)

Compile .proto file to krpc.pb file with `protoc` compiler that comes with Google Protobuf
```
protoc -okrpc.pb krpc.proto
```
Then process it with nanopb generator:
```
nanopb/generator/nanopb_generator.py krpc.pb
```
