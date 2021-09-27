//
// Created by kilian on 01.08.2021.
//

#ifndef PROJECT_STATUS_H
#define PROJECT_STATUS_H

#include <SPI.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include <Adafruit_NeoPixel.h>
#include <Pinout.h>
#include <Logger.h>
#include <Configuration.h>
#include <Logic.h>

/**
 * Status Led (Button, Battery)
 */
class Status {
private:
    //object to use pixels
    static Adafruit_NeoPixel *pixels;
public:
    //initialize
    static void begin();

    //update led
    static void led(uint8_t id, boolean show);

    //update percentage led
    static void updateBatteryStatus(int percentage);

    //update brightness level
    static void updateBrightness();

    //turn all led off
    static void turnOff();
};

#endif //PROJECT_STATUS_H
