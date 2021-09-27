//
// Created by kilian on 02.08.2021.
//

#include <Sound.h>

/**
 * used to play a short beep
 * can be completely muted by Configuration::sound_enabled
 */

/**
 * setup pin to output
 */
void Sound::begin() {
    Logger::log("sound begin");

    pinMode(PIN_SOUND, OUTPUT);
}

/**
 * play a short beep, will delay for 50 ms and turn off itself
 */
void Sound::playSound() {
    Logger::log("sound playSound");

    if (Configuration::sound_enabled) {
        digitalWrite(PIN_SOUND, HIGH);
        delay(50);
        digitalWrite(PIN_SOUND, LOW);
    }
}