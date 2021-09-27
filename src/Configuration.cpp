//
// Created by kilian on 02.08.2021.
//

#include <Configuration.h>

Preferences preferences;

/**
 * calibration of load cells
 */
double Configuration::cell_0_Calibration;
double Configuration::cell_1_Calibration;
bool Configuration::ever_calibrated;

/**
 * self hosted Wifi settings
 */
String Configuration::wifi_ssid;
String Configuration::wifi_password;
bool Configuration::always_on_ap;

/**
 * remote wifi
 */
String Configuration::network_ssid;
String Configuration::network_password;
bool Configuration::network_enabled;

/**
 * brightness of different items
 *
 * display_brightness       0-15
 *
 * brightness_btn           0-255
 * brightness_btn_led       0-255
 * brightness_status_led    0-255
 */
uint8_t Configuration::display_brightness;
bool Configuration::display_always;
uint8_t Configuration::brightness_btn;
uint8_t Configuration::brightness_led;

/**
 * display off time
 */
int Configuration::deepSleepTime;

/**
 * bluetooth connections to new devices allowed
 */
bool Configuration::bluetooth_enabled;

/**
 * display off time
 */
bool Configuration::sound_enabled;
//timer
bool Configuration::timer_enabled;

/**
 * load current stored configuration with default values
 */
void Configuration::loadConfiguration() {
    Logger::log("conf loadConfiguration");

    preferences.begin("configuration", false);

    cell_0_Calibration = preferences.getDouble("calibration_0", 3300);
    cell_1_Calibration = preferences.getDouble("calibration_1", 3300);
    ever_calibrated = preferences.getBool("calibrated", false);

    wifi_ssid = preferences.getString("wifi_ssid", "Scale");
    wifi_password = preferences.getString("wifi_password", "12345678010");
    always_on_ap = preferences.getBool("always_on_ap", false);

    network_ssid = preferences.getString("network_ssid", "");
    network_password = preferences.getString("network_pass", "");
    network_enabled = preferences.getBool("network_enabled", true);

    display_brightness = preferences.getUInt("br_display", 15);
    display_always = preferences.getUInt("display_always", true);
    brightness_btn = preferences.getUInt("br_btn", 255);
    brightness_led = preferences.getUInt("br_led", 255);

    deepSleepTime = preferences.getInt("deepSleepTime", 10);

    bluetooth_enabled = preferences.getBool("bluetooth_en", true);
    sound_enabled = preferences.getBool("sound_enabled", true);
    timer_enabled = preferences.getBool("timer_enabled", true);

    preferences.end();
}


/**
 * save after config changes
 */
void Configuration::saveConfiguration() {
    Logger::log("conf saveConfiguration");

    preferences.begin("configuration", false);

    preferences.putDouble("calibration_0", cell_0_Calibration);
    preferences.putDouble("calibration_1", cell_1_Calibration);
    preferences.putBool("calibrated", ever_calibrated);

    preferences.putString("wifi_ssid", wifi_ssid);
    preferences.putString("wifi_password", wifi_password);
    preferences.putBool("always_on_ap", always_on_ap);

    preferences.putString("network_ssid", network_ssid);
    preferences.putString("network_pass", network_password);

    preferences.putBool("network_enabled", network_enabled);

    preferences.putUInt("br_display", display_brightness);
    preferences.putBool("display_always", display_always);

    preferences.putUInt("br_btn", brightness_btn);
    preferences.putUInt("br_led", brightness_led);

    preferences.putInt("deepSleepTime", deepSleepTime);

    preferences.putBool("bluetooth_en", bluetooth_enabled);

    preferences.putBool("sound_enabled", sound_enabled);
    preferences.putBool("timer_enabled", timer_enabled);

    preferences.end();
}

/**
 * reset Configuration to default settings and restart
 *
 * @param resetCalibration true if calibration should be reset too
 */
void Configuration::resetDefaultSettings(bool resetCalibration) {
    Logger::log("conf resetDefaultSettings");

    double old_cell_0_Calibration = cell_0_Calibration;
    double old_cell_1_Calibration = cell_1_Calibration;
    preferences.begin("configuration", false);
    preferences.clear();
    preferences.end();
    loadConfiguration();
    if (resetCalibration) {
        cell_0_Calibration = old_cell_0_Calibration;
        cell_1_Calibration = old_cell_1_Calibration;
        saveConfiguration();
    }
    ESP.restart();
}