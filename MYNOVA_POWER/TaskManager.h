/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>
#include <vector>
#include <map>
#include "esp_timer.h"

// 枚举定义
enum class RepeatType {
    ONCE,           // 只执行一次
    DAILY,          // 每天执行
    WEEKLY          // 每周自定义
};

enum class ConditionType {
    TYPE_INPUT,    // 输入
    TYPE_OUTPUT   // 输出
};

enum class ConditionQuantity {
    QUANTITY_CURRENT,         // 电流
    QUANTITY_POWER            // 功率
};

enum class CompareType {
    GREATER_THAN,   // 大于
    LESS_THAN       // 小于
};

enum class TimeTaskType {
    PERIOD,         // 时间段任务（包含开始和结束时间）
    TIMER_ON,       // 定时开机
    TIMER_OFF       // 定时关机
};

enum WeekDay {
    SUNDAY = 0x01,    // 0b00000001
    MONDAY = 0x02,    // 0b00000010
    TUESDAY = 0x04,   // 0b00000100
    WEDNESDAY = 0x08, // 0b00001000
    THURSDAY = 0x10,  // 0b00010000
    FRIDAY = 0x20,    // 0b00100000
    SATURDAY = 0x40   // 0b01000000
};

// 结构体定义
struct PeriodTask {
    uint32_t id;                // 任务ID
    TimeTaskType type;          // 任务类型
    uint8_t startHour;         // 开始小时
    uint8_t startMinute;       // 开始分钟
    uint8_t endHour;           // 结束小时
    uint8_t endMinute;         // 结束分钟
    RepeatType repeatType;     // 重复类型
    uint8_t weekDays;          // 周几执行
    bool enabled;              // 是否启用
};

struct ConditionTask {
    uint32_t id;                // 任务ID
    ConditionType conditionType;     // 条件类型
    ConditionQuantity conditionQuantity; // 条件量
    CompareType compareType;    // 比较类型
    float conditionValue;            // 阈值
    uint16_t conditionMinutes;           // 持续分钟数
    bool enabled;               // 是否启用
    unsigned long startTime;    // 条件开始满足的时间
};


class TaskManager {
public:
    // 构造和析构
    TaskManager();
    ~TaskManager();

    // 公共方法
    void start();
    void setShutdownTimer(uint32_t sec);
    void cancelShutdownTimer();
    void setGetValueCallback(float (*callback)(ConditionType type, ConditionQuantity quantity));
    void setupNextAlarm(const PeriodTask& task);
    void cancelAlarm(const PeriodTask& task);
    void printAllAlarms();
    // 任务管理方法
    uint32_t addPeriodTask(TimeTaskType type, uint8_t startHour, uint8_t startMinute,
                          uint8_t endHour, uint8_t endMinute,
                          RepeatType repeatType, uint8_t weekDays, bool enabled);
    void removePeriodTask(uint32_t taskId);
    uint32_t addConditionTask(ConditionType conditionType, ConditionQuantity conditionQuantity, CompareType compareType,
                            float conditionValue, uint16_t conditionMinutes, bool enabled);
    void removeConditionTask(uint32_t taskId);
    void togglePeriodTask(uint32_t taskId, bool enabled);
    void toggleConditionTask(uint32_t taskId, bool enabled);
    void clearAllTasksAndAlarms();

    // 辅助方法声明
    static uint8_t convertWeekDaysArrayToBitmap(bool weekDays[7]);
    static void convertBitmapToWeekDaysArray(uint8_t bitmap, bool weekDays[7]);
    static String convertBigmapToWeekDaysString(uint8_t bitmap);
    // JSON相关方法
    String getAllTasksJson();

    // 公共成员变量
    std::vector<PeriodTask> periodTasks;
    std::vector<ConditionTask> conditionTasks;
    unsigned long _shutdownTime;      // 剩余秒数
    bool _shutdownTimerActive;

    struct AlarmInfo {
        uint32_t taskId;
        esp_timer_handle_t timer;
        TimeTaskType type;
        RepeatType repeatType;
        uint8_t weekDays;
        TaskManager* manager;
    };
    std::map<uint32_t, AlarmInfo> _alarms;

private:
    // 私有方法
    static void alarmCallback(void* arg);
    static void taskFunction(void* parameter);
    void checkTasks();
    void checkConditionTasks();
    bool isWeekDayMatch(uint8_t weekDays);
    uint32_t generateTaskId();
    void createAlarm(uint32_t id, time_t alarmTime, TimeTaskType type, 
                            RepeatType repeatType, uint8_t weekDays);
    void deleteAlarm(uint32_t id);
    // 私有成员变量
    TaskHandle_t _taskHandle;
    float (*_getValueCallback)(ConditionType type, ConditionQuantity quantity);
    uint32_t _lastTaskId;
};

#endif 