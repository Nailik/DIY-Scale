//
// Created by kilian on 02.08.2021.
//

#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

#include <Program.h>
#include <WString.h>
#include <Arduino.h>
#include <chrono>
#include <ctime>
#include <cstdio>

/**
 * used to log data
 *
 * timestamp code: https://stackoverflow.com/questions/52728952/how-do-i-write-a-message-timestamp-to-a-log-file
 */
class Logger {
public:
    //log line with timestamp
    static void log(const String &string);
};

#endif //PROJECT_LOGGER_H
