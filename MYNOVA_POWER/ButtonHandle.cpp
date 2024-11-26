/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "ButtonHandle.h"
#include "IOPin.h"
#include "GUIRender.h"
#include "pmbus.h"
#include "ESP32TimerInterrupt.h"
#include "EventHandle.h"
#include "Settings.h"

ESP32Timer esp32Timer(1);
extern GUIRender guiRender;
extern int displayPage;
extern PMBus psu;
extern Settings settings;
EventHandle eventHandle;
//QueueHandle_t eventQueue;
bool TimerHandle(void *timeNo)
{
    button_ticks();
    return true;
}

ButtonHandle::ButtonHandle()
{
}

void ButtonHandle::init()
{
    // 针脚初始化
    pinMode(PIN_BUTTON_L, INPUT_PULLUP);
    pinMode(PIN_BUTTON_R, INPUT_PULLUP);

    // 按钮初始化
    pBtnLeft = new Button();
    button_init(pBtnLeft, PIN_BUTTON_L, 0);
    button_attach(pBtnLeft, SINGLE_CLICK, Click_Handle_BtnLeft);
    button_attach(pBtnLeft, LONG_RRESS_START, LongPressStart_Handler_BtnLeft);
    button_start(pBtnLeft);

    pBtnRight = new Button();
    button_init(pBtnRight, PIN_BUTTON_R, 0);
    button_attach(pBtnRight, SINGLE_CLICK, Click_Handle_BtnRight);
    button_attach(pBtnRight, LONG_RRESS_START, LongPressStart_Handler_BtnRight);
    button_start(pBtnRight);

    if (esp32Timer.attachInterruptInterval(TICKS_INTERVAL * 1000, TimerHandle))
    {
        Serial.print("Button timer started!\r\n");
    }

    eventHandle.init();
}

void ButtonHandle::Click_Handle_BtnLeft(void *btn)
{
    // 开机状态才操作
    if (displayPage > 0 && displayPage < 4)
    {
        --displayPage;
        if (displayPage < 1)
            displayPage = 3;
    }
    else if (displayPage == -1)
    {
       eventHandle.powerOn();
    }
    else if (displayPage == -2)
    {
        eventHandle.powerOff();
    }
    else if (displayPage == 4)
    { // 当前页在温度页面
        if (guiRender.FanSpeed - 10 > 0)
        {
            guiRender.FanSpeed -= 10;
        }
        else
        {
            guiRender.FanSpeed = 0;
        }
        eventHandle.setSpeed(guiRender.FanSpeed);
    }
    else if (displayPage == 10)
    {
        //当前在设置页面
       if(guiRender.menuSel == 0)
       {    //WIFI AP
            guiRender.leftBtnFrame = 3;
            settings.enable_AP = !settings.enable_AP;
            if(settings.enable_AP && !settings.AP_ssid.isEmpty() && !settings.AP_password.isEmpty())
            {
                eventHandle.connectWifi_AP();
            }
            else if(!settings.enable_AP)
            {
                eventHandle.disConnectWifi_AP();
            }
            eventHandle.saveSettings();
       }
       else if(guiRender.menuSel == 1)
       {    //WIFI ST
            guiRender.leftBtnFrame = 3;
            settings.enable_ST = !settings.enable_ST;
            if(settings.enable_ST && !settings.ST_ssid.isEmpty())
            {
                eventHandle.connectWifi_ST();
            }
            else if(!settings.enable_ST)
            {
                eventHandle.disConnectWifi_ST();
            }
            eventHandle.saveSettings();
       }
       else if(guiRender.menuSel == 2)
       {    //BRIGHTNESS
            guiRender.leftBtnFrame = 3;
            if(settings.brightness >= 100)
                settings.brightness = 0;
            settings.brightness += 10;
            eventHandle.setContrast();
            eventHandle.saveSettings();
       }
       else if(guiRender.menuSel == 3)
       {    //AC POWER
            guiRender.leftBtnFrame = 3;
            settings.AC_PowerON = !settings.AC_PowerON;
            eventHandle.saveSettings();
       }
       else if(guiRender.menuSel == 4)
       {    //EXIT
            displayPage = 3;
       }
    }
}
void ButtonHandle::LongPressStart_Handler_BtnLeft(void *btn)
{
   LongPressStart_Handler_BtnRight(btn);
}
void ButtonHandle::Click_Handle_BtnRight(void *btn)
{
    // 开机状态才操作
    if (displayPage > 0 && displayPage < 4)
    {
        ++displayPage;
        if (displayPage > 3)
            displayPage = 1;
    }
    else if (displayPage == -1)
    {
        // 取消启动
        displayPage = 0;
    }
    else if (displayPage == -2)
    {
        // 取消关机
        displayPage = 1;
    }
    else if (displayPage == 0)
    {
        //显示启动确认
        displayPage = -1;
    }
    else if (displayPage == 4)
    { // 当前页在温度页面
        if (guiRender.FanSpeed + 10 <= 100)
        {
            guiRender.FanSpeed += 10;
        }
        else
        {
            guiRender.FanSpeed = 10;
        }
        eventHandle.setSpeed(guiRender.FanSpeed);
    }
    else if (displayPage == 10)
    {
        //当前在设置页面
        guiRender.menuSel++;
        if(guiRender.menuSel < 5)
        {
            guiRender.animeFrame = 10;
            guiRender.frameDown = 2;
        }
        else
        {    
            guiRender.animeFrame = 10 * 4; 
            guiRender.frameDown = 8;
            guiRender.menuSel = 0;
        }
    }
}
void ButtonHandle::LongPressStart_Handler_BtnRight(void *btn)
{
    if (displayPage == 1)
    { // 当前是开启状态
        // 显示关机确认页面
        displayPage = -2;
    }
    else if (displayPage == 2)
    { // 当前页在温度页面
        // 显示温度设置
        eventHandle.getSpeed();
        displayPage = 4;
    }
    else if(displayPage == 3)
    {// 当前在网络页面
        displayPage = 10;//显示设置页面
    }
}

ButtonHandle::~ButtonHandle()
{
    if (pBtnLeft != NULL)
        delete pBtnLeft;
    if (pBtnRight != NULL)
        delete pBtnRight;
}