/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "TaskManager.h"
#include <ArduinoJson.h>
#include "EventHandle.h"

extern EventHandle eventHandle;
extern bool powerOn;
TaskManager::TaskManager() {
    _shutdownTimerActive = false;
    _shutdownTime = 0;
    _lastTaskId = 0;
    _getValueCallback = nullptr;
    Serial.println("TaskManager initialized");
    for (auto& alarm : _alarms) {
        alarm.second.manager = this;
    }
}

void TaskManager::start() {
    xTaskCreatePinnedToCore(
        taskFunction,
        "TaskManager",
        4096,
        this,
        15,
        &_taskHandle,
        0
    );
}

void TaskManager::setShutdownTimer(uint32_t seconds) {
    _shutdownTimerActive = true;
    _shutdownTime = seconds;
    Serial.printf("Shutdown timer set: %d minutes (%d seconds)\n", _shutdownTime / 60, _shutdownTime);
}

void TaskManager::cancelShutdownTimer() {
    _shutdownTimerActive = false;
    _shutdownTime = 0;
    Serial.println("Shutdown timer cancelled");
}

// 任务检查函数
void TaskManager::taskFunction(void* parameter) {
    TaskManager* self = static_cast<TaskManager*>(parameter);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while (true) {
        if(powerOn){
            self->checkTasks();
        }
        // 使用 vTaskDelayUntil 替代 vTaskDelay 以获得更精确的周期
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}

void TaskManager::checkTasks() {
    // 检查倒计时关机
    if (_shutdownTimerActive) {
        if (_shutdownTime > 0) {
            _shutdownTime--;  // 每秒减1
            if (_shutdownTime % 60 == 0) {  // 每分钟打印一次日志
                Serial.printf("Shutdown timer: %d minutes remaining\n", _shutdownTime / 60);
            }
            
            if (_shutdownTime <= 0) {
                Serial.println("Shutdown timer expired, powering off");
                eventHandle.powerOff();
                _shutdownTimerActive = false;
            }
        }
    }
    // 检查条件任务
    checkConditionTasks();
}


bool TaskManager::isWeekDayMatch(uint8_t weekDays) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return false;
    }
    
    // timeinfo.tm_wday 范围是 0-6，0=周日
    uint8_t today = (1 << timeinfo.tm_wday); // 将今天转换为位图
    return (weekDays & today) != 0; // 检查今天是否在选中的日期中
}

// 时间段任务管理
uint32_t TaskManager::addPeriodTask(TimeTaskType type, uint8_t startHour, uint8_t startMinute,
                                   uint8_t endHour, uint8_t endMinute,
                                   RepeatType repeatType, uint8_t weekDays, bool enabled) {
    uint32_t taskId = generateTaskId();
    
    // 添加到任务列表
    periodTasks.push_back({
        taskId, type, startHour, startMinute, endHour, endMinute, 
        repeatType, weekDays, enabled
    });
    if(enabled){
        // 设置闹钟
        setupNextAlarm(periodTasks.back());
    }
    return taskId;
}

void TaskManager::setupNextAlarm(const PeriodTask& task) {
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    if (task.type == TimeTaskType::TIMER_ON || task.type == TimeTaskType::TIMER_OFF) {
        // 处理单次定时任务（开机或关机）
        struct tm alarmTime = timeinfo;
        alarmTime.tm_hour = task.type == TimeTaskType::TIMER_ON ? task.startHour : task.endHour;
        alarmTime.tm_min = task.type == TimeTaskType::TIMER_ON ? task.startMinute : task.endMinute;
        alarmTime.tm_sec = 0;
        time_t targetTime = mktime(&alarmTime);
        
        // 根据重复类型计算下一次闹钟时间
        if (targetTime <= now) {
            switch (task.repeatType) {
                case RepeatType::DAILY:
                    targetTime += 24 * 60 * 60;  // 明天同一时间
                    break;
                    
                case RepeatType::WEEKLY:
                    // 找到下一个符合的星期
                    do {
                        targetTime += 24 * 60 * 60;
                        localtime_r(&targetTime, &alarmTime);
                    } while (!(task.weekDays & (1 << alarmTime.tm_wday)));
                    break;
                    
                case RepeatType::ONCE:
                    if (targetTime <= now) {
                        targetTime += 24 * 60 * 60;  // 已过期就设为明天
                    }
                    break;
            }
        }
        
        createAlarm(task.id, targetTime, task.type, task.repeatType, task.weekDays);
    }
    else if (task.type == TimeTaskType::PERIOD) {
        // 分别计算开始和结束时间
        struct tm startTime = timeinfo;
        startTime.tm_hour = task.startHour;
        startTime.tm_min = task.startMinute;
        startTime.tm_sec = 0;
        time_t startAlarmTime = mktime(&startTime);
        
        struct tm endTime = timeinfo;
        endTime.tm_hour = task.endHour;
        endTime.tm_min = task.endMinute;
        endTime.tm_sec = 0;
        time_t endAlarmTime = mktime(&endTime);
        
        // 如果结束时间小于开始时间，说明跨天
        if (endAlarmTime < startAlarmTime) {
            endAlarmTime += 24 * 60 * 60;
        }
        
        // 根据重复类型计算下一次闹钟时间
        if (endAlarmTime <= now) {
            switch (task.repeatType) {
                case RepeatType::DAILY:
                    startAlarmTime += 24 * 60 * 60;
                    endAlarmTime += 24 * 60 * 60;
                    break;
                    
                case RepeatType::WEEKLY:
                    do {
                        startAlarmTime += 24 * 60 * 60;
                        endAlarmTime += 24 * 60 * 60;
                        localtime_r(&startAlarmTime, &startTime);
                    } while (!(task.weekDays & (1 << startTime.tm_wday)));
                    break;
                    
                case RepeatType::ONCE:
                    if (endAlarmTime <= now) {
                        startAlarmTime += 24 * 60 * 60;
                        endAlarmTime += 24 * 60 * 60;
                    }
                    break;
            }
        }
        
        // 设置两个闹钟
        if (startAlarmTime > now) {
            createAlarm(task.id, startAlarmTime, TimeTaskType::TIMER_ON, task.repeatType, task.weekDays);
        }
        if (endAlarmTime > now) {
            createAlarm(task.id + 10000, endAlarmTime, TimeTaskType::TIMER_OFF, task.repeatType, task.weekDays);
        }
    }
}

void TaskManager::cancelAlarm(const PeriodTask& task) {
    if (task.type == TimeTaskType::TIMER_ON || task.type == TimeTaskType::TIMER_OFF) {
        deleteAlarm(task.id);
    } else if (task.type == TimeTaskType::PERIOD) {
        deleteAlarm(task.id);
        deleteAlarm(task.id + 10000);
    }
}

// 新增辅助方法来创建闹钟
void TaskManager::createAlarm(uint32_t id, time_t alarmTime, TimeTaskType type, 
                            RepeatType repeatType, uint8_t weekDays) {
    time_t now;
    time(&now);
    
    // 添加闹钟信息到map
    _alarms[id] = AlarmInfo{
        .taskId = id,
        .timer = nullptr,
        .type = type,
        .repeatType = repeatType,
        .weekDays = weekDays,
        .manager = this
    };
    
    AlarmInfo* alarmInfo = &_alarms[id];
    
    esp_timer_create_args_t timer_args = {
        .callback = &TaskManager::alarmCallback,
        .arg = alarmInfo,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "alarm_timer"
    };
    
    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_once(timer, (alarmTime - now) * 1000000ULL);
    
    alarmInfo->timer = timer;
    
    // 打印闹钟信息
    struct tm timeinfo;
    struct tm currentTime;
    time_t currentTimestamp;
    
    time(&currentTimestamp);
    localtime_r(&currentTimestamp, &currentTime);
    localtime_r(&alarmTime, &timeinfo);
    
    const char* taskTypes[] = {"Period", "Timer ON", "Timer OFF"};
    const char* weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
    Serial.printf("Alarm set: ID=%d, Type=%s\n"
                 "  Current: %04d-%02d-%02d %02d:%02d:%02d (%s)\n"
                 "  Target:  %04d-%02d-%02d %02d:%02d:%02d (%s)\n",
                 id,
                 taskTypes[static_cast<int>(type)],
                 currentTime.tm_year + 1900, currentTime.tm_mon + 1, currentTime.tm_mday,
                 currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec,
                 weekdays[currentTime.tm_wday],
                 timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                 weekdays[timeinfo.tm_wday]);
}

void TaskManager::deleteAlarm(uint32_t id) {
    // 先停止和删除定时器
    auto it = _alarms.find(id);
    if (it != _alarms.end() && it->second.timer != nullptr) {
        esp_timer_stop(it->second.timer);
        esp_timer_delete(it->second.timer);
        _alarms.erase(it);
    }
    
    Serial.printf("Delete alarm with ID: %d\n", id);
}

void IRAM_ATTR TaskManager::alarmCallback(void* arg) {
    AlarmInfo* alarmInfo = static_cast<AlarmInfo*>(arg);
    TaskManager* self = static_cast<TaskManager*>(alarmInfo->manager);
    
    // 通过事件处理器触发闹钟事件
    eventHandle.triggerAlarm(alarmInfo->taskId, alarmInfo->type, alarmInfo->repeatType);
}

void TaskManager::removePeriodTask(uint32_t taskId) {
    // 清理开始时间闹钟
    auto startIt = _alarms.find(taskId);
    if (startIt != _alarms.end()) {
        if (startIt->second.timer != nullptr) {
            esp_timer_stop(startIt->second.timer);
            esp_timer_delete(startIt->second.timer);
        }
        _alarms.erase(startIt);
    }
    
    // 清理结束时间闹钟
    auto endIt = _alarms.find(taskId + 10000);
    if (endIt != _alarms.end()) {
        if (endIt->second.timer != nullptr) {
            esp_timer_stop(endIt->second.timer);
            esp_timer_delete(endIt->second.timer);
        }
        _alarms.erase(endIt);
    }
    
    // 删除任务
    auto taskIt = std::find_if(periodTasks.begin(), periodTasks.end(),
        [taskId](const PeriodTask& task) { return task.id == taskId; });
    if (taskIt != periodTasks.end()) {
        periodTasks.erase(taskIt);
    }

    Serial.printf("Remove period task: %d\n", taskId);
}

// 条件任务管理
uint32_t TaskManager::addConditionTask(ConditionType conditionType, ConditionQuantity conditionQuantity, CompareType compareType,
                                     float conditionValue, uint16_t conditionMinutes, bool enabled) {
    uint32_t taskId = generateTaskId();
    conditionTasks.push_back({
        taskId, conditionType, conditionQuantity, compareType, conditionValue, conditionMinutes, 
        enabled
    });
    

    Serial.printf("Condition task added: ID=%d, Type=%d, Quantity=%d, Threshold=%.2f\n", 
                 taskId, static_cast<int>(conditionType), static_cast<int>(conditionQuantity), conditionValue);
    return taskId;
}

void TaskManager::removeConditionTask(uint32_t taskId) {
    conditionTasks.erase(
        std::remove_if(conditionTasks.begin(), conditionTasks.end(),
            [taskId](const ConditionTask& task) { return task.id == taskId; }),
        conditionTasks.end());

}

void TaskManager::setGetValueCallback(float (*callback)(ConditionType type, ConditionQuantity quantity)) {
    _getValueCallback = callback;
}

uint32_t TaskManager::generateTaskId() {
    return ++_lastTaskId;
}

void TaskManager::checkConditionTasks() {
    if (!_getValueCallback) {
        Serial.println("No value callback set for condition tasks");
        return;
    }

    unsigned long currentTime = millis();
    
    for (auto& task : conditionTasks) {
        if (!task.enabled) {
            Serial.printf("Condition task %d is disabled, skipping\n", task.id);
            continue;
        }

        float currentValue = _getValueCallback(task.conditionType, task.conditionQuantity);
        Serial.printf("Checking condition taskId %d: conditionType=%d, conditionQuantity=%d, currentValue=%.2f, conditionValue=%.2f\n", 
                     task.id, static_cast<int>(task.conditionType), static_cast<int>(task.conditionQuantity), currentValue, task.conditionValue);

        bool conditionMet = false;
        if (task.compareType == CompareType::GREATER_THAN) {
            conditionMet = currentValue > task.conditionValue;
        } else {
            conditionMet = currentValue < task.conditionValue;
        }

        if (conditionMet) {
            if (task.startTime == 0) {
                Serial.printf("Condition met for task %d, starting timer\n", task.id);
                task.startTime = currentTime;
            } else {
                unsigned long elapsedMinutes = (currentTime - task.startTime) / 60000;
                Serial.printf("Task %d condition still met, elapsed time: %lu minutes\n", 
                             task.id, elapsedMinutes);
                
                if ((currentTime - task.startTime) >= (task.conditionMinutes * 60000)) {
                    Serial.printf("Executing condition task %d action\n", task.id);
                    eventHandle.powerOff();
                    task.startTime = 0;
                }
            }
        } else {
            if (task.startTime != 0) {
                Serial.printf("Condition no longer met for task %d, resetting timer\n", task.id);
                task.startTime = 0;
            }
        }
    }
}

String TaskManager::getAllTasksJson() {
    JsonDocument doc;
    
    // 添加倒计时关机状态
    JsonObject shutdownTimer = doc.createNestedObject("shutdownTimer");
    shutdownTimer["active"] = _shutdownTimerActive;
    shutdownTimer["remainingSeconds"] = _shutdownTime;
    
    // 添加时间段任务
    JsonArray periodArray = doc.createNestedArray("periodTasks");
    for (const auto& task : periodTasks) {
        JsonObject taskObj = periodArray.createNestedObject();
        taskObj["id"] = task.id;
        taskObj["type"] = static_cast<int>(task.type);  // 娣诲姞浠诲姟绫诲瀷
        taskObj["startHour"] = task.startHour;
        taskObj["startMinute"] = task.startMinute;
        taskObj["endHour"] = task.endHour;
        taskObj["endMinute"] = task.endMinute;
        taskObj["repeatType"] = static_cast<int>(task.repeatType);
        
        // 将位图转换为数组
        JsonArray weekDaysArray = taskObj.createNestedArray("weekDays");
        for (int i = 0; i < 7; i++) {
            if (task.weekDays & (1 << i)) {
                weekDaysArray.add(i);  // 添加0-6，对应周日开始周一到周六
            }
        }
        
        taskObj["enabled"] = task.enabled;
    }
    
    // 添加条件任务
    JsonArray condArray = doc.createNestedArray("conditionTasks");
    for (const auto& task : conditionTasks) {
        JsonObject taskObj = condArray.createNestedObject();
        taskObj["id"] = task.id;
        taskObj["conditionType"] = static_cast<int>(task.conditionType);
        taskObj["conditionQuantity"] = static_cast<int>(task.conditionQuantity);
        taskObj["compareType"] = static_cast<int>(task.compareType);
        taskObj["conditionValue"] = task.conditionValue;
        taskObj["conditionMinutes"] = task.conditionMinutes;
        taskObj["enabled"] = task.enabled;
    }
    
    // 添加当前系统时间（格式化为 yyyy/MM/dd HH:mm:ss）
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%04d/%02d/%02d %02d:%02d:%02d",
        timeinfo.tm_year + 1900,
        timeinfo.tm_mon + 1,
        timeinfo.tm_mday,
        timeinfo.tm_hour,
        timeinfo.tm_min,
        timeinfo.tm_sec);
    
    doc["currentTime"] = timeStr;

    String output;
    serializeJson(doc, output);
    return output;
}

void TaskManager::togglePeriodTask(uint32_t taskId, bool enabled) {
    for (auto& task : periodTasks) {
        if (task.id == taskId) {
            task.enabled = enabled;
            if(enabled){
                setupNextAlarm(task);
            }else{
                cancelAlarm(task);
            }
            Serial.printf("Period task %d %s\n", taskId, enabled ? "enabled" : "disabled");
            break;
        }
    }
}

void TaskManager::toggleConditionTask(uint32_t taskId, bool enabled) {
    for (auto& task : conditionTasks) {
        if (task.id == taskId) {
            task.enabled = enabled;
            if (!enabled) {
                task.startTime = 0;
            }
            Serial.printf("Condition task %d %s\n", taskId, enabled ? "enabled" : "disabled");
            break;
        }
    }
}

// 辅助方法：将星期数组转换为位图
uint8_t TaskManager::convertWeekDaysArrayToBitmap(bool weekDays[7]) {
    uint8_t bitmap = 0;
    for (int i = 0; i < 7; i++) {
        if (weekDays[i]) {
            bitmap |= (1 << i);
        }
    }
    return bitmap;
}

// 辅助方法：将位图转换为星期数组
void TaskManager::convertBitmapToWeekDaysArray(uint8_t bitmap, bool weekDays[7]) {
    for (int i = 0; i < 7; i++) {
        weekDays[i] = (bitmap & (1 << i)) != 0;
    }
}

String TaskManager::convertBigmapToWeekDaysString(uint8_t bitmap) {
    bool weekDays[7];
    convertBitmapToWeekDaysArray(bitmap, weekDays);
    String weekDaysStr = "";
    if(weekDays[0]){
        weekDaysStr += "Sun ";
    }
    if(weekDays[1]){
        weekDaysStr += "Mon ";
    }   
    if(weekDays[2]){
        weekDaysStr += "Tue ";
    }
    if(weekDays[3]){
        weekDaysStr += "Wed ";
    }
    if(weekDays[4]){
        weekDaysStr += "Thu ";
    }
    if(weekDays[5]){
        weekDaysStr += "Fri ";
    }
    if(weekDays[6]){
        weekDaysStr += "Sat ";
    }
    return weekDaysStr;
}

void TaskManager::printAllAlarms() {
    const char* taskTypes[] = {"Period", "Timer ON", "Timer OFF"};
    const char* repeatTypes[] = {"Once", "Daily", "Weekly"};
    //打印出所有的PeriodTask内容
    Serial.println("All Period tasks:");
    for (const auto& task : periodTasks) {
        Serial.printf("ID: %u\tStart: %02d:%02d\tEnd: %02d:%02d\tRepeat: %s\t Enabled: %s\n", 
                     task.id, task.startHour, task.startMinute, task.endHour, task.endMinute, repeatTypes[static_cast<int>(task.repeatType)], task.enabled ? "Yes" : "No");
    }   

    Serial.println("Current active timers:");
    for (const auto& pair : _alarms) {
        uint32_t id = pair.first;
        const AlarmInfo& alarm = pair.second;
        
        //WeakDays转换成星期的字符串
        String weekDaysStr = "";
        if(alarm.repeatType == RepeatType::WEEKLY){
            weekDaysStr = convertBigmapToWeekDaysString(alarm.weekDays);
        }
        Serial.printf("ID: %u\t Type: %s\t Repeat: %s\t WeakDays: %s\n", id,taskTypes[static_cast<int>(alarm.type)],repeatTypes[static_cast<int>(alarm.repeatType)],weekDaysStr.c_str());
        Serial.println("------------------------");
    }
    
    Serial.println("\nSystem Timer Stats:");
    // 打印系统定时器的整体信息
    esp_timer_dump(stdout);
}

void TaskManager::clearAllTasksAndAlarms() {
    periodTasks.clear();
    conditionTasks.clear();
    //停止所有alarm后删除
    for (auto& pair : _alarms) {
        if (pair.second.timer != nullptr) {
            esp_timer_stop(pair.second.timer);
            esp_timer_delete(pair.second.timer);
        }
    }
    _alarms.clear();
}

TaskManager::~TaskManager() {
    for (auto& pair : _alarms) {
        if (pair.second.timer != nullptr) {
            esp_timer_stop(pair.second.timer);
            esp_timer_delete(pair.second.timer);
        }
    }
    _alarms.clear();
    
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
    }
}
