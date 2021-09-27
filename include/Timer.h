//
// Created by kilian on 14.09.2021.
//

#ifndef PROJECT_TIMER_H
#define PROJECT_TIMER_H

#include <chrono>
#include <ctime>
#include <cstdio>
#include <Program.h>

/**
 * timer stops time and resets
 */
class Timer {
private:
    //save last tick in millis to know time
    float lastTick = 0;
    //current saved time
    float currentTime = 0;
    //if it's currently running
    bool running = false;
public:
    //to start now
    void start();

    //stop timer
    void stop();

    //if it's currently running
    bool isRunning();

    //returns current time
    int getCurrentTime();

    //call at loop to increase time and run autostart sequence
    void loop();
};

#endif //PROJECT_TIMER_H
