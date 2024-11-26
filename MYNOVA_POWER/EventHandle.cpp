/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "EventHandle.h"
#include "GUIRender.h"
#include "pmbus.h"
#include "Settings.h"
#include "PageTimer.h"
#include "TaskManager.h"
#include "TimeSync.h"


extern GUIRender guiRender;
extern int displayPage;
extern PMBus psu;
extern WIFINetwork wifiNetwork;
extern Settings settings;
extern PageTimer pageTimer;
extern TaskManager taskManager;
extern TimeSync timeSync;
extern EventHandle eventHandle;
QueueHandle_t eventQueue;
EventHandle::EventHandle()
{
}

void EventHandle::eventTask(void *param)
{
    EventData eventData;
    while (1)
    {
        if (xQueueReceive(eventQueue, &eventData, portMAX_DELAY))
        {
            // 处理事件
            switch (eventData.type)
            {
                case EVENT_POWERON:
                    psu.on();
                    break;
                case EVENT_POWEROFF:
                    psu.off();
                    displayPage = 0;
                    break;
                case EVENT_SETSPEED:
                    psu.writeFanCommand(guiRender.FanSpeed);
                    pageTimer.resetTimer();
                    break;
                case EVENT_GETSPEED:
                    guiRender.FanSpeed = psu.readFanCommand();
                    break;
                case EVENT_CONNECTWIFI_ST:
                    if(wifiNetwork.getWIFIState() == WL_CONNECTED)
                        wifiNetwork.disconnectWIFI();
                    wifiNetwork.startWIFI();
                    break;
                case EVENT_DISCONNECTWIFI_ST:
                    wifiNetwork.disconnectWIFI();
                    break;
                case EVENT_CONNECTWIFI_AP:
                    wifiNetwork.startAP();
                    break;
                case EVENT_DISCONNECTWIFI_AP:
                    wifiNetwork.disconnectAP();
                    break;
                case EVENT_SAVE_SETTINGS:
                    settings.UpdateTasksFromManager(taskManager);
                    settings.SaveSettings();
                    //wifiNetwork.wifi_json = wifiNetwork.getAllSSIDJson();
                    break;
                case EVENT_SETCONTRAST:
                    guiRender.setContrast(settings.brightness);
                    break;
                case EVENT_SYNC_TIME:
                    if(timeSync.checkInternetConnection())
                    {  
                        if (timeSync.syncTime()) {
                            Serial.println("Time Sync OK");
                            Serial.println("Current Time: " + timeSync.getFormattedTime());
                            timeSync.timeSynced = true;
                        } else {
                            Serial.println("Time Sync Failed");
                        }
                    }
                    break;
                case EVENT_PRINT_ALARMS:
                    taskManager.printAllAlarms();
                    break;
                case EVENT_ALARM_TRIGGER:
                    const AlarmEventData& alarmData = eventData.data.alarm;
                    uint32_t originalTaskId = alarmData.taskId >= 10000 ? alarmData.taskId - 10000 : alarmData.taskId;
                    // 打印触发信息
                    const char* taskTypes[] = {"Period", "Timer ON", "Timer OFF"};
                    const char* repeatTypes[] = {"Once", "Daily", "Weekly"};
                    Serial.printf("Alarm triggered: ID=%d, Type=%s, Repeat=%s\n", 
                                 alarmData.taskId,
                                 taskTypes[static_cast<int>(alarmData.type)],
                                 repeatTypes[static_cast<int>(alarmData.repeatType)]);
                    
                    // 执行开关机操作
                    if (alarmData.type == TimeTaskType::TIMER_ON) {
                        psu.on();
                    } else {
                        psu.off();
                    }
                    
                    // 查找原始任务
                    auto taskIt = std::find_if(taskManager.periodTasks.begin(), taskManager.periodTasks.end(),
                        [originalTaskId](const PeriodTask& task) { return task.id == originalTaskId; });
                    
                    if (taskIt != taskManager.periodTasks.end()) {
                        if (taskIt->type == TimeTaskType::PERIOD) {
                            if (alarmData.repeatType == RepeatType::ONCE) {
                                // 一次性任务，如果是结束闹钟就禁用任务
                                if (alarmData.taskId >= 10000) {
                                    taskIt->enabled = false;
                                    eventHandle.saveSettings();
                                }
                            } else {
                                // 重复的时间段任务
                                // 如果是结束闹钟触发，需要设置下一天的开始和结束闹钟
                                if (alarmData.taskId >= 10000) {
                                    taskManager.setupNextAlarm(*taskIt);
                                }
                                // 如果是开始闹钟触发，不需要额外操作，因为结束闹钟已经设置好了
                            }
                        } else {
                            // 非时间段任务（定时开机或定时关机）
                            if (alarmData.repeatType == RepeatType::ONCE) {
                                taskIt->enabled = false;
                                eventHandle.saveSettings();
                            } else {
                                // 重复任务设置下一次闹钟
                                taskManager.setupNextAlarm(*taskIt);
                            }
                        }
                    }
                    // 清理当前闹钟
                    auto alarmIt = taskManager._alarms.find(alarmData.taskId);
                    if (alarmIt != taskManager._alarms.end()) {
                        if (alarmIt->second.timer != nullptr) {
                            esp_timer_stop(alarmIt->second.timer);
                            esp_timer_delete(alarmIt->second.timer);
                        }
                        taskManager._alarms.erase(alarmIt);
                    }
                    break;
            }
        }
    }
}

void EventHandle::init()
{
    eventQueue = xQueueCreate(10, sizeof(EventData));
    xTaskCreate(EventHandle::eventTask, "EventTaskFunc", 4096, NULL, 1, NULL);
}

void EventHandle::powerOn()
{
    // 启动电源
    EventData eventData;
    eventData.type = EVENT_POWERON;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::powerOff()
{
    // 关闭电源
    EventData eventData;
    eventData.type = EVENT_POWEROFF;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::setSpeed(int value)
{
    guiRender.FanSpeed = value;
    EventData eventData;
    eventData.type = EVENT_SETSPEED;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::getSpeed()
{
    EventData eventData;
    eventData.type = EVENT_GETSPEED;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::connectWifi_ST()
{
    EventData eventData;
    eventData.type = EVENT_CONNECTWIFI_ST;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::disConnectWifi_ST()
{
    EventData eventData;
    eventData.type = EVENT_DISCONNECTWIFI_ST;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::connectWifi_AP()
{
    EventData eventData;
    eventData.type = EVENT_CONNECTWIFI_AP;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::disConnectWifi_AP()
{
    EventData eventData;
    eventData.type = EVENT_DISCONNECTWIFI_AP;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::saveSettings()
{
    EventData eventData;
    eventData.type = EVENT_SAVE_SETTINGS;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::setContrast()
{
    EventData eventData;
    eventData.type = EVENT_SETCONTRAST;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::triggerAlarm(uint32_t taskId, TimeTaskType type, RepeatType repeatType) {
    EventData eventData;
    eventData.type = EVENT_ALARM_TRIGGER;
    eventData.data.alarm.taskId = taskId;
    eventData.data.alarm.type = type;
    eventData.data.alarm.repeatType = repeatType;
    
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

void EventHandle::syncTime() {

    EventData eventData;
    eventData.type = EVENT_SYNC_TIME;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
} 

void EventHandle::printAlarms() {
    EventData eventData;
    eventData.type = EVENT_PRINT_ALARMS;
    eventData.data.alarm = {};
    xQueueSend(eventQueue, &eventData, portMAX_DELAY);
}

EventHandle::~EventHandle()
{
   
}