//
// Created by kilian on 02.08.2021.
//

#include <Logger.h>

/**
 * used to log data
 *
 * timestamp code: https://stackoverflow.com/questions/52728952/how-do-i-write-a-message-timestamp-to-a-log-file
 */
// strftime format
#define LOGGER_PRETTY_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

// printf format
#define LOGGER_PRETTY_MS_FORMAT ".%03d"

/**
 * convert current time to milliseconds since unix epoch
 */
template<typename T>
static int to_ms(const std::chrono::time_point<T> &tp) {
    using namespace std::chrono;

    auto dur = tp.time_since_epoch();
    return static_cast<int>(duration_cast<milliseconds>(dur).count());
}


/**
 * format it in two parts: main part with date and time and part with milliseconds
 */
static std::string pretty_time() {
    auto tp = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(tp);

    // this function use static global pointer. so it is not thread safe solution
    std::tm *time_info = std::localtime(&current_time);

    char buffer[128];

    int string_size = strftime(
            buffer, sizeof(buffer),
            LOGGER_PRETTY_TIME_FORMAT,
            time_info
    );

    int ms = to_ms(tp) % 1000;

    string_size += snprintf(
            buffer + string_size, sizeof(buffer) - string_size,
            LOGGER_PRETTY_MS_FORMAT, ms
    );

    return std::string(buffer, buffer + string_size);
}

/**
 * log with timestamp
 *
 * @param string string to be written in a line behind timestamp
 */
void Logger::log(const String &string) {
    if (DEBUG_LOG_ENABLED) {
        Serial.println(String(pretty_time().c_str()) + " | " + string);
    }
}