//
// Created by kilian on 14.09.2021.
//

#include <Timer.h>

/**
 * timer stops time and resets
 */

/**
 * start timer now
 */
void Timer::start() {
    Logger::log("timer start");

    lastTick = millis();
    currentTime = 0;
    running = true;
}

/**
 * stop timer or disarm if currently armed
 */
void Timer::stop() {
    Logger::log("timer stop");

    float current = millis();
    if (running) {
        currentTime += current - lastTick;
    }
    lastTick = current;
    running = false;
}

/**
 * returns if timer is currently running
 *
 * @return true = running, false = off
 */
bool Timer::isRunning() {
    return running;
}

/**
 * converts currentTime (millis) to seconds
 *
 * @return timer time in seconds with one decimal, -1 if neither armed nor running
 */
int Timer::getCurrentTime() {
    return (int) (currentTime / 100);
}

/**
 * call on every loop so timer can check if it should autostart (if armed)
 * also used to calculate current time
 *
 * @param currentWeight weight on scale
 */
void Timer::loop() {
    if (running) {
        float current = millis();
        currentTime += current - lastTick;
        lastTick = current;
    }
}