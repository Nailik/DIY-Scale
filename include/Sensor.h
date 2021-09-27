//
// Created by kilian on 01.08.2021.
//

/**
 * handles the weight cells and the HX711
 */

#ifndef PROJECT_SENSOR_H
#define PROJECT_SENSOR_H

#include <Program.h>
#include <cstdint>
#include <HX711.h>
#include <Logger.h>

/**
 * one hx711 connected sensor
 */
class Sensor {
private:
    //the hx711 device
    HX711 scale;
    //pin for hx711
    uint8_t pin;
    //clock pin
    uint8_t clock_pin;
    //calibration factor
    double calibration_factor;
public:
    //constructor to initialize fields
    Sensor(uint8_t pin, uint8_t clock_pin, double calibration_factor);

    //use pin
    void begin();

    //tare (blocking)
    void tare();

    //update calibration factor
    void setCalibrationFactor(double calibration_factor);

    //get current weight
    float getWeight();

    //get value multiple times
    double getValue(int times);
};


#endif //PROJECT_SENSOR_H
