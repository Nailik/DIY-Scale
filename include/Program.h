//
// Created by kilian on 08.09.2021.
//

#ifndef PROJECT_PROGRAM_H
#define PROJECT_PROGRAM_H

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//current software version, shown on webpage
#define CURRENT_VERSION 1.0
//if code should print log
#define DEBUG_LOG_ENABLED 1
//disables Sensor tare functionality so code runs without hardware
#define DEBUG_DISABLE_TARE 1

#include <Configuration.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <OtaConnection.h>
#include <OtaServer.h>
#include <Button.h>
#include <Display.h>
#include <Sensor.h>
#include <Status.h>
#include <Bluetooth.h>
#include <Sound.h>
#include <BleCallbacks.h>
#include <Timer.h>
#include <Logic.h>

class Configuration;

class Bluetooth;

class Logic;

class Timer;

class Program {
public:
    //handles logic (weight, display, tare)
    static Logic *logic;
    //used to connect bluetooth app
    static Bluetooth *bluetooth;

    //default setup
    static void setup();

    //default loop
    static void loop();
};

#endif //PROJECT_PROGRAM_H
