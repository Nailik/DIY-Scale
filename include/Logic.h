//
// Created by kiliam on 01.08.2021.
//

#ifndef PROJECT_LOGIC_H
#define PROJECT_LOGIC_H

#include <Calibration.h>
#include <Display.h>
#include <Button.h>
#include <Sensor.h>
#include <Sound.h>
#include <Configuration.h>
#include <Status.h>
#include <Program.h>
#include <Timer.h>

class Display;

class Button;

class Timer;

class Sensor;

//pin 27 and 25 so 2^27 + 2^25 to Hex
#define BUTTON_PIN_BITMASK 0xA000000

/**
 * setup display, buttons and load cells and handle sleep mode
 */
class Logic {
private:
    //displays in one class
    Display *display;
    //right button
    Button *btnTare;
    //left button
    Button *btnTimer;
    //load cells
    Sensor *cell_0;
    Sensor *cell_1;
    //Timer
    Timer *timer;
    //stores current weight
    float current_weight = 0;
    //if display should be on
    bool displayOn = true;
    //stores if currently tare is running
    bool tareProcessRunning = false;
    //tare on next loop
    bool awaitTareFlag = false;
    //sleep on button release
    bool awaitSleepFlag = false;
    //store last batter level
    int lastBatteryPercentage = 100;
    //last time some action was done
    static unsigned long lastActionTime;
    //time when tare led was turned on
    static unsigned long tareLedTime;
    //time when timer led was turned on
    static unsigned long timerLedTime;

    //reset tare button
    void consumeTareClick();

    //reset timer button
    void consumeTimerClick();

    //read current weight
    void readWeight();

    //read current battery percentage
    static int readBatteryPercentage();

public:
    //setup everything
    void setup();

    //loop
    void loop();

    //returns current weight
    float getWeight();

    //switch display on/off from bluetooth request
    void bleDisplay(bool on);

    //ble device disconnected
    void bleDisconnected();

    //sets the awaitTare, used from isr (non-blocking, tare on next loop)
    void awaitTare();

    //execute tare now
    void tare();

    //returns current battery percentage
    static int getBatteryPercentage();

    //update display on off
    void updateDisplaySettings();

    //update brightness button, LED, display
    void updateBrightnessSettings();

    //get current brightness settings as json
    static String getBrightnessState();

    //returns weight as text
    String getWeightText();

    //return state json of display
    String getDisplayState();

    //updates calibration values
    void updateCalibration();

    //tell logic there was an action so de vice is not going into sleep mode
    static void actionDone();

    //read value from sensor 0
    double getValue_0(int times);

    //read value from sensor 1
    double getValue_1(int times);
};


#endif //PROJECT_LOGIC_H
