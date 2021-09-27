//
// Created by kilian on 02.08.2021.
//

/**
 * to load the configuration file
 */

#ifndef PROJECT_CONFIGURATION_H
#define PROJECT_CONFIGURATION_H

#include <Logger.h>
#include <list>
#include <Preferences.h>

class Configuration {
public:
    //calibration of load cells
    static double cell_0_Calibration;
    static double cell_1_Calibration;
    static bool ever_calibrated;
    // self-hosted Wi-Fi settings
    static String wifi_ssid;
    static String wifi_password;
    static bool always_on_ap;
    //network settings
    static String network_ssid;
    static String network_password;
    static bool network_enabled;

    /**
     * brightness of different items
     *
     * display_brightness       0-15
     *
     * brightness_btn           0-255
     * brightness_btn_led       0-255
     * brightness_status_led    0-255
     */
    static uint8_t display_brightness;
    static bool display_always;
    static uint8_t brightness_btn;
    static uint8_t brightness_led;
    //time when devices go in deep sleep in minutes
    static int deepSleepTime;
    //bluetooth connections to new devices allowed
    static bool bluetooth_enabled;
    //display off time
    static bool sound_enabled;
    //timer
    static bool timer_enabled;

    //load the configuration from file
    static void loadConfiguration();

    //save the configuration to a file
    static void saveConfiguration();

    //load default configuration
    static void resetDefaultSettings(bool resetCalibration);
};

#endif //PROJECT_CONFIGURATION_H
