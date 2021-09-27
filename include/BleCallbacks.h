//
// Created by kilian on 10.09.2021.
//

#ifndef PROJECT_BLECONNECTIONCALLBACKS_H
#define PROJECT_BLECONNECTIONCALLBACKS_H

#include <Bluetooth.h>
#include <Program.h>

class Bluetooth;

/**
 * Callback is used when device connects/disconnects
 */
class BleConnectionCallback : public NimBLEServerCallbacks {
private:
    //pointer to bluetooth to call
    Bluetooth *bluetooth;
public:
    BleConnectionCallback(Bluetooth *bluetooth);

    //device connects
    void onConnect(NimBLEServer *pServerC) override;

    //device disconnects
    void onDisconnect(NimBLEServer *pServerC) override;
};

/**
 * BleButtonCallback is used for commands
 */
class BleButtonCallback : public NimBLECharacteristicCallbacks {
public:
    //when command is written to this device
    void onWrite(NimBLECharacteristic *pCharacteristic) override;
};

#endif //PROJECT_BLECONNECTIONCALLBACKS_H
