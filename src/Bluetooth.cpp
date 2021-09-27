//
// Created by kilian on 04.08.2021.
//

#include <Bluetooth.h>

/**
 * Handles Bluetooth Connection
 *
 * Apps that use Atomax (Skale 2) Api can connect and read weight, battery and use tare functionality
 */


/**
 * creates bluetooth device definition
 */
void Bluetooth::setup() {
    Logger::log("ble setup");

    // Create the BLE Device
    NimBLEDevice::init("Skale");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create the BLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new BleConnectionCallback(this));

    /**
     * Unknown Service
     * 0000ff08-0000-1000-8000-00805f9b34fb
     * PRIMARY SERVICE
     */
    NimBLEService *pService_weight = pServer->createService(UUID_WEIGHT_SERVICE);

    /**
     * Unknown Service
     * 0000ef83-0000-1000-8000-00805f9b34fb
     * Properties: READ
     * Parsed value: (0x) 00-29-01-01-FD-D2-9F-10-00-01-FF-FF-FF-00-00
     * Raw value: (0x) 00-29-01-01-FD-D2-9F-10-00-01-FF-FF-FF-00-00
     * sometimes FD ssometimes FC, sometimes FB
     */
    NimBLECharacteristic *unknownWeightChar = pService_weight->createCharacteristic(UUID_WEIGHT_UNKNOWN, NIMBLE_PROPERTY::READ);
    unknownWeightChar->setValue((uint8_t[]) {0x00, 0x29, 0x01, 0x01, 0xFC, 0xD2, 0x9F, 0x10, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0x00, 0x00});

    /**
     * Unknown Service
     * 0000ef82-0000-1000-8000-00805f9b34fb
     * Properties: NOTIFY
     * 0x01 = tare, 0x02 = power
     */
    Bluetooth::pCharacteristic_button = pService_weight->createCharacteristic(UUID_WEIGHT_BUTTON_NOTIFY, NIMBLE_PROPERTY::NOTIFY);
    /**
     * Unknown Service
     * 0000ef81-0000-1000-8000-00805f9b34fb
     * Properties: NOTIFY
     * Parsed value: (0x) 00-FF-FF-FF-FF-FF-FF-FF-FF
     * Raw value: (0x) 00-FF-FF-FF-FF-FF-FF-FF-FF
     */
    Bluetooth::pCharacteristic_measure = pService_weight->createCharacteristic(UUID_WEIGHT_MEASURE, NIMBLE_PROPERTY::NOTIFY);
    /**
     * Unknown Service
     * 0000ef80-0000-1000-8000-00805f9b34fb
     * Properties: WRITE, WRITE NO RESPONSE
     */
    Bluetooth::pCharacteristic_command = pService_weight->createCharacteristic(UUID_WEIGHT_COMMAND, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    pCharacteristic_command->setCallbacks(new BleButtonCallback());


    /**
     * Unknown Service
     * 0f050001-3225-44b1-b97d-d3274acb29de
     * PRIMARY SERVICE
     */
    BLEService *unknownService = pServer->createService(UUID_SERVICE_UNKNOWN);
    /**
     * Unknown Service
     * 0f050002-3225-44b1-b97d-d3274acb29de
     * Properties: READ
     * Parsed value: (0x) 01-00-58-0C-B4-25-BE-38-61-A4-24-48-0C-B0-C6-93-A9-BE-9D-38-B1-DA-A8-C4-D3-A8-05-42-04-CC-26-6B-52-A4
     * Raw value: (0x) 01-00-58-0C-B4-25-BE-38-61-A4-24-48-0C-B0-C6-93-A9-BE-9D-38-B1-DA-A8-C4-D3-A8-05-42-04-CC-26-6B-52-A4
     */
    NimBLECharacteristic *unknownCharacteristics = unknownService->createCharacteristic(UUID_CHAR_UNKNOWN, NIMBLE_PROPERTY::READ);
    unknownCharacteristics->setValue((uint8_t[]) {0x01, 0x00, 0x58, 0x0C, 0xB4, 0x25, 0xBE, 0x38, 0x61, 0xA4, 0x24, 0x48, 0x0C, 0xB0,
                                                  0xC6, 0x93, 0xA9, 0xBE, 0x9D, 0x38, 0xB1, 0xDA, 0xA8, 0xC4, 0xD3, 0xA8, 0x05, 0x42, 0x04,
                                                  0xCC, 0x26, 0x6B, 0x52, 0xA4});

    /**
     * Battery Service
     * 0x180F
     * PRIMARY SERVICE
     */
    BLEService *pService_battery = pServer->createService(NimBLEUUID((uint16_t) 0x180f)); //TODO test UUID_BATTERY_SERVICE);
    /**
     * Battery Level
     * 0x2A19
     * Properties: NOTIFY, READ
     * Parsed value: 100%
     * Raw value: (0x) 64, "d"
     */
    Bluetooth::pCharacteristic_battery = pService_battery->createCharacteristic((uint16_t) 0x2a19, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);  //TODO test: UUID_BATTERY_LEVEL

    //set percentage as unit, important for leva!
    NimBLE2904 *batteryLevelDescriptor = (NimBLE2904 *) pCharacteristic_battery->createDescriptor((uint16_t) 0x2904);
    batteryLevelDescriptor->setFormat(NimBLE2904::FORMAT_UINT8);
    batteryLevelDescriptor->setNamespace(1);
    batteryLevelDescriptor->setUnit(0x27ad);

    pCharacteristic_battery->setValue({0x64});
    pCharacteristic_battery->notify(true);

    /**
     * Device Information
     * 0x180A
     * PRIMARY SERVICE
     */
    BLEService *deviceInformationService = pServer->createService(UUID_DEVICE_INFO_SERVICE);
    /**
     * Manufacturer Name String
     * 0x2A29
     * Properties: READ
     * Parsed value: ATOMAX INC.
     * Raw value: (0x) 41-54-4F-4D-41-58-20-49-4E-43-2E, "ATOMAX INC."
     */
    NimBLECharacteristic *manufacturerName = deviceInformationService->createCharacteristic(UUID_MANUFACTURER_NAME, NIMBLE_PROPERTY::READ);
    manufacturerName->setValue((uint8_t[]) {0x41, 0x54, 0x4F, 0x4D, 0x41, 0x58, 0x20, 0x49, 0x4E, 0x43, 0x2E});
    /**
     * Model Number String
     * 0x2A24
     * Properties: READ
     * Parsed value: SkaleII
     * Raw value: (0x) 53-6B-61-6C-65-49-49, "SkaleII"
     */
    NimBLECharacteristic *modelNumber = deviceInformationService->createCharacteristic(UUID_MODEL_NUMBER, NIMBLE_PROPERTY::READ);
    modelNumber->setValue((uint8_t[]) {0x53, 0x6B, 0x61, 0x6C, 0x65, 0x49, 0x49});
    /**
     * Hardware Revision String
     * 0x2A27
     * Properties: READ
     * Parsed value: R0B4
     * Raw value: (0x) 52-30-42-34, "R0B4"
     */
    NimBLECharacteristic *hardwareRevision = deviceInformationService->createCharacteristic(UUID_HARDWARE_REVISION, NIMBLE_PROPERTY::READ);
    hardwareRevision->setValue((uint8_t[]) {0x52, 0x30, 0x42, 0x34});
    /**
     * Firmware Revision String
     * 0x2A26
     * Properties: READ
     * Parsed value: R029
     * Raw value: (0x) 52-30-32-39, "R029"
     */
    NimBLECharacteristic *firmwareRevision = deviceInformationService->createCharacteristic(UUID_FIRMWARE_REVISION, NIMBLE_PROPERTY::READ);
    firmwareRevision->setValue((uint8_t[]) {0x52, 0x30, 0x32, 0x39});
    /**
     * Software Revision String
     * 0x2A28
     * Properties: READ
     * Parsed value: R019
     * Raw value: (0x) 52-30-31-39, "R019"
     */
    NimBLECharacteristic *softwareRevision = deviceInformationService->createCharacteristic(UUID_SOFTWARE_REVISION, NIMBLE_PROPERTY::READ);
    softwareRevision->setValue((uint8_t[]) {0x52, 0x30, 0x31, 0x39});



    //set initial data
    Logger::log("ble initial data");
    sendData();

    Logger::log("ble start services");
    //0x1800
    //0x1801
    //0xff08
    pService_weight->start();
    //0f05
    unknownService->start();
    //0x180F
    pService_battery->start();
    //0x180A
    deviceInformationService->start();

    //create advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    NimBLEAdvertisementData data = NimBLEAdvertisementData();

    //manufacturer data
    uint8_t manufacturerData[8] = {0xEF, 0x81, 0x00, 0x00, 0x00, 0xFF};
    data.setManufacturerData(std::string((char *) &manufacturerData[0], 6));

    //all services are added as one single uuid
    data.setCompleteServices(NimBLEUUID(UUID_SCAN_RESPONSE_128));
    //data.setName("Skale");

    pAdvertising->setScanResponseData(data);
    pAdvertising->setScanResponse(true);

    //functions that help with iPhone connections issue - but leva! will not find the scale
    //pAdvertising->setMinPreferred(0x06);
    //pAdvertising->setMinPreferred(0x12);

    Logger::log("ble start advertising");
    //start advertising
    pAdvertising->start();
}

/**
 * sends current weight and battery percentage to bluetooth device
 */
void Bluetooth::sendData() {
    uint8_t data[9];

    //*10 necessary to remove comma
    long weigh = (long) (Program::logic->getWeight() * (double) 10);

    //split long in 4 data packets
    data[0] = 0; //unused

    data[1] = weigh & 0x00FF;       //exponent
    data[2] = (weigh >> 8) & 0x00FF; //mantisse1
    data[3] = (weigh >> 16) & 0x00FF; //mantisse2
    data[4] = -1; //mantisse3

    //probably end of data, official app doesn't work without
    data[5] = weigh & 0x00FF; //exponent
    data[6] = (weigh >> 8) & 0x00FF; //mantisse1
    data[7] = (weigh >> 16) & 0x00FF; //mantisse2
    data[8] = -1; //mantisse3


    //send weight
    pCharacteristic_measure->setValue(data, 9);
    pCharacteristic_measure->notify(true);

    //send battery status
    uint8_t bat = Logic::getBatteryPercentage();

    pCharacteristic_battery->setValue(bat);
    pCharacteristic_battery->notify(true);
}


/**
 * sends to ble device that a button was pressed
 */
void Bluetooth::btnPressed(int btn) {
    Logger::log("ble btnPressed");
    if (Configuration::bluetooth_enabled) {
        if (deviceConnected) {
            uint8_t data[1];
            data[0] = btn; //1 = tare //2 = on off
            pCharacteristic_button->setValue(data, btn);
            pCharacteristic_button->notify(true);
        }
    }
}

/**
 * loop checks if has to send data or if it should start advertising
 */
void Bluetooth::loop() {
    if (Configuration::bluetooth_enabled) {
        if (deviceConnected) {
            sendData();
        }

        // disconnecting
        if (!deviceConnected && oldDeviceConnected) {
            Logger::log("ble startAdvertising");

            delay(500); // give the bluetooth stack the chance to get things ready
            pServer->startAdvertising(); // start advertising
            oldDeviceConnected = deviceConnected;
        }

        // connecting
        if (deviceConnected && !oldDeviceConnected) {
            Logger::log("ble stopAdvertising");

            oldDeviceConnected = deviceConnected;
            //stop advertising
            pServer->stopAdvertising();
        }
    }
}

/**
 * stores new device state
 *
 * @param deviceState true = connected, false = disconnected
 */
void Bluetooth::deviceChanged(bool deviceState) {
    Logger::log("ble deviceChanged");

    deviceConnected = deviceState;
    if (!deviceConnected) {
        Program::logic->bleDisconnected();
    }
}

/**
 * get current device state
 *
 * @return "off": bluetooth of
 *          "on": bluetooth on but not connected
 *          "name": Bluetooth connected to device
 */
String Bluetooth::getDeviceState() {
    if (Configuration::bluetooth_enabled) {
        if (deviceConnected) {
            return pServer->getPeerInfo(0).getIdAddress().toString().c_str();
        }
        return "1";
    }
    return "0";
}

/**
 * toggle bluetooth on/off
 * saves in Configuration
 */
void Bluetooth::toggleBluetooth() {
    Logger::log("ble toggleBluetooth");

    Configuration::bluetooth_enabled = !Configuration::bluetooth_enabled;
    Configuration::saveConfiguration();
    if (Configuration::bluetooth_enabled) {
        Bluetooth::setup();
    } else {
        NimBLEDevice::deinit(true);
    }
}

