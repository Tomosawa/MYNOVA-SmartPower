/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef __WEBSERVICE__H__
#define __WEBSERVICE__H__
#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include <DNSServer.h>
#include <SPIFFS.h>
#include <LittleFS.h>
#include "WIFINetwork.h"
#include "pmbus.h"
class WebService
{
public:
    WebService();
    void init(IPAddress ServerIP, WIFINetwork *wifiNetwork, PMBus *psu);
    void serverStart();
    void processRequest();

    void handleWIFIScanRequest(AsyncWebServerRequest *request);
    void handleWIFISaveRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void handleWIFIClearRequest(AsyncWebServerRequest *request);
    void handleWIFIConnectRequest(AsyncWebServerRequest *request);
    void handlePSUInfoRequest(AsyncWebServerRequest *request);
    void handlePSUSettingRequest(AsyncWebServerRequest *request);
    void handlePSUPowerRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    void handlePSUFanRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
private:
    DNSServer dnsserver;
    AsyncWebServer server;
    String domianURL;    // 访问的域名
    IPAddress Server_IP; // 要访问的地址的IP
    WIFINetwork *pWifiNetwork;
    PMBus *pPSU;
};

#endif