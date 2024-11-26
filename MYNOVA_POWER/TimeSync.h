/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <functional>

class TimeSync {
public:
    TimeSync(const char* ntpServer = "cn.pool.ntp.org", long gmtOffset_sec = 8 * 3600, int daylightOffset_sec = 0);
    bool syncTime();
    String getFormattedTime();
    void checkIfNeedSync();
    bool checkInternetConnection();
    void setTimeSyncCallback(std::function<void()> callback) {
        _syncCallback = callback;
    }

private:
    const char* _ntpServer;
    long _gmtOffset_sec;
    int _daylightOffset_sec;
    unsigned long lastSyncMillis = 0;
    const unsigned long syncInterval = 5000; // 5秒
    int syncAttempts = 0;
    const int maxSyncAttempts = 10;
    std::function<void()> _syncCallback = nullptr;
public:
    bool timeSynced = false;
};

#endif // TIME_SYNC_H
