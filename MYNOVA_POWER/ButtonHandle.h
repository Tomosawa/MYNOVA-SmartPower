/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef __BUTTONHANDLE_H_
#define __BUTTONHANDLE_H_
#pragma once
#include <Arduino.h>
#include "multi_button.h"
#include "GUIRender.h"

class ButtonHandle
{
public:
    ButtonHandle();
    ~ButtonHandle();
    void init();
private:
    static void Click_Handle_BtnLeft(void *btn);
    static void Click_Handle_BtnRight(void *btn);
    static void LongPressStart_Handler_BtnLeft(void *btn);
    static void LongPressStart_Handler_BtnRight(void *btn);
private:
    Button* pBtnLeft;
    Button* pBtnRight;
public:
};

#endif