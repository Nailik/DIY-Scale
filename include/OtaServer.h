//
// Created by kiliam on 10.08.2021.
//

#ifndef PROJECT_OTASERVER_H
#define PROJECT_OTASERVER_H

#ifdef ESP32

#include <WiFi.h>
#include <AsyncTCP.h>
#include <Update.h>
#include <ESPmDNS.h>

#define U_PART U_SPIFFS

#elif ESP8266

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <Updater.h>
#include <ESP8266mDNS.h>
#define U_PART U_FS

#endif

#include <DNSServer.h>
#include <OtaConnection.h>
#include <Configuration.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <Logic.h>
#include <Program.h>
#include <ESPAsyncWebServer.h>

/**
 * Creates a webserver to host a website for Settings and Calibration
 * html files are stored in SPIFFS
 */
class OtaServer : public AsyncWebHandler {
private:
    //dns server
    static void setupDns();

    //url routes
    static void setupRoutes();

    //url routes for files
    static void setupFileRoutes();

    //url routes for data requested by website
    static void setupDataRequests();

    //url routes for data to be posted on
    static void setupDataPosts();

    //url routes for calibration
    static void setupCalibration();

    //send a file as response to client
    static void sendFileResponse(const String &uri, const String &type, AsyncWebServerRequest *request);

public:
    //starts setup
    static void setup();

    //process dns requests
    static void loop();
};


#endif //PROJECT_OTASERVER_H
