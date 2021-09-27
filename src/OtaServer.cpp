//
// Created by kilian on 10.08.2021.
//

#include <OtaServer.h>


/**
 * Creates a webserver to host a website for Settings and Calibration
 * html files are stored in SPIFFS
 */
static AsyncWebServer otaServer_server(80);
static DNSServer dnsServer;

/**
 * setup dns, routes, start server and open Spiffs files for webserver
 */
void OtaServer::setup() {
    Logger::log("otaserver setup");

    setupDns();
    // Initialize SPIFFS
    SPIFFS.begin(false);
    setupRoutes();
    otaServer_server.begin();
}


/**
 * Start Dns Server, set Skale Internal network IP to 192.168.0.1
 * And Starts a MDNS Server to be found in networks
 *
 * Depends on Network how fast and reliable the Skale can be found
 */
void OtaServer::setupDns() {
    Logger::log("otaserver setupDns");

    dnsServer.start(53, "*", IPAddress(192, 168, 0, 1));
    MDNS.begin("scale");
    MDNS.addService("http", "tcp", 80);
}

/**
 * process dns requests
 */
void OtaServer::loop() {
    dnsServer.processNextRequest();
}

/**
 * routes for web server to answer on
 */
void OtaServer::setupRoutes() {
    Logger::log("otaserver setupRoutes");

    setupFileRoutes();
    setupDataRequests();
    setupDataPosts();
    setupCalibration();

    //default when site is not found
    otaServer_server.onNotFound([](AsyncWebServerRequest *request) {
        Logger::log("404 server request " + request->url());
        request->send(404, "text/html", "Site not found");
    });


    //cancel the calibration (only if calibration is running)
    otaServer_server.on("/doUpdate", HTTP_POST,
                        [](AsyncWebServerRequest *request) {},
                        [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
                            if (!index) {
                                // if filename includes spiffs, update the spiffs partition
                                int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH;
                                if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
                                    Update.printError(Serial);
                                    return;
                                }
                            }

                            if (Update.write(data, len) != len) {
                                Update.printError(Serial);
                                return;
                            }

                            request->send(200, "text/plane");
                            //print update process
                            Logger::log("Update Progress: " + String((int) ((Update.progress() * 100) / Update.size())) + " %");

                            if (final) {
                                AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "rebooting");
                                response->addHeader("Refresh", "20");
                                response->addHeader("Location", "/");
                                request->send(response);
                                if (!Update.end(true)) {
                                    Update.printError(Serial);
                                } else {
                                    Logger::log("Update complete");
                                    Serial.flush();
                                    ESP.restart();
                                }
                            }
                        });
}

/**
 * Setup Routes for files (html, css, js)
 */
void OtaServer::setupFileRoutes() {
    otaServer_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver index");
        sendFileResponse("/html/index.html", "text/html", request);
    });

    otaServer_server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver index");
        sendFileResponse("/html/index.html", "text/html", request);
    });

    otaServer_server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver settings");
        sendFileResponse("/html/settings.html", "text/html", request);
    });

    otaServer_server.on("/calibrate.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver calibrate");
        sendFileResponse("/html/calibrate.html", "text/html", request);
    });

    otaServer_server.on("/update.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver update");
        sendFileResponse("/html/update.html", "text/html", request);
    });

    otaServer_server.on("/bootstrap/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver bootstrap css");
        sendFileResponse("/bootstrap/bootstrap.min.css", "text/css", request);
    });

    otaServer_server.on("/bootstrap/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver bootstrap");
        sendFileResponse("/bootstrap/bootstrap.min.js", "text/js", request);
    });

    otaServer_server.on("/bootstrap/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver jquery");
        sendFileResponse("/bootstrap/jquery.min.js", "text/js", request);
    });
}

/**
 * sends file as response to request automaticall uses gzip file if client accepts it
 *
 * @param uri String to path
 * @param type Type of file data
 * @param request request pointer
 */
void OtaServer::sendFileResponse(const String &uri, const String &type, AsyncWebServerRequest *request) {
    if (request->hasHeader("Accept-Encoding") && request->getHeader("Accept-Encoding")->value().indexOf("gzip") != -1) {
        //check if accepts gzip
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, uri + ".gz", type);
        //add header because there is a bug where header is maybe not set
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    } else {
        request->send(SPIFFS, uri, type);
    }
}

/**
 * Website requests data to need no reload and show changes continuously
 */
void OtaServer::setupDataRequests() {
    // Route to load style.css file
    otaServer_server.on("/battery", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", String(Logic::getBatteryPercentage()));
    });

    //returns weight and battery
    otaServer_server.on("/weight", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", Program::logic->getWeightText());
    });

    //send client current network state and information
    otaServer_server.on("/network", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", OtaConnection::getNetworkState());
    });

    //send client current wi-fi state
    otaServer_server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/json", OtaConnection::getWifiState());
    });

    //send client current bluetooth state
    otaServer_server.on("/bluetooth", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", Program::bluetooth->getDeviceState());
    });

    //send client if sound is currently enabled as on/off value
    otaServer_server.on("/sound", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", String(Configuration::sound_enabled));
    });

    //send client if timer is currently enabled as on/off value
    otaServer_server.on("/timer", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", String(Configuration::timer_enabled));
    });

    //send client if display information
    otaServer_server.on("/display", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", Program::logic->getDisplayState());
    });

    //send the current brightness settings
    otaServer_server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", Logic::getBrightnessState());
    });

    //send client current sleep time
    otaServer_server.on("/sleep", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", String(Configuration::deepSleepTime));
    });

    //send client current sleep time
    otaServer_server.on("/calibrated", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", String(Configuration::ever_calibrated));
    });

    //send client current version
    otaServer_server.on("/version", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plane", String(CURRENT_VERSION));
    });

    //website requests tare
    otaServer_server.on("/tare", HTTP_GET, [](AsyncWebServerRequest *request) {
        Program::logic->awaitTare();
        request->send(200, "text/plane");
    });
}

/**
 * Website sets data to change settings
 */
void OtaServer::setupDataPosts() {
    //set network settings
    otaServer_server.on("/setnetwork", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver setnetwork");
        Logic::actionDone();

        if (request->hasParam("param1", false) &&
            request->hasParam("param2", false) &&
            request->hasParam("param3", false)) {

            String ssid = request->getParam("param1", false)->value();
            String password = request->getParam("param2", false)->value();

            if (ssid.length() > 0) {
                Configuration::network_ssid = ssid;
                Configuration::network_password = password;

                bool enabled = false;
                if (request->getParam("param3", false)->value() == "true") {
                    enabled = true;
                }
                Configuration::network_enabled = enabled;
                Configuration::saveConfiguration();
                //Attempt connection
                OtaConnection::networkAttemptConnection();
                request->send(200, "text/plane", OtaConnection::getNetworkState());
                return;
            }
        }

        request->send(400);
    });

    //set wifi settings
    otaServer_server.on("/setwifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver setwifi");
        Logic::actionDone();

        if (request->hasParam("param1", false) &&
            request->hasParam("param2", false) &&
            request->hasParam("param3", false)) {

            String ssid = request->getParam("param1", false)->value();
            String password = request->getParam("param2", false)->value();

            if (ssid.length() > 0) {
                Configuration::wifi_ssid = ssid;
                Configuration::wifi_password = password;
                bool enabled = false;
                if (request->getParam("param3", false)->value() == "true") {
                    enabled = true;
                }
                Configuration::always_on_ap = enabled;
                Configuration::saveConfiguration();
                //check if ap needs to be started
                OtaConnection::checkApMode();
                request->send(200, "text/json", OtaConnection::getWifiState());
                return;
            }

            request->send(400);
        }
    });

    //toggle bluetooth on/off<
    otaServer_server.on("/togglebluetooth", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver togglebluetooth");
        Logic::actionDone();
        Program::bluetooth->toggleBluetooth();
        request->send(200, "text/plane", Program::bluetooth->getDeviceState());
    });

    //toggle sound depending on current device settings
    otaServer_server.on("/togglesound", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver togglesound");
        Logic::actionDone();
        Configuration::sound_enabled = !Configuration::sound_enabled;
        Configuration::saveConfiguration();
        request->send(200, "text/plane", String(Configuration::sound_enabled));
    });

    //set display settings
    otaServer_server.on("/toggledisplay", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver toggledisplay");
        Logic::actionDone();
        Configuration::display_always = !Configuration::display_always;
        Configuration::saveConfiguration();
        Program::logic->updateDisplaySettings();
        request->send(200, "text/plane", Program::logic->getDisplayState());
    });

    //toggle sound depending on current device settings
    otaServer_server.on("/toggletimer", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver toggletimer");
        Logic::actionDone();
        Configuration::timer_enabled = !Configuration::timer_enabled;
        Configuration::saveConfiguration();
        request->send(200, "text/plane", String(Configuration::timer_enabled));
    });

    //set wifi settings
    otaServer_server.on("/setbrightness", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver setbrightness");
        Logic::actionDone();
        if (request->hasParam("param1", false) &&
            request->hasParam("param2", false) &&
            request->hasParam("param3", false)) {

            char *p;
            int brightnessD = strtol(request->getParam("param1", false)->value().c_str(), &p, 10);
            int brightnessB = strtol(request->getParam("param2", false)->value().c_str(), &p, 10);
            int brightnessL = strtol(request->getParam("param3", false)->value().c_str(), &p, 10);

            if (*p) {
                request->send(400);
                return;
            } else {
                Configuration::display_brightness = brightnessD;
                Configuration::brightness_btn = brightnessB;
                Configuration::brightness_led = brightnessL;

                Configuration::saveConfiguration();
                Program::logic->updateBrightnessSettings();

                request->send(200, "text/plane");
                return;
            }
        }
        request->send(400);
    });

    //set sleep time settings
    otaServer_server.on("/setsleep", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver setsleep");
        Logic::actionDone();
        if (request->hasParam("param1", false)) {
            int num = request->getParam("param1", false)->value().toInt();
            if (num > 0) {
                //check if num > 0 because don't allow sleep time <= 0, therefore atoi is fine
                Configuration::deepSleepTime = num;
                Configuration::saveConfiguration();
                request->send(200, "text/plane", String(Configuration::deepSleepTime));
                return;
            }
        }
        request->send(400);
    });

    //reset settings
    otaServer_server.on("/setreset", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver setreset");
        Logic::actionDone();
        if (request->hasParam("param1", false)) {

            bool resetCalibration = false;
            if (request->getParam("param1", false)->value() == "true") {
                resetCalibration = true;
            }

            request->send(200, "text/plane");
            Configuration::resetDefaultSettings(resetCalibration);
            return;
        }
        request->send(400);
    });
}

void OtaServer::setupCalibration() {
    //starts the calibration process with weight (if not running)
    otaServer_server.on("/startCalibration", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver startCalibration");
        Logic::actionDone();
        if (!Calibration::isCalibrating() && request->hasParam("param0", false)) {

            float weight = request->getParam("param0", false)->value().toFloat();
            if (weight > 0) {
                Calibration::start_calibration(weight);
                request->send(200);
                return;
            }
        }
        request->send(400);
    });

    // execute a step for the calibration param sensor index (1 or 2) (only if calibration is running)
    otaServer_server.on("/calibrate", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver calibrate");
        Logic::actionDone();
        if (Calibration::isCalibrating() && request->hasParam("param0", false)) {

            int index = request->getParam("param0", false)->value().toInt();

            if (index > 0) {
                Calibration::calibrate_sensor(index);
                request->send(200);
                return;
            }
        }

        request->send(400);
    });

    //finish the calibration (only if calibration is running)
    otaServer_server.on("/finishCalibration", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver finishCalibration");
        Logic::actionDone();
        if (Calibration::isCalibrating()) {
            Calibration::finish_calibration();
            request->send(200);
            return;
        }
        request->send(400);
    });

    //cancel the calibration (only if calibration is running)
    otaServer_server.on("/cancelCalibration", HTTP_POST, [](AsyncWebServerRequest *request) {
        Logger::log("otaserver cancelCalibration");
        Logic::actionDone();
        if (Calibration::isCalibrating()) {
            Calibration::cancel_calibration();
            request->send(200);
            return;
        }
        request->send(400);
    });
}
