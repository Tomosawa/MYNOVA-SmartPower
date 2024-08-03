/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef __EVENTHANDLE_H_
#define __EVENTHANDLE_H_
#pragma once
#include <Arduino.h>

enum EventType
{
    EVENT_POWERON,
    EVENT_POWEROFF,
    EVENT_SETSPEED,
    EVENT_GETSPEED,
    EVENT_CONNECTWIFI,
    EVENT_DISCONNECTWIFI,
    EVENT_SAVEWIFI,
};
class EventHandle
{
public:
    EventHandle();
    ~EventHandle();
    void init();
    void powerOn();
    void powerOff();
    void setSpeed(int value);
    void getSpeed();
    void connectWifi();
    void disConnectWifi();
    void saveWifi();
    static void eventTask(void *param);
    
};

#endif