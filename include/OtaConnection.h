//
// Created by kilian on 10.08.2021.
//

#ifndef PROJECT_OTACONNECTION_H
#define PROJECT_OTACONNECTION_H

#include <WiFi.h>
#include <Configuration.h>
#include <Logger.h>

/**
 * Handles self hosted wifi and network to connect to
 */
class OtaConnection {
private:
    //start self-hosted network
    static void startAP();

    //how often it already
    static int retryCount;
    //last time a connection was tried
    static unsigned long lastStatus;
    //save soft ap password to check if settings change
    static String softApPassword;
    //connected to a network
    static bool networkIsConnected;

    //network did connect
    static void networkConnected();

    //network did disconnect
    static void networkDisconnected();

public:
    //setup connection and dns
    static void setup();

    //loop used to retry connection
    static void loop();

    //wi-fi state changes (sta or and ap)
    static void WiFiEvent(WiFiEvent_t event);

    //try to connect to network
    static void networkAttemptConnection();

    //check if ap should be started, maybe settings changed
    static void checkApMode();

    //state of connected network
    static String getWifiState();

    //if wi-fi is currently on
    static String getNetworkState();
};

#endif //PROJECT_OTACONNECTION_H
