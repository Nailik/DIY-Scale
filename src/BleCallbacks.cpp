//
// Created by kilian on 10.09.2021.
//

#include <BleCallbacks.h>

/**
 * constructor
 *
 * @param bluetooth_new pointer to Bluetooth instance
 */
BleConnectionCallback::BleConnectionCallback(Bluetooth *bluetooth_new) {
    this->bluetooth = bluetooth_new;
}

/**
 * called when a device connects
 */
void BleConnectionCallback::onConnect(NimBLEServer *pServerC) {
    Logger::log("ble onConnect");
    bluetooth->deviceChanged(true);
}

/**
 * called when a device disconnects
 */
void BleConnectionCallback::onDisconnect(NimBLEServer *pServerD) {
    Logger::log("ble onDisconnect");
    bluetooth->deviceChanged(false);
}

/**
 * BleButtonCallback is used for commands
 */
void BleButtonCallback::onWrite(NimBLECharacteristic *pCharacteristic) {
    if (pCharacteristic->getUUID() == NimBLEUUID(UUID_WEIGHT_COMMAND)) {
        std::string value = pCharacteristic->getValue();
        auto data = (uint8_t *) value.data();
        switch (data[0]) {
            case CMD_TARE:
                Logger::
                log("ble awaitTare");
                Program::logic->awaitTare();
                break;
            case CMD_DISPLAY_ON:
                Logger::log("ble display on");
                Program::logic->bleDisplay(true);
                break;
            case CMD_DISPLAY_OFF:
                Logger::log("ble display off");
                Program::logic->bleDisplay(false);
                break;
        }
    }
}