//
// Created by kilian on 01.08.2021.
//

#include <Button.h>

bool Button::ledOn = false;

/**
 * Handles Touch Buttons
 *
 * when touching the Button the isr_btn event with a pointer to the button will be called
 * @param pin pin of data in
 * @param led_pin pin for led data out
 */
Button::Button(uint8_t pin, uint8_t ledPin, uint8_t statusLedId) {
    this->pin = pin;
    this->ledPin = ledPin;
    this->statusLedId = statusLedId;
    this->pressType = 0;
    this->consumed = false;
}

/**
 * set pin modes and attach Interrupt
 */
void Button::begin() {
    Logger::log("btn begin");

    pinMode(pin, INPUT_PULLUP);

    ledcSetup(0, 5000, 8);
    ledcAttachPin(ledPin, 0);

    toggleLight(true);
    attachInterruptArg(pin, isr_btn, (void *) this, CHANGE);
}

/**
 * called by isr when pin state changes
 * checks for long or short press
 */
void Button::stateChanged(int pinState) {
    Logger::log("btn stateChanged");

    unsigned long currentTime = millis();

    if (status == LOW && pinState == HIGH) {
        consumed = false;
        //button was pressed
        pressTime = currentTime;

    } else if (status == HIGH && pinState == LOW) {
        //button was released
        if (!consumed) {
            if (currentTime - pressTime >= LongPressMillis) {
                //long press
                this->pressType = 2;
            } else {
                //short press
                this->pressType = 1;
            }
        }
    }
    status = pinState;
}

/**
 * checks for long press without releasing button
 */
void Button::loop() {
    unsigned long currentTime = millis();

    if (status == HIGH) {
        //led should be turned on/stay on
        Status::led(statusLedId, true);
        ledTime = currentTime;
    }

    if (status == HIGH && digitalRead(pin) == HIGH) {
        //button was not released
        if (currentTime - pressTime >= LongPressMillis) {
            //long press
            if (!consumed) {
                this->pressType = 2;
            }
        }
    } else if (status == LOW) {
        //check if led should be turned off
        if (currentTime - ledTime > 3000) {
            Status::led(statusLedId, false);
        }
    }
}

/**
 * @return  0 = not, 1 = short, 2 = long
 */
int Button::isPressed() const {
    return pressType;
}

/**
 * resets the button pressed state to false
 */
void Button::resetButton() {
    Logger::log("btn resetButton");

    this->consumed = true;
    this->pressType = 0;
}

/**
 * resets the button pressed state to false
 * @return HIGH if currently pressed else LOW
 */
int Button::readPinState() {
    return status;
}

/**
 * toggle the led pin to HIGH/LOW
 * @param on true -> on, false -> off
 */
void Button::toggleLight(bool on) {
    Logger::log("btn toggleLight");

    ledOn = on;
    if (on) {
        // turn the LED on
        ledcWrite(0, Configuration::brightness_btn);
    } else {
        // turn the LED off
        ledcWrite(0, 0);
    }
}

/**
 * update brightness of buttons
 *
 * @param brightness  brightness between 0 and 255
 */
void Button::updateBrightness() {
    Logger::log("btn updateBrightness");

    if (ledOn) {
        ledcWrite(0, Configuration::brightness_btn);
    }
}

/**
 * called when button is pressed by isr
 *
 * @param button pointer to button that was pressed
 */
void IRAM_ATTR Button::isr_btn(void *button) {
    int pinState = digitalRead(((Button *) button)->pin);
    ((Button *) button)->stateChanged(pinState);
}