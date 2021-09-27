//
// Created by kilian on 01.08.2021.
//

#ifndef PROJECT_BUTTON_H
#define PROJECT_BUTTON_H

#include <cstdint>
#include <Arduino.h>
#include <Pinout.h>
#include <Configuration.h>
#include <Logger.h>

#define LongPressMillis 2000

/**
 * Handles Touch Buttons
 *
 * when touching the Button the isr_btn event with a pointer to the button will be called
 */
class Button {
    //isr action
    static void isr_btn(void *button);

private:
    //status of last isr action 1 = pressed, 0 = released
    uint8_t status = LOW;
    //saves if current press was already consumed prevent multiple clicks on long press and hold
    bool consumed;
    //to determine between short and long pressed
    unsigned long pressTime = 0;
    //pin of data in
    uint8_t pin;
    //pin for led
    uint8_t ledPin;
    //id of status led
    uint8_t statusLedId;
    //saves if button is pressed 0 = not, 1 = short, 2 = long
    int pressType;
    //stores if led is set to on
    static bool ledOn;
    //stores time of led
    unsigned long ledTime = 0;
public:
    //constructor
    Button(uint8_t pin, uint8_t led_pin, uint8_t statusLedId);

    //start pins
    void begin();

    //checks for long press without releasing button
    void loop();

    //called by isr when pin state changes
    void stateChanged(int pinState);

    //returns if button is pressed  0 = not, 1 = short, 2 = long
    int isPressed() const;

    //reset button state
    void resetButton();

    //read current pin state and update status
    int readPinState();

    //toggle light on and off
    static void toggleLight(bool on);

    //update Button brightness
    static void updateBrightness();
};

#endif //PROJECT_BUTTON_H
