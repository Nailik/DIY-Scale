//
// Created by kilian on 01.08.2021.
//

/**
 * Logic to calibrate the scale
 */

#ifndef PROJECT_CALIBRATION_H
#define PROJECT_CALIBRATION_H

#include <Logger.h>

class Calibration {
private:
    //current weight to calibrate
    static float current_weight;
    //calibration factor of sensor 0
    static double calibration_0;
    //calibration factor of sensor 1
    static double calibration_1;
    //stores if calibration is running
    static bool calibrating;
public:
    //start, will tara
    static void start_calibration(float weight);

    //calibrates the given sensor
    static void calibrate_sensor(int sensor);

    //cancel calibration without saving
    static void cancel_calibration();

    //save the calibration and update sensors
    static void finish_calibration();

    //returns if calibration is currently running
    static bool isCalibrating();
};

#endif //PROJECT_CALIBRATION_H
