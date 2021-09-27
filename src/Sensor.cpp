//
// Created by kilian on 01.08.2021.
//

#include <Sensor.h>

/**
 * one hx711 connected sensor
 */

/**
 * sets up pins
 * @param pin data pin
 * @param clock_pin clk pin
 * @param calibration_factor current calibration factor
 */
Sensor::Sensor(uint8_t pin, uint8_t clock_pin, double calibration_factor) {
    this->pin = pin;
    this->clock_pin = clock_pin;
    this->calibration_factor = calibration_factor;
}

/**
 * use pin, will not finish when hx711 is not connected correctly
 */
void Sensor::begin() {
    Logger::log("sen begin");

    scale.begin(pin, clock_pin);
    scale.set_scale((float) calibration_factor);
    scale.set_gain(64); //works better
}

/**
 * tare this sensor
 */
void Sensor::tare() {
    Logger::log("sen tare");

    if(!DEBUG_DISABLE_TARE) {
        scale.tare();
    }
}

/**
 * update calibration factor
 * @param new_calibration_factor calibration factor
 */
void Sensor::setCalibrationFactor(double new_calibration_factor) {
    Logger::log("sen setCalibrationFactor");

    this->calibration_factor = new_calibration_factor;
    scale.set_scale((float) calibration_factor); //Adjust to this calibration factor
}

/**
 * get current weight of sensor (uses calibration)
 * @return weight as float in gramms
 */
float Sensor::getWeight() {
    return scale.get_units(1);
}

/**
 * get value multiple times
 * @param times how many times the value should read
 * @return average weight
 */
double Sensor::getValue(int times) {
    return scale.get_value(times);
}