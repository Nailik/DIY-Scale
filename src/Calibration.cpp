//
// Created by kilian on 01.08.2021.
//

#include <Calibration.h>

/**
 * Logic to calibrate the scale
 */

bool Calibration::calibrating = false;
float Calibration::current_weight = 0;
double Calibration::calibration_0 = 0;
double Calibration::calibration_1 = 0;

/**
 * starts the calibration
 *
 * @param weight send the weight that's used for the calibration
 */
void Calibration::start_calibration(float weight) {
    Logger::log("cal start_calibration");

    Logger::log(String(weight));
    calibrating = true;
    current_weight = weight;
    calibration_0 = 0;
    calibration_1 = 0;
    //tare
    Program::logic->tare();
}

/**
 * calibrates the given sensor
 * @param sensor 0 = left, 1 = right
 */
void Calibration::calibrate_sensor(int sensor) {
    Logger::log("cal calibrate_sensor");

    if (sensor == 2) {
        double val_0 = Program::logic->getValue_0(12);
        Logger::log("cal value " + String(val_0));
        calibration_0 = val_0 / current_weight;
        Logger::log("cal vactor " + String(calibration_0));
        Logger::log("cal result " + String(val_0 / calibration_0));
    } else if (sensor == 1) {
        double val_1 = Program::logic->getValue_1(12);
        Logger::log("cal value " + String(val_1));
        calibration_1 = val_1 / current_weight;
        Logger::log("cal vactor " + String(calibration_1));
        Logger::log("cal result " + String(val_1 / calibration_1));
    }
}

/**
 * cancel calibration without saving
 */
void Calibration::cancel_calibration() {
    Logger::log("cal cancel_calibration");

    current_weight = 0;
    calibration_0 = 0;
    calibration_1 = 0;
    calibrating = false;
}

/**
 * save the calibration and update sensors
 */
void Calibration::finish_calibration() {
    Logger::log("cal finish_calibration");

    Configuration::cell_0_Calibration = calibration_0;
    Configuration::cell_1_Calibration = calibration_1;

    Configuration::ever_calibrated = true;
    Configuration::saveConfiguration();

    Program::logic->updateCalibration();

    calibrating = false;
}

/**
 * returns if calibration is currently running
 *
 * @return bool
 */
bool Calibration::isCalibrating() {
    return calibrating;
}
