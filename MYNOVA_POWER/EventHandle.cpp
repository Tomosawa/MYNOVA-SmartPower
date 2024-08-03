/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "EventHandle.h"
#include "GUIRender.h"
#include "pmbus.h"

extern GUIRender guiRender;
extern int displayPage;
extern PMBus psu;
extern WIFINetwork wifiNetwork;

QueueHandle_t eventQueue;
EventHandle::EventHandle()
{
}

void EventHandle::eventTask(void *param)
{
    EventType event;
    while (1)
    {
        if (xQueueReceive(eventQueue, &event, portMAX_DELAY))
        {
            // 处理事件
            switch (event)
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
                    break;
                case EVENT_GETSPEED:
                    guiRender.FanSpeed = psu.readFanCommand();
                    break;
                case EVENT_CONNECTWIFI:
                    if(wifiNetwork.getWIFIState() == WL_CONNECTED)
                        wifiNetwork.disconnectWIFI();
                    if(wifiNetwork.ReadConfig())
                        wifiNetwork.startWIFI();
                    break;
                case EVENT_DISCONNECTWIFI:
                    wifiNetwork.disconnectWIFI();
                    break;
                case EVENT_SAVEWIFI:
                    wifiNetwork.SaveConfig();
                    wifiNetwork.wifi_json = wifiNetwork.getAllSSIDJson();
                    break;
            }
        }
    }
}

void EventHandle::init()
{
    eventQueue = xQueueCreate(10, sizeof(EventType));
    xTaskCreate(EventHandle::eventTask, "EventTaskFunc", 2048, NULL, 1, NULL);
}

void EventHandle::powerOn()
{
     // 启动电源
    EventType event = EVENT_POWERON;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

void EventHandle::powerOff()
{
     // 关闭电源
    EventType event = EVENT_POWEROFF;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

void EventHandle::setSpeed(int value)
{
    guiRender.FanSpeed = value;
    EventType event = EVENT_SETSPEED;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

void EventHandle::getSpeed()
{
    EventType event = EVENT_GETSPEED;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

void EventHandle::connectWifi()
{
    EventType event = EVENT_CONNECTWIFI;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

void EventHandle::disConnectWifi()
{
    EventType event = EVENT_DISCONNECTWIFI;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

void EventHandle::saveWifi()
{
    EventType event = EVENT_SAVEWIFI;
    xQueueSend(eventQueue, &event, portMAX_DELAY);
}

EventHandle::~EventHandle()
{
   
}