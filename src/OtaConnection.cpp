//
// Created by kilian on 10.08.2021.
//

#include <OtaConnection.h>

/**
 * Handles self hosted wifi and network to connect to
 */

bool OtaConnection::networkIsConnected = false;
int OtaConnection::retryCount = 0;
unsigned long OtaConnection::lastStatus = 0;
String OtaConnection::softApPassword = "";

/**
 * setup wifi settings,
 * start self-hosted network,
 * try to connect to network if enabled
 */
void OtaConnection::setup() {
    Logger::log("ota setup");

    WiFi.enableIpV6();
    //setup ota connection
    WiFi.setAutoConnect(true);
    WiFi.onEvent(WiFiEvent);

    WiFiClass::mode(WIFI_MODE_AP);

    startAP();
    networkAttemptConnection();
}

/**
 * loop used to retry connection every 5 minutes
 */
void OtaConnection::loop() {
    if (!networkIsConnected && Configuration::network_enabled && millis() - lastStatus > 300000) {
        retryCount = 0;
        networkAttemptConnection();
    }
}

/**
 * start self hosted network
 */
void OtaConnection::startAP() {
    Logger::log("ota startAP");

    softApPassword = Configuration::wifi_password;
    WiFi.softAP(Configuration::wifi_ssid.c_str(), softApPassword.c_str());
    delay(100);
    IPAddress Ip(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(Ip, Ip, NMask);
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);
}

/**
 * check if ap should be started, maybe settings changed
 */
void OtaConnection::checkApMode() {
    Logger::log("ota checkApMode");

    if (!networkIsConnected || Configuration::always_on_ap) {
        if (networkIsConnected && WiFiClass::getMode() != WIFI_MODE_APSTA) {
            //switch wi-fi mode and start ap
            WiFiClass::mode(WIFI_MODE_APSTA);
            startAP();
            return;
        }
        if (WiFi.softAPSSID() != Configuration::wifi_ssid.c_str() || softApPassword != Configuration::wifi_password) {
            //needs to be restarted because settings change
            startAP();
        }
    }
}

/**
 * checks if network is in range and then tries to connect
 */
void OtaConnection::networkAttemptConnection() {
    Logger::log("ota networkAttemptConnection " + Configuration::network_ssid);

    //check if enabled and ssid is empty or not set
    if (Configuration::network_enabled && Configuration::network_ssid.length() > 0) {
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++) {
            if (WiFi.SSID(i) == Configuration::network_ssid) {
                Logger::log("ota network found");

                //try to connect therefore it's necessary to switch wi-fi mode
                WiFiClass::mode(WIFI_MODE_APSTA);
                //try to connect
                WiFi.begin(Configuration::network_ssid.c_str(), Configuration::network_password.c_str());
                break;
            }
        }
    }
}

/**
 * network connection established
 */
void OtaConnection::networkConnected() {
    Logger::log("ota networkConnected");

    networkIsConnected = true;
    //connection worked change wi-fi mode if necessary
    if (Configuration::always_on_ap) {
        WiFiClass::mode(WIFI_MODE_APSTA);
    } else {
        //close ap network
        WiFiClass::mode(WIFI_MODE_STA);
    }
}

/**
 * network disconnected (out of range, did block connect, wrong password etc)
 */
void OtaConnection::networkDisconnected() {
    lastStatus = millis();
    if (networkIsConnected) {
        Logger::log("ota networkDisconnected");

        networkIsConnected = false;
    }
    retryCount++;

    if (retryCount >= 6) {
        //start ap mode and stop retry connecting (because auto connect is on)
        WiFiClass::mode(WIFI_MODE_AP);
        startAP();
    }
}

/**
 * when any wifi event triggers
 *
 * @param event id of event
 */
void OtaConnection::WiFiEvent(WiFiEvent_t event) {
    switch (event) {
        case SYSTEM_EVENT_STA_START:
            //set sta hostname here
            WiFi.setHostname("scale.local");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            networkConnected();
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            networkDisconnected();
            break;
        default:
            break;
    }
}


/**
 * if network is connected
 * @return json which stat (0,1) and connected network id ssid
 */
String OtaConnection::getNetworkState() {
    return R"({"enabled":)" + String(Configuration::network_enabled) +
           R"(,"state":)" + String(networkIsConnected) +
           R"(,"ssid":")" + Configuration::network_ssid + R"("})";
}

/**
 * if wi-fi is currently on
 * @return json wich state (0, 1), if always on(0, 1) and self hosted network name (String)
 */
String OtaConnection::getWifiState() {
    return R"({"state":)" + String(WiFiClass::getMode() == WIFI_MODE_APSTA || WiFiClass::getMode() == WIFI_MODE_AP) +
           R"(,"always":)" + String(Configuration::always_on_ap) +
           R"(,"ssid":")" + Configuration::wifi_ssid + R"("})";
}