//
// Created by kilian on 01.08.2021.
//

#include <Logic.h>

//last time some action was done
unsigned long Logic::lastActionTime = 0;

/**
 * setup display, buttons and load cells and handle sleep mode
 *
 * setup creates instances of objects and sets the up
 * tare
 * and setup sleep mode
 */
void Logic::setup() {
    Logger::log("lg setup");

    lastActionTime = millis();
    //Timer
    display = new Display();

    btnTare = new Button(PIN_BTN_TARA, PIN_BTN_TARA_LED, IND_BTN_TARE_STATUS_LED);
    btnTimer = new Button(PIN_BTN_TIMER, PIN_BTN_TIMER_LED, IND_BTN_TIMER_STATUS_LED);
    cell_0 = new Sensor(PIN_SENSOR_0, PIN_HX711_CLOCK_0, Configuration::cell_0_Calibration);
    cell_1 = new Sensor(PIN_SENSOR_1, PIN_HX711_CLOCK_1, Configuration::cell_1_Calibration);

    timer = new Timer();

    display->begin();
    btnTare->begin();
    btnTimer->begin();
    cell_0->begin();
    cell_1->begin();

    cell_0->tare();
    cell_1->tare();

    current_weight = 0.0;

    //setup sleep mode
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
}

/**
 * checks button, updates display and checks sleep mode
 */
void Logic::loop() {
    //saves is there was an action and not go to sleep
    bool currentAction = false;
    unsigned long currentTime = millis();

    btnTare->loop();
    btnTimer->loop();

    //check sleep mode by time
    if ((currentTime - lastActionTime) > Configuration::deepSleepTime * 60000) {
        display->clear();
        btnTare->resetButton();
        btnTimer->resetButton();
        Status::turnOff();

        Logger::log("lg sleep");
        Serial.flush();
        Sound::playSound();
        esp_deep_sleep_start();
    }


    // go sleep if button is not long pressed anymore
    if (awaitSleepFlag && btnTare->readPinState() == LOW) {
        //make sure display is cleared
        display->clear();
        btnTare->resetButton();
        btnTimer->resetButton();
        Status::turnOff();

        awaitSleepFlag = false;
        Logger::log("lg sleep btn");
        Serial.flush();
        esp_deep_sleep_start();
    }

    // sleep mode request but only if flag not set yet
    if (btnTare->isPressed() == 2 && !awaitSleepFlag) {
        display->clear();
        consumeTareClick();
        Button::toggleLight(false);
        awaitSleepFlag = true;
        Logger::log("Request sleep");
    }

    //if currently calibration get text to show
    if (Calibration::isCalibrating()) {

        //CAL
        //C  0b0000000000111001
        //A  0b0000000011110111
        //L  0b0000000000111000
        uint16_t code[] = {0x39, 0x77, 0b0000000000111000};
        display->showStatusCode(code, 3);

        if (awaitTareFlag) {
            currentAction = true;
            tare();
            Sound::playSound();
        }

    } else {
        if (btnTare->isPressed() == 1 || awaitTareFlag) {
            //tara button
            currentAction = true;
            consumeTareClick();
            Logger::log("awaitTareFlag");
            tare();
        }

        //power button
        if (btnTimer->isPressed() == 1) {
            currentAction = true;
            consumeTimerClick();
            //start timer, stop timer (start auto resets)
            if (timer->isRunning()) {
                timer->stop();
            } else {
                timer->start();
            }
        } else if (btnTimer->isPressed() == 2) {
            consumeTimerClick();
            currentAction = true;
            //timer disable
            Configuration::timer_enabled = !Configuration::timer_enabled;
            Configuration::saveConfiguration();
        }
        //read current weight
        readWeight();

        float prev_weight = current_weight;

        if (abs(current_weight - prev_weight) > 2) {
            //if there was a weight change bigger than 2 gramm there is action on the scale
            currentAction = true;
        }

        // update timer
        timer->loop();

        //show weight but only if display is on and device doesn't want to go to sleep
        if (displayOn && !awaitSleepFlag) {
            display->show(current_weight, timer->getCurrentTime());
        }
    }

    //read and update battery level
    int currentBatteryLevel = readBatteryPercentage();
    if (lastBatteryPercentage != currentBatteryLevel) {
        lastBatteryPercentage = currentBatteryLevel;
        Status::updateBatteryStatus(currentBatteryLevel);
    }

    //check if there was an action, if so save time to not go to sleep
    if (currentAction) {
        lastActionTime = millis();
    }
}

/**
 * reset tare button and play sound
 */
void Logic::consumeTareClick() {
    if (btnTare->isPressed() != 0) {
        Sound::playSound();
        btnTare->resetButton();
    }
}

/**
 * reset tare button and play sound
 */
void Logic::consumeTimerClick() {
    if (btnTimer->isPressed() != 0) {
        Sound::playSound();
        btnTimer->resetButton();
    }
}

/**
 * get weight
 * @return float as weight in gramms
 */
float Logic::getWeight() {
    return current_weight;
}

/**
 * reads current weight
 */
void Logic::readWeight() {
    double v_0 = cell_0->getValue(1);
    double v_1 = cell_1->getValue(1);

    double val = ((v_0 * Configuration::cell_1_Calibration) + (v_1 * Configuration::cell_0_Calibration)) /
                 (Configuration::cell_0_Calibration * Configuration::cell_1_Calibration);
    float w = (float) ((int) (val * 10)) / 10; //remove everything behind first digit after comma to get stable value
    current_weight = w; //remove everything behind first digit after comma to get stable values
    if (current_weight == -0) {
        current_weight = 0;
    }
}

/**
 * executes tare, display will blink while tare
 */
void Logic::tare() {
    Logger::log("lg tare");

    tareProcessRunning = true;
    display->blink(true);
    awaitTareFlag = false;
    cell_0->tare();
    cell_1->tare();
    display->blink(false);
    Sound::playSound();
    tareProcessRunning = false;
}

/**
 * reads current battery percentage
 * @return int from 0 to 100
 */
int Logic::readBatteryPercentage() {
    return (int) map(analogRead(BAT_PIN) * 1.7l, BAT_MIN, BAT_MAX, 0, 100);
}


/**
 * switch display on/off from bluetooth request
 * only if not overridden by user settings
 * @param on true or false
 */
void Logic::bleDisplay(bool on) {
    Logger::log("lg bleDisplay");

    actionDone();
    if (!Configuration::display_always) {
        //checks if user wants that display stays on
        displayOn = on;
        if (!displayOn) {
            //clear display if off
            display->clear();
        }
    }
}

/**
 * called when ble device disconnects to turn display back on
 */
void Logic::bleDisconnected() {
    displayOn = true;
}

/**
 * set the await tare flag
 */
void Logic::awaitTare() {
    Logger::log("lg awaitTare");

    actionDone();
    awaitTareFlag = true;
}

/**
 * get current battery level
 *
 * @return battery level 0 to 100
 */
int Logic::getBatteryPercentage() {
    return Program::logic->lastBatteryPercentage;
}

/**
 * updates brightness of display according to configuration
 */
void Logic::updateDisplaySettings() {
    Logger::log("lg updateDisplaySettings");

    if (!displayOn && Configuration::display_always) {
        //turn display back on, turned off by app
        displayOn = true;
    }
}

/**
 * updates brightness of display according to configuration
 */
void Logic::updateBrightnessSettings() {
    Logger::log("lg updateBrightnessSettings");

    display->updateBrightness();
    Button::updateBrightness();
    Status::updateBrightness();
}

/**
 * returns current weight
 * @return weight in gramm as string or "tare" if tare process is running or awaiting tare
 */
String Logic::getWeightText() {
    if (awaitTareFlag || tareProcessRunning) {
        return "tare";
    }
    return String(getWeight());
}

/**
 * current state of display
 *
 * @return as json {"always": 1, "state": 1, "brightness": 0-15}
 */
String Logic::getDisplayState() {
    return R"({"state":)" + String(displayOn) + R"(,"always":)" + Configuration::display_always + R"(})";
}

/**
 * current brightness settings
 *
 * @return as json {"display": 0-15, "buttons":0-255, "led": 0-255}
 */
String Logic::getBrightnessState() {
    return R"({"display":)" + String(Configuration::display_brightness) + R"(,"buttons":)" + String(Configuration::brightness_btn) + R"(,"led":)" + Configuration::brightness_led + R"(})";
}

/**
 * when this is called las action time is set to current time
 */
void Logic::actionDone() {
    Logger::log("actionDone");

    lastActionTime = millis();
}

/**
 * read value from sensor n times
 * @param times int how often sensor is read
 * @return average value
 */
double Logic::getValue_0(int times) {
    return cell_0->getValue(times);
}

/**
 * read value from sensor n times
 * @param times int how often sensor is read
 * @return  average value
 */
double Logic::getValue_1(int times) {
    return cell_1->getValue(times);
}

/**
 * update calibration factor of sensors
 */
void Logic::updateCalibration() {
    Logger::log("lg updateCalibration");

    actionDone();
    cell_0->setCalibrationFactor(Configuration::cell_0_Calibration);
    cell_1->setCalibrationFactor(Configuration::cell_1_Calibration);
}
