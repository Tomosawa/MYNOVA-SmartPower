/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef __PAGETIMER__H__
#define __PAGETIMER__H__
#include <Arduino.h>

class PageTimer
{
public:
    PageTimer();
    bool checkTimeOut(u_long millisecond);
    void resetTimer();
private:
    u_long lastTime = 0;  // 记录上次计时时间
    bool timerInitialized = false;  // 标识计时器是否初始化
};

#endif