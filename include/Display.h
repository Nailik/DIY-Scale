//
// Created by kilian on 01.08.2021.
//

#ifndef PROJECT_DISPLAY_H
#define PROJECT_DISPLAY_H

#include <Configuration.h>
#include <Adafruit_LEDBackpack.h>
#include <Logger.h>
#include <Pinout.h>

class Configuration;

/**
 * handles both displays together
 */
class Display {
private:
    //pointer to segments
    Adafruit_7segment *matrix1;
    Adafruit_7segment *matrix2;
    //store current number
    float current_number = std::numeric_limits<float>::max();
    //store current timer
    int current_timer = std::numeric_limits<int>::max();
    //if currently showing timer to get config update
    bool showing_timer = false;

    //fixes and prints on second display when d > 5
    void writeDigitNum(uint8_t d, uint8_t num, bool dot);

    //fixes and prints on second display when x > 5
    void writeDigitRaw(uint8_t x, uint16_t bitmask);

public:
    //constructor
    Display();

    //start pins
    void begin();

    //show a number (back) and the timer (front)
    void show(float number, int timer);

    //Display will only show this status code and no timer
    void showStatusCode(uint16_t code[], int codeSize);

    //update Brightness reads from configuration
    void updateBrightness();

    //blinking
    void blink(bool on);

    //clear display
    void clear();
};

#endif //PROJECT_DISPLAY_H
