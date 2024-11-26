/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "PageTimer.h"

PageTimer::PageTimer()
{

}

bool PageTimer::checkTimeOut(u_long millisecond)
{
    u_long currentTime = millis();  // 获取当前时间
    
    // 初始化计时器，只在第一次调用时执行
    if (!timerInitialized) {
        lastTime = currentTime;
        timerInitialized = true;
    }
    // 检查是否已经超过指定的时间
    if (currentTime - lastTime >= millisecond) {
        return true;  // 超时，返回 true
    }
    
    return false;  // 未超时，返回 false
}

void PageTimer::resetTimer()
{
    timerInitialized = false;
}