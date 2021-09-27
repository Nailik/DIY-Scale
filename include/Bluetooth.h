//
// Created by kilian on 04.08.2021.
//

#ifndef PROJECT_BLUETOOTH_H
#define PROJECT_BLUETOOTH_H

/**
 * Generic Access
 * 0x1800
 * PRIMARY SERVICE
 * defined in nimconfig.h
 */

#include <NimBLEDevice.h>
#include <Program.h>
#include <BleCallbacks.h>
#include <esp_bt_main.h>

#define UUID_GENERIC_ACCESS       "00001800-0000-1000-8000-00805f9b34fb"
#define UUID                      "00001801-0000-1000-8000-00805f9b34fb"
#define UUID_DEVICE_NAME          "00002a00-0000-1000-8000-00805f9b34fb"
#define UUID_DEVICE_APPEARANCE    "00002a01-0000-1000-8000-00805f9b34fb"
#define UUID_DEVICE_PERIPHERAL    "00002a04-0000-1000-8000-00805f9b34fb"

#define UUID_WEIGHT_SERVICE       "0000ff08-0000-1000-8000-00805f9b34fb"
#define UUID_WEIGHT_UNKNOWN       "0000ef83-0000-1000-8000-00805f9b34fb"
#define UUID_WEIGHT_BUTTON_NOTIFY "0000ef82-0000-1000-8000-00805f9b34fb"
#define UUID_WEIGHT_MEASURE       "0000ef81-0000-1000-8000-00805f9b34fb"
#define UUID_WEIGHT_COMMAND       "0000ef80-0000-1000-8000-00805f9b34fb"

#define UUID_SERVICE_UNKNOWN      "0f050001-3225-44b1-b97d-d3274acb29de"
#define UUID_CHAR_UNKNOWN         "0f050002-3225-44b1-b97d-d3274acb29de"

#define UUID_BATTERY_SERVICE      "0000180f-0000-1000-8000-00805f9b34fb"
#define UUID_BATTERY_LEVEL        "00002a19-0000-1000-8000-00805f9b34fb"

#define UUID_DEVICE_INFO_SERVICE  "0000180A-0000-1000-8000-00805f9b34fb"
#define UUID_MANUFACTURER_NAME    "00002A29-0000-1000-8000-00805f9b34fb"
#define UUID_MODEL_NUMBER         "00002a24-0000-1000-8000-00805f9b34fb"
#define UUID_HARDWARE_REVISION    "00002a27-0000-1000-8000-00805f9b34fb"
#define UUID_FIRMWARE_REVISION    "00002a26-0000-1000-8000-00805f9b34fb"
#define UUID_SOFTWARE_REVISION    "00002a28-0000-1000-8000-00805f9b34fb"

#define UUID_SCAN_RESPONSE_128    "6e400001-b5a3-f393-e0a9-e50e24dcca9e"


/**
 * Handles Bluetooth Connection
 *
 * Apps that use Atomax (Skale 2) Api can connect and read weight, battery and use tare functionality
 */
class Bluetooth {
private:
    //stores nimble data
    NimBLEServer *pServer;
    //to send weight
    NimBLECharacteristic *pCharacteristic_measure;
    //to send battery
    NimBLECharacteristic *pCharacteristic_battery;
    //to receive commands
    NimBLECharacteristic *pCharacteristic_command;
    //to send when a button was clicked
    NimBLECharacteristic *pCharacteristic_button;
    //stores if device currently connected
    bool deviceConnected = false;
    //stores if a device was connected
    bool oldDeviceConnected = false;

    //function to send data
    void sendData();

public:
    //setup bluetooth data
    void setup();

    //send data or start advertisement
    void loop();

    //used from callbacks for device state changes
    void deviceChanged(bool deviceState);

    //used from website to get device state
    String getDeviceState();

    //toggle bluetooth on/off
    void toggleBluetooth();

    //send to ble device that a button was pressed 1 = tara, 2 = power
    void btnPressed(int btn);
};

#endif //PROJECT_BLUETOOTH_H
