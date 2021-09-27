//
// Created by kilian on 01.08.2021.
//

/**
 * saves all HUZZAH 32 pins for easy lookup and also whats connected to it in this project
 */

#ifndef PROJECT_PINOUT_H
#define PROJECT_PINOUT_H

#define PIN_STATUS_LED      13 //A12
#define PIN_SOUND           26 //A0
#define PIN_BTN_TARA        27 //A10
#define PIN_BTN_TARA_LED    12 //A11
#define PIN_BTN_TIMER       25 //A1
#define PIN_BTN_TIMER_LED   4  //A5
#define PIN_SENSOR_0        32 //A7 left sensor (close at esp)
#define PIN_SENSOR_1        33 //A9 right sensor
#define PIN_HX711_CLOCK_0   14 //A6
#define PIN_HX711_CLOCK_1   14 //A8

#define ADDRESS_DISPLAY_0   0x70
#define ADDRESS_DISPLAY_1   0x71

#define IND_BTN_TARE_STATUS_LED    0
#define IND_BTN_BAT_LED            1
#define IND_BTN_TIMER_STATUS_LED   2

#define BAT_PIN 35 //A13
#define BAT_MIN 3000 //mV
#define BAT_MAX 4200 //mV

// https://de.decentespresso.com/scale_api
#define CMD_TARE            0x10
#define CMD_DISPLAY_ON      0xED
#define CMD_DISPLAY_OFF     0xEE

#endif //PROJECT_PINOUT_H