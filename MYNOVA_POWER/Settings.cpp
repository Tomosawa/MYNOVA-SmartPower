/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "Settings.h"
#include <Preferences.h>

#define SETTINGS_KEY "MYNOVA"
Settings::Settings()
{
    enable_AP = true;
    enable_ST = true;
    brightness = 100;
    ST_ssid = "";// WIFI的SSID
    ST_password = "";// WIFI的密码
    AP_ssid = "MYNOVA";// AP-SSID
    AP_password = "MyNova123";// AP密码
    AC_PowerON = false;
}

void Settings::ClearSettings()
{
    periodTasks.clear();
    conditionTasks.clear();
    Preferences prefs;
    prefs.begin(SETTINGS_KEY);
    prefs.clear();
    prefs.end();
}

void Settings::ReadSettings()
{
    Preferences prefs;
    prefs.begin(SETTINGS_KEY);

    enable_AP =     prefs.getBool("ENABLE_AP", true);
    enable_ST =     prefs.getBool("ENABLE_ST", true);
    brightness =    prefs.getInt("BRIGHTNESS", 100);
    AP_ssid =       prefs.getString("AP_SSID", "MYNOVA");
    AP_password =   prefs.getString("AP_PASSWORD", "MyNova123");
    ST_ssid =       prefs.getString("ST_SSID", "");
    ST_password =   prefs.getString("ST_PASSWORD", "");
    AC_PowerON =    prefs.getBool("AC_POWERON", false);

    // 读取任务数量
    size_t periodCount = prefs.getUInt("PERIOD_COUNT", 0);
    size_t condCount = prefs.getUInt("COND_COUNT", 0);

    // 清空现有任务列表
    periodTasks.clear();
    conditionTasks.clear();

    // 读取时间段任务
    for(size_t i = 0; i < periodCount; i++) {
        String prefix = "PERIOD_" + String(i) + "_";
        PeriodTask task;
        task.type = static_cast<TimeTaskType>(prefs.getInt((prefix + "TYPE").c_str(), 0));
        task.startHour = prefs.getUChar((prefix + "SH").c_str(), 0);
        task.startMinute = prefs.getUChar((prefix + "SM").c_str(), 0);
        task.endHour = prefs.getUChar((prefix + "EH").c_str(), 0);
        task.endMinute = prefs.getUChar((prefix + "EM").c_str(), 0);
        task.repeatType = static_cast<RepeatType>(prefs.getInt((prefix + "REP").c_str(), 0));
        task.weekDays = prefs.getUChar((prefix + "WEEK").c_str(), 0);
        task.enabled = prefs.getBool((prefix + "ENB").c_str(), true);
        periodTasks.push_back(task);
    }

    // 读取条件任务
    for(size_t i = 0; i < condCount; i++) {
        String prefix = "COND_" + String(i) + "_";
        ConditionTask task;
        task.conditionType = static_cast<ConditionType>(prefs.getInt((prefix + "TYPE").c_str(), 0));
        task.conditionQuantity = static_cast<ConditionQuantity>(prefs.getInt((prefix + "QUAN").c_str(), 0));
        task.compareType = static_cast<CompareType>(prefs.getInt((prefix + "CMP").c_str(), 0));
        task.conditionValue = prefs.getFloat((prefix + "THR").c_str(), 0);
        task.conditionMinutes = prefs.getUShort((prefix + "MIN").c_str(), 0);
        task.enabled = prefs.getBool((prefix + "ENB").c_str(), true);
        conditionTasks.push_back(task);
    }

    prefs.end();
}

void Settings::SaveSettings()
{
    Preferences prefs;
    prefs.begin(SETTINGS_KEY);

    prefs.putBool("ENABLE_AP", enable_AP);
    prefs.putBool("ENABLE_ST", enable_ST);
    prefs.putInt("BRIGHTNESS", brightness);
    prefs.putString("AP_SSID", AP_ssid);
    prefs.putString("AP_PASSWORD", AP_password);
    prefs.putString("ST_SSID", ST_ssid);
    prefs.putString("ST_PASSWORD", ST_password);
    prefs.putBool("AC_POWERON", AC_PowerON);

    // 保存任务数量
    prefs.putUInt("PERIOD_COUNT", periodTasks.size());
    prefs.putUInt("COND_COUNT", conditionTasks.size());

    // 保存时间段任务
    for(size_t i = 0; i < periodTasks.size(); i++) {
        String prefix = "PERIOD_" + String(i) + "_";
        const auto& task = periodTasks[i];
        prefs.putInt((prefix + "TYPE").c_str(), static_cast<int>(task.type));
        prefs.putUChar((prefix + "SH").c_str(), task.startHour);
        prefs.putUChar((prefix + "SM").c_str(), task.startMinute);
        prefs.putUChar((prefix + "EH").c_str(), task.endHour);
        prefs.putUChar((prefix + "EM").c_str(), task.endMinute);
        prefs.putInt((prefix + "REP").c_str(), static_cast<int>(task.repeatType));
        prefs.putUChar((prefix + "WEEK").c_str(), task.weekDays);
        prefs.putBool((prefix + "ENB").c_str(), task.enabled);
    }

    // 保存条件任务
    for(size_t i = 0; i < conditionTasks.size(); i++) {
        String prefix = "COND_" + String(i) + "_";
        const auto& task = conditionTasks[i];
        prefs.putInt((prefix + "TYPE").c_str(), static_cast<int>(task.conditionType));
        prefs.putInt((prefix + "QUAN").c_str(), static_cast<int>(task.conditionQuantity));
        prefs.putInt((prefix + "CMP").c_str(), static_cast<int>(task.compareType));
        prefs.putFloat((prefix + "THR").c_str(), task.conditionValue);
        prefs.putUShort((prefix + "MIN").c_str(), task.conditionMinutes);
        prefs.putBool((prefix + "ENB").c_str(), task.enabled);
    }

    prefs.end();
}

// 添加恢复任务的实现
void Settings::RestoreTasks(TaskManager& taskManager) {
    taskManager.clearAllTasksAndAlarms();
    // 恢复时间段任务
    for(const auto& task : periodTasks) {
        taskManager.addPeriodTask(
            task.type,
            task.startHour,
            task.startMinute,
            task.endHour,
            task.endMinute,
            task.repeatType,
            task.weekDays,
            task.enabled
        );
    }
    
    // 恢复条件任务
    for(const auto& task : conditionTasks) {
        taskManager.addConditionTask(
            task.conditionType,
            task.conditionQuantity,
            task.compareType,
            task.conditionValue,
            task.conditionMinutes,
            task.enabled
        );
    }
}

void Settings::UpdateTasksFromManager(const TaskManager& taskManager) {
    // 清空现有任务列表
    periodTasks.clear();
    conditionTasks.clear();
    
    const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* taskTypes[] = {"Period", "Timer ON", "Timer OFF"};
    const char* repeatTypes[] = {"Once", "Daily", "Weekly"};

    // 获取并转换时间段任务
    for (const auto& task : taskManager.periodTasks) {
        PeriodTask savedTask;
        savedTask.type = task.type;
        savedTask.startHour = task.startHour;
        savedTask.startMinute = task.startMinute;
        savedTask.endHour = task.endHour;
        savedTask.endMinute = task.endMinute;
        savedTask.repeatType = task.repeatType;
        savedTask.weekDays = task.weekDays;
        savedTask.enabled = task.enabled;
        periodTasks.push_back(savedTask);
        
        // 打印基本信息
        Serial.printf("Saved period task: Type=%s, Start=%02d:%02d, End=%02d:%02d, Repeat=%s, Enabled=%d", 
                     taskTypes[static_cast<int>(savedTask.type)],
                     savedTask.startHour, savedTask.startMinute, 
                     savedTask.endHour, savedTask.endMinute,
                     repeatTypes[static_cast<int>(savedTask.repeatType)],
                     savedTask.enabled);
        
        // 如果是每周重复，打印启用的星期
        if (savedTask.repeatType == RepeatType::WEEKLY) {
            Serial.print(", Weekdays=[");
            bool first = true;
            for (int i = 0; i < 7; i++) {
                if (savedTask.weekDays & (1 << i)) {
                    if (!first) Serial.print(",");
                    Serial.print(weekdays[i]);
                    first = false;
                }
            }
            Serial.print("]");
        }
        Serial.println();
    }
    
    // 获取并转换条件任务
    for (const auto& task : taskManager.conditionTasks) {
        ConditionTask savedTask;
        savedTask.conditionType = task.conditionType;
        savedTask.conditionQuantity = task.conditionQuantity;
        savedTask.compareType = task.compareType;
        savedTask.conditionValue = task.conditionValue;
        savedTask.conditionMinutes = task.conditionMinutes;
        savedTask.enabled = task.enabled;
        conditionTasks.push_back(savedTask);
        Serial.printf("Saved condition task: Type=%d, Quantity=%d, Threshold=%.2f, Enabled=%d\n", 
                     static_cast<int>(savedTask.conditionType), static_cast<int>(savedTask.conditionQuantity), savedTask.conditionValue, savedTask.enabled);
    }
}