//
// Created by kilian on 02.08.2021.
//

#ifndef PROJECT_SOUND_H
#define PROJECT_SOUND_H

#include <Pinout.h>
#include <Arduino.h>
#include <Logger.h>
#include <Configuration.h>

/**
 * used to play a short beep
 * can be completely muted by Configuration::sound_enabled
 */
class Sound {
public:
    //setup pin mode
    static void begin();

    //play short sound
    static void playSound();
};

#endif //PROJECT_SOUND_H
