//
// Created by kilian on 01.08.2021.
//

#include <Display.h>

/**
 * Handles both displays
 * can display String or Float
 * has variable brightness
 */
Display::Display() {
    this->showing_timer = Configuration::timer_enabled;
    this->current_number = std::numeric_limits<float>::max();

    this->matrix1 = new Adafruit_7segment();
    this->matrix2 = new Adafruit_7segment();
}

/**
 * start pins
 */
void Display::begin() {
    Logger::log("disp begin");

    //pins depending on how it was installed
    matrix1->begin(ADDRESS_DISPLAY_0);
    matrix2->begin(ADDRESS_DISPLAY_1);

    updateBrightness();
}

/**
 * Display will only show this status code and no timer
 * if longer than 8 rest will not be shown
 */
void Display::showStatusCode(uint16_t code[], int codeSize) {
    matrix1->clear();
    matrix2->clear();
    int d = 9;
    for (int i = codeSize - 1; i >= 0; i--) {
        writeDigitRaw(d, code[i]);
        d--;
        if (d == 7 || d == 2) { //reserved for : dots
            d--;
        }
        if (d < 0) {
            break;
        }
    }
    //flush buffer to displays
    matrix1->writeDisplay();
    matrix2->writeDisplay();
}

/**
 * Show a number across both displays
 * only updates when number is different to previous
 *
 * @param number number with one decimal to show, if greater than 999 it shows ERR
 * @param timer number of timer, if > 60 its 1.xx if < 60 its xx only shown if enabled in settings
 */
void Display::show(float number, int timer) {
    //only if different from previous
    if (number == current_number &&
        (Configuration::timer_enabled == showing_timer && (!Configuration::timer_enabled || current_timer == timer))) {
        return;
    }

    //store current number
    current_number = number;
    current_timer = timer;
    matrix1->clear();
    matrix2->clear();

    if (number > 999.9) {
        //shows error Err
        writeDigitRaw(6, 0x79); //E
        writeDigitRaw(8, 0b0000000001010000); //r
        writeDigitRaw(9, 0b0000000001010000); //r
    } else {
        //vector to store digits
        std::vector<int> v;

        //remove numbers after comma
        int i = (int) (number * 10);
        //make number positive
        if (i < 0) i *= -1;

        //store digits in vector
        while (i != 0) {
            v.push_back(i % 10);
            i = i / 10;
        }

        //between -0.9 and 0.9 add a missing zero
        if (number < 1 && number > -1) v.push_back(0);

        //at 0.0 add a missing zero
        if (number == 0.0) v.push_back(0);

        //we have 10 digits in both displays including :
        int z = 9;

        for (int x = 0; x < v.size(); x++) {
            if (z == 2 || z == 7) { //jump over :
                z--;
            }
            if (x == 1) {
                //before last digit display dot
                writeDigitNum(z, v[x], true);
            } else if (x == 0) {
                writeDigitNum(z, v[x], false);
            } else {
                writeDigitNum(z, v[x], false);
            }
            z--;
        }
        //show '-' when number is below 0
        if (number < 0) {
            writeDigitRaw(z, 0b0000000001000000);
        }
    }

    if (Configuration::timer_enabled && timer >= 0) {
        //write timer on first 3 digits of display
        if (timer < 600) {
            //split into seconds and tenth of a second
            writeDigitNum(3, timer % 10, false);
            timer /= 10;
            writeDigitNum(1, timer % 10, true);
            timer /= 10;
            writeDigitNum(0, timer % 10, false);
        } else {
            //split into minutes and seconds
            int multiple = timer / 600;
            timer -= 600 * multiple;
            writeDigitNum(0, multiple, true);
            timer /= 10;
            writeDigitNum(3, timer % 10, false);
            timer /= 10;
            writeDigitNum(1, timer % 10, false);
        }
    }
    showing_timer = Configuration::timer_enabled;

    //flush buffer to displays
    matrix1->writeDisplay();
    matrix2->writeDisplay();
}

/**
 * update the brightness of the display
 */
void Display::updateBrightness() {
    Logger::log("disp updateBrightness");

    matrix1->setBrightness(Configuration::display_brightness);
    matrix2->setBrightness(Configuration::display_brightness);

    matrix1->writeDisplay();
    matrix2->writeDisplay();
}

/**
 * set blink on/off of both displays
 *
 * @param on true -> blinks, false -> no blinking
 */
void Display::blink(bool on) {
    Logger::log("disp blink");

    if (on) {
        Wire.beginTransmission(ADDRESS_DISPLAY_0);
        Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (5 << 1));
        Wire.endTransmission();
        Wire.beginTransmission(ADDRESS_DISPLAY_1);
        Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (5 << 1));
        Wire.endTransmission();
        //matrix1->blinkRate(3);
        //matrix2->blinkRate(3);
    } else {
        matrix1->blinkRate(0);
        matrix2->blinkRate(0);
    }
}

/**
 * write a num on a specific index
 * automatically chooses correct display
 *
 * @param d index between 0 and 9, should not be 2 or 7 as this is for dots
 * @param num number to show
 * @param dot if dot should be shown after number
 */
void Display::writeDigitNum(uint8_t d, uint8_t num, bool dot) {
    if (d >= 5) {
        matrix2->writeDigitNum(d - 5, num, dot);
    } else {
        matrix1->writeDigitNum(d, num, dot);
    }
}

/**
 * write a bitmask on a specific index
 * automatically chooses correct display
 *
 * @param d index between 0 and 9, should not be 2 or 7 as this is for dots
 * @param bitmask number to bitmask
 */
void Display::writeDigitRaw(uint8_t d, uint16_t bitmask) {
    if (d >= 5) {
        matrix2->writeDigitRaw(d - 5, bitmask);
    } else {
        matrix1->writeDigitRaw(d, bitmask);
    }
}


/**
 * clear display, show nothing
 */
void Display::clear() {
    Logger::log("disp clear");

    matrix1->clear();
    matrix2->clear();

    matrix1->writeDisplay();
    matrix2->writeDisplay();
}