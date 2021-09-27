//
// Created by kilian on 01.08.2021.
//

#include <Status.h>

Adafruit_NeoPixel *Status::pixels;

/**
 * Status Led (Button, Battery)
 */

/**
 * initialize neopixel
 */
void Status::begin() {
    Logger::log("status begin");

    pixels = new Adafruit_NeoPixel(3, PIN_STATUS_LED, NEO_GRB + NEO_KHZ800);
    pixels->begin();
    updateBrightness();
}

/**
 * update led
 *
 * @param id index
 * @param show on or off
 */
void Status::led(uint8_t id, boolean show) {
    if (show) {
        pixels->setPixelColor(id, Adafruit_NeoPixel::Color(255, 255, 255));
    } else {
        pixels->setPixelColor(id, Adafruit_NeoPixel::Color(0, 0, 0));
    }
    pixels->show();
}

/**
 * show the battery status
 *
 * @param percentage > 75 = green, > 50 = blue, > 25 = orange, < 25 = red
 */
void Status::updateBatteryStatus(int percentage) {
    if (percentage > 75) {
        //green
        pixels->setPixelColor(IND_BTN_BAT_LED, Adafruit_NeoPixel::Color(102, 204, 0));
    } else if (percentage > 50) {
        //blue
        pixels->setPixelColor(IND_BTN_BAT_LED, Adafruit_NeoPixel::Color(0, 102, 204));
    } else if (percentage > 25) {
        //orange
        pixels->setPixelColor(IND_BTN_BAT_LED, Adafruit_NeoPixel::Color(204, 102, 0));
    } else {
        //red
        pixels->setPixelColor(IND_BTN_BAT_LED, Adafruit_NeoPixel::Color(0, 0, 204));
    }
    pixels->show();
}

/**
 * update brightness and show it
 */
void Status::updateBrightness() {
    Logger::log("status updateBrightness");

    pixels->setBrightness(Configuration::brightness_led);
    pixels->show();
}

/**
 * turn all led off
 */
void Status::turnOff() {
    Logger::log("status turnOff");

    pixels->setBrightness(0);
    pixels->show();
}
