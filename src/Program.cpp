//
// Created by kilian on 08.09.2021.
//

#include <Program.h>

/**
 * separated loop to use less power for wifi
 *
 * @param pvParameters
 */
[[noreturn]] void otaLoop(void *pvParameters) {
    OtaConnection::setup();
    OtaServer::setup();
    while (true) {
        OtaServer::loop();
        delay(1000);
    }
}

/**
 * separated loop to not block bluetooth while tare
 *
 * @param pvParameters
 */
[[noreturn]] void bluetoothLoop(void *pvParameters) {
    while (true) {
        ((Bluetooth *) pvParameters)->loop();
        delay(100);
    }
}

//handles logic (weight, display, tare)
Logic *Program::logic;
Bluetooth *Program::bluetooth;

/**
 * This Program consists of 4 Parts
 * Configuration: holds Configuration values
 * Logic: User Interaction, Battery Management
 * Ota: Ota updates, Login, Webpage -> input is sent to Logic or/and config is updated
 * Bluetooth: BLE interface -> input is sent to Logic
 */
void Program::setup() {
    Serial.begin(9600);

    //load config files
    Configuration::loadConfiguration();

    //begin sound and status
    Sound::begin();
    Sound::playSound();
    Status::begin();

    //generate classes
    logic = new Logic();
    bluetooth = new Bluetooth();

    //start classes
    logic->setup();
    bluetooth->setup();

    //start bluetooth for bluetooth activities
    xTaskCreatePinnedToCore(bluetoothLoop, "bluetoothLoop", 4096, bluetooth, 1, nullptr, ARDUINO_RUNNING_CORE);
    //start loop for wi-fi activities
    xTaskCreatePinnedToCore(otaLoop, "otaLoop", 4096, nullptr, 1, nullptr, 0);
}

/**
 * loop ofer program
 */
void Program::loop() {
    logic->loop();

    delay(100);
}