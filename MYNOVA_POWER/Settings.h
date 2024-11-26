/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef __SETTINGS__H__
#define __SETTINGS__H__
#include <Arduino.h>
#include <vector>
#include "TaskManager.h"  // 需要包含任务相关的枚举定义

class Settings
{
public:
    Settings();
    void ClearSettings();
    void ReadSettings();
    void SaveSettings();
    void RestoreTasks(TaskManager& taskManager);
    void UpdateTasksFromManager(const TaskManager& taskManager);
public:
    bool enable_AP;
    bool enable_ST;
    int brightness;
    String ST_ssid;        // WIFI的SSID
    String ST_password;    // WIFI的密码
    String AP_ssid;        // AP-SSID
    String AP_password;    // AP密码
    bool AC_PowerON;       //上电开机
private:
    std::vector<PeriodTask> periodTasks;
    std::vector<ConditionTask> conditionTasks;
};

#endif