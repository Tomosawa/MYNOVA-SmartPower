#include "WebService.h"
#include <ArduinoJson.h>
#include "EventHandle.h"
#include "Settings.h"

extern Settings settings;
extern EventHandle eventHandle;
extern TaskManager taskManager;
extern bool powerOn;
void handleRequest(AsyncWebServerRequest *request){}
void handleUploadRequest(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){}
WebService::WebService() : server(80)
{
    domianURL = "mynova.com";
}

void WebService::init(IPAddress ServerIP, WIFINetwork *wifiNetwork, PMBus *psu)
{
    Server_IP = ServerIP;
    pWifiNetwork = wifiNetwork;
    pPSU = psu;
    dnsserver.start(53, domianURL, Server_IP); // 设置DNS的端口、网址、和IP
}

void WebService::serverStart()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    Serial.println("Server on");
    server.on("/api/wifiscan", HTTP_GET, std::bind(&WebService::handleWIFIScanRequest, this, std::placeholders::_1));
    server.on("/api/wifisave", HTTP_POST, handleRequest, handleUploadRequest, std::bind(&WebService::handleWIFISaveRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    server.on("/api/wificlear", HTTP_DELETE, std::bind(&WebService::handleWIFIClearRequest, this, std::placeholders::_1));
    server.on("/api/wificonnect", HTTP_GET, std::bind(&WebService::handleWIFIConnectRequest, this, std::placeholders::_1));
    //开启或关闭WIFI，wifienable
    server.on("/api/wifienable", HTTP_POST, handleRequest, handleUploadRequest,std::bind(&WebService::handleWIFIEnableRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    //获取AP&WIF热点信息
    server.on("/api/apwifiinfo", HTTP_GET, std::bind(&WebService::handleAPWIFIInfoRequest, this, std::placeholders::_1));
    //修改AP热点信息
    server.on("/api/apsave", HTTP_POST, handleRequest, handleUploadRequest, std::bind(&WebService::handleAPSaveRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    //开启或关闭AP热点
    server.on("/api/apenable", HTTP_POST, handleRequest, handleUploadRequest,std::bind(&WebService::handleAPEnableRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    server.on("/api/psuinfo", HTTP_GET, std::bind(&WebService::handlePSUInfoRequest, this, std::placeholders::_1));
    server.on("/api/psusetting", HTTP_GET, std::bind(&WebService::handlePSUSettingRequest, this, std::placeholders::_1));
    server.on("/api/psupower", HTTP_PUT, handleRequest, handleUploadRequest,std::bind(&WebService::handlePSUPowerRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    server.on("/api/psufan", HTTP_PUT, handleRequest, handleUploadRequest,std::bind(&WebService::handlePSUFanRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    // 上电自动开机
    server.on("/api/acpoweron", HTTP_POST, handleRequest, handleUploadRequest,std::bind(&WebService::handleACPowerOnRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    // 任务管理
    server.on("/api/tasks/all", HTTP_GET, std::bind(&WebService::handleTasksRequest, this, std::placeholders::_1));
    server.on("/api/shutdown/timer", HTTP_GET, 
        std::bind(&WebService::handleGetShutdownTimerRequest, this, std::placeholders::_1));
    
    server.on("/api/shutdown/timer", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleSetShutdownTimerRequest, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    server.on("/api/shutdown/timer", HTTP_DELETE,
        std::bind(&WebService::handleCancelShutdownTimerRequest, this, std::placeholders::_1));
    
    server.on("/api/tasks/period/add", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleAddPeriodTaskRequest, this, std::placeholders::_1, std::placeholders::_2,
                 std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    server.on("/api/tasks/condition/add", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleAddConditionTaskRequest, this, std::placeholders::_1, std::placeholders::_2,
                 std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    // 删除任务的路由
    server.on("/api/tasks/period/delete", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleDeletePeriodTaskRequest, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    server.on("/api/tasks/condition/delete", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleDeleteConditionTaskRequest, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    // 启用/禁用任务的路由
    server.on("/api/tasks/period/toggle", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleTogglePeriodTaskRequest, this, std::placeholders::_1, std::placeholders::_2,
                 std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    server.on("/api/tasks/condition/toggle", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleToggleConditionTaskRequest, this, std::placeholders::_1, std::placeholders::_2,
                 std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
    // 同步时间的路由
    server.on("/api/synctime", HTTP_POST, handleRequest, handleUploadRequest,
        std::bind(&WebService::handleSyncTimeRequest, this, std::placeholders::_1,
                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
    
     // 处理所有非 API 路由
    server.onNotFound([](AsyncWebServerRequest *request) {
        // 如果是 API 请求，返回 404
        if (request->url().startsWith("/api/")) {
            request->send(404);
            return;
        }
        // 其他所有请求都返回 index.html
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.begin();
}

void WebService::processRequest()
{
    dnsserver.processNextRequest();
}

void WebService::handleWIFIScanRequest(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", pWifiNetwork->wifi_json);
}

void WebService::handleWIFISaveRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String jsonStr =(char*)data;
  
    // 打印原始POST数据
    Serial.println(jsonStr);
    // 解析JSON数据
    JsonDocument doc; // 根据你的JSON数据大小调整容量
    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"result\":\"failed\",message:\"JSON parsing failed\"}");
        return;
    }
    // 处理解析后的JSON数据
    const char* ssid = doc["SSID"];
    const char* pwd = doc["PWD"];
    settings.ST_ssid = ssid;
    settings.ST_password = pwd;
    settings.enable_ST = true;
    eventHandle.saveSettings();
    // 返回响应
    request->send(200, "application/json", "{\"result\":\"OK\"}");

}

void WebService::handleWIFIClearRequest(AsyncWebServerRequest *request)
{
    eventHandle.disConnectWifi_ST();
    settings.ST_ssid = "";
    settings.ST_password = "";
    settings.enable_ST = false;
    eventHandle.saveSettings();
    // 返回响应
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleWIFIConnectRequest(AsyncWebServerRequest *request)
{
    eventHandle.connectWifi_ST();
    settings.enable_ST = true;
    eventHandle.saveSettings();
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleWIFIEnableRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String jsonStr =(char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"result\":\"failed\",message:\"JSON parsing failed\"}");
        return;
    }
    //开启或关闭WIFI
    if(doc["WIFI_ENABLE"].as<bool>())
    {
        pWifiNetwork->startWIFI();
        settings.enable_ST = true;
    }
    else
    {
        pWifiNetwork->disconnectWIFI();
        settings.enable_ST = false;
    }
    eventHandle.saveSettings();
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}   

void WebService::handleAPWIFIInfoRequest(AsyncWebServerRequest *request)
{
    //返回AP热点的名称和密码
    JsonDocument json;
    json["WIFI_SSID"] = settings.ST_ssid;
    json["WIFI_PASSWORD"] = settings.ST_password;
    json["WIFI_ENABLE"] = settings.enable_ST;
    json["AP_SSID"] = settings.AP_ssid;
    json["AP_PWD"] = settings.AP_password;
    json["AP_ENABLE"] = settings.enable_AP;
    String strJson;
    serializeJson(json, strJson);
    request->send(200, "application/json", strJson);
}   

void WebService::handleAPSaveRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    //获取POST过来的AP热点名称和密码并保存
    String jsonStr =(char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"result\":\"failed\",message:\"JSON parsing failed\"}");
        return;
    }
    settings.AP_ssid = doc["AP_SSID"].as<String>();
    settings.AP_password = doc["AP_PWD"].as<String>();
    eventHandle.saveSettings();
    request->send(200, "application/json", "{\"result\":\"OK\"}");  
}   

void WebService::handleAPEnableRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String jsonStr =(char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"result\":\"failed\",message:\"JSON parsing failed\"}");
        return;
    }
    //开启或关闭AP热点
    if(doc["AP_ENABLE"].as<bool>())
    {
        pWifiNetwork->startAP();
        settings.enable_AP = true;
    }
    else
    {
        pWifiNetwork->disconnectAP();
        settings.enable_AP = false;
    }
    eventHandle.saveSettings();
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}   

void WebService::handlePSUInfoRequest(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", pPSU->getPSUDataJson()); // 发送带有 JSON 内容
}

void WebService::handlePSUSettingRequest(AsyncWebServerRequest *request)
{
    JsonDocument json;
    json["POWER_SW"] = powerOn;
    json["FAN_SETTING"] = pPSU->fanSpeed;
    json["READ_FAN_SPEED_1"] = pPSU->fan[0];
    if (powerOn)
        pPSU->total_power_on = millis() - pPSU->turn_on_time; 
    else
        pPSU->total_power_on = 0;
    json["RUN_TIME"] = pPSU->total_power_on;
    json["AC_POWER_ON"] = settings.AC_PowerON;

    String strJson;
    serializeJson(json, strJson);
    request->send(200, "application/json", strJson);
}

void WebService::handlePSUPowerRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String jsonStr =(char*)data;
  
    // 打印原始POST数据
    Serial.println(jsonStr);
    // 解析JSON数据
    JsonDocument doc; // 根据你的JSON数据大小调整容量
    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"result\":\"failed\",message:\"JSON parsing failed\"}");
        return;
    }
    if(doc["POWER_ON"].as<bool>())
    {
        eventHandle.powerOn();
    }
    else
    {
        eventHandle.powerOff();
    }
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handlePSUFanRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String jsonStr =(char*)data;
  
    // 打印原始POST数据
    Serial.println(jsonStr);
    // 解析JSON数据
    JsonDocument doc; // 根据你的JSON数据大小调整容量
    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"result\":\"failed\",message:\"JSON parsing failed\"}");
        return;
    }

    eventHandle.setSpeed(doc["FAN_SPEED"].as<uint8_t>());
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleACPowerOnRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }

    settings.AC_PowerON = doc["AC_POWER_ON"].as<bool>();
    eventHandle.saveSettings();

    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleTasksRequest(AsyncWebServerRequest *request) {
    request->send(200, "application/json", taskManager.getAllTasksJson());
}

// 添加定时任务
void WebService::handleAddPeriodTaskRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    TimeTaskType type = static_cast<TimeTaskType>(doc["type"].as<int>());
    uint8_t startHour = doc["startHour"];
    uint8_t startMinute = doc["startMinute"];
    uint8_t endHour = doc["endHour"];
    uint8_t endMinute = doc["endMinute"];
    RepeatType repeatType = static_cast<RepeatType>(doc["repeatType"].as<int>());
    
    // 将数组转换为位图
    uint8_t weekDays = 0;
    JsonArray weekDaysArray = doc["weekDays"].as<JsonArray>();
    for (JsonVariant value : weekDaysArray) {
        int dayIndex = value.as<int>();
        switch(dayIndex) {
            case 0:
                weekDays |= SUNDAY;    // 0x01
                break;
            case 1:
                weekDays |= MONDAY;    // 0x02
                break;
            case 2:
                weekDays |= TUESDAY;   // 0x04
                break;
            case 3:
                weekDays |= WEDNESDAY; // 0x08
                break;
            case 4:
                weekDays |= THURSDAY;  // 0x10
                break;
            case 5:
                weekDays |= FRIDAY;    // 0x20
                break;
            case 6:
                weekDays |= SATURDAY;  // 0x40
                break;
        }
    }
    
    uint32_t taskId = taskManager.addPeriodTask(type, startHour, startMinute, 
                                              endHour, endMinute, repeatType, weekDays, true);
    eventHandle.saveSettings();
    eventHandle.printAlarms();

    JsonDocument response;
    response["result"] = "OK";
    response["taskId"] = taskId;
    
    String output;
    serializeJson(response, output);
    request->send(200, "application/json", output);
}

// 添加条件任务
void WebService::handleAddConditionTaskRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    ConditionType conditionType = static_cast<ConditionType>(doc["ConditionType"].as<int>());
    ConditionQuantity conditionQuantity = static_cast<ConditionQuantity>(doc["ConditionQuantity"].as<int>());
    CompareType compareType = static_cast<CompareType>(doc["CompareType"].as<int>());
    float conditionValue = doc["ConditionValue"];
    uint16_t conditionMinutes = doc["ConditionMinutes"];
    
    uint32_t taskId = taskManager.addConditionTask(conditionType, conditionQuantity, compareType, conditionValue, conditionMinutes, true);
    eventHandle.saveSettings();
    
    JsonDocument response;
    response["result"] = "OK";
    response["taskId"] = taskId;
    
    String output;
    serializeJson(response, output);
    request->send(200, "application/json", output);
}

// 修改处理函数
void WebService::handleDeletePeriodTaskRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    uint32_t taskId = doc["taskId"].as<uint32_t>();
    taskManager.removePeriodTask(taskId);
    eventHandle.saveSettings();
    eventHandle.printAlarms();

    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleDeleteConditionTaskRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    uint32_t taskId = doc["taskId"].as<uint32_t>();
    taskManager.removeConditionTask(taskId);
    eventHandle.saveSettings();

    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

// 启用/禁用任务的处理函数
void WebService::handleTogglePeriodTaskRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    uint8_t taskId = doc["taskId"].as<uint8_t>();
    bool toggle = doc["toggle"].as<bool>();
    
    taskManager.togglePeriodTask(taskId, toggle);
    eventHandle.saveSettings();
    eventHandle.printAlarms();

    JsonDocument response;
    response["result"] = "OK";
    String output;
    serializeJson(response, output);
    request->send(200, "application/json", output);
}

// 启用/禁用任务的处理函数
void WebService::handleToggleConditionTaskRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    uint8_t taskId = doc["taskId"].as<uint8_t>();
    bool toggle = doc["toggle"].as<bool>();
    
    taskManager.toggleConditionTask(taskId, toggle);
    eventHandle.saveSettings();
    
    JsonDocument response;
    response["result"] = "OK";
    String output;
    serializeJson(response, output);
    request->send(200, "application/json", output);
}

void WebService::handleGetShutdownTimerRequest(AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["active"] = taskManager._shutdownTimerActive;
    doc["remainingSeconds"] = taskManager._shutdownTime;
    
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
}

void WebService::handleSetShutdownTimerRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    String jsonStr = (char*)data;
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        request->send(400, "application/json", "{\"result\":\"failed\",\"message\":\"JSON parsing failed\"}");
        return;
    }
    
    uint32_t seconds = doc["seconds"].as<uint32_t>();
    taskManager.setShutdownTimer(seconds);
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleCancelShutdownTimerRequest(AsyncWebServerRequest *request) {
    taskManager.cancelShutdownTimer();
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleSyncTimeRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
   
    // 将时间同步请求交给事件处理系统
    eventHandle.syncTime();
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}
