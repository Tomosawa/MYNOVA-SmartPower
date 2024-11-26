/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "TimeSync.h"

TimeSync::TimeSync(const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec)
    : _ntpServer(ntpServer), _gmtOffset_sec(gmtOffset_sec), _daylightOffset_sec(daylightOffset_sec) {}

bool TimeSync::syncTime() {
    configTime(_gmtOffset_sec, _daylightOffset_sec, _ntpServer);
    
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Cannot sync time");
        return false;
    }
    return true;
}

String TimeSync::getFormattedTime() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        return "Get time failed";
    }
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(timeStringBuff);
}

bool TimeSync::checkInternetConnection() {
    WiFiClient client;
    if (client.connect("www.baidu.com", 80)) {
        client.stop();
        return true;
    }
    return false;
}

void TimeSync::checkIfNeedSync() {
    if (WiFi.status() == WL_CONNECTED && !timeSynced && syncAttempts < maxSyncAttempts) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastSyncMillis >= syncInterval) {
            lastSyncMillis = currentMillis;
            syncAttempts++;

            if (checkInternetConnection()) {
                if (syncTime()) {
                    Serial.println("Time Sync OK");
                    Serial.println("Current Time: " + getFormattedTime());
                    timeSynced = true;
                    syncAttempts = 0;
                    if (_syncCallback) {
                        _syncCallback();
                    }
                } else {
                    Serial.println("Time Sync Failed, Retry in next loop");
                }
            } else {
                Serial.println("No Internet Connection, Retry in next 5s");
            }

            if (syncAttempts >= maxSyncAttempts && !timeSynced) {
                Serial.println("Reach max sync attempts, give up sync");
            }
        }
    }
}