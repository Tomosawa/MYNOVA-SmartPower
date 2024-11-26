/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef __EVENTHANDLE_H_
#define __EVENTHANDLE_H_
#pragma once
#include <Arduino.h>

// 前向声明
class TaskManager;
enum class TimeTaskType;
enum class RepeatType;

enum EventType
{
    EVENT_POWERON,
    EVENT_POWEROFF,
    EVENT_SETSPEED,
    EVENT_GETSPEED,
    EVENT_CONNECTWIFI_ST,
    EVENT_DISCONNECTWIFI_ST,
    EVENT_CONNECTWIFI_AP,
    EVENT_DISCONNECTWIFI_AP,
    EVENT_SAVE_SETTINGS,
    EVENT_SETCONTRAST,
    EVENT_ALARM_TRIGGER,
    EVENT_SYNC_TIME,
    EVENT_PRINT_ALARMS
};

// 定义闹钟事件的数据结构
struct AlarmEventData {
    uint32_t taskId;
    TimeTaskType type;
    RepeatType repeatType;
};

// 定义事件数据结构
struct EventData {
    EventType type;
    union {
        AlarmEventData alarm;
        // 其他事件的数据可以放在这里
    } data;
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
    void connectWifi_ST();
    void disConnectWifi_ST();
    void connectWifi_AP();
    void disConnectWifi_AP();
    void saveSettings();
    void setContrast();
    static void eventTask(void *param);
    void triggerAlarm(uint32_t taskId, TimeTaskType type, RepeatType repeatType);
    void syncTime();
    void printAlarms();
};

#endif