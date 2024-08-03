#include "WebService.h"
#include <ArduinoJson.h>
#include "EventHandle.h"

extern EventHandle eventHandle;

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
    server.on("/api/psuinfo", HTTP_GET, std::bind(&WebService::handlePSUInfoRequest, this, std::placeholders::_1));
    server.on("/api/psusetting", HTTP_GET, std::bind(&WebService::handlePSUSettingRequest, this, std::placeholders::_1));
    server.on("/api/psupower", HTTP_PUT, handleRequest, handleUploadRequest,std::bind(&WebService::handlePSUPowerRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
    server.on("/api/psufan", HTTP_PUT, handleRequest, handleUploadRequest,std::bind(&WebService::handlePSUFanRequest, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5));
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
    pWifiNetwork->wifi_ssid = ssid;
    pWifiNetwork->wifi_password = pwd;
    eventHandle.saveWifi();
    // 返回响应
    request->send(200, "application/json", "{\"result\":\"OK\"}");

}

void WebService::handleWIFIClearRequest(AsyncWebServerRequest *request)
{
    eventHandle.disConnectWifi();
    pWifiNetwork->wifi_ssid = "";
    pWifiNetwork->wifi_password = "";
    eventHandle.saveWifi();
    // 返回响应
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handleWIFIConnectRequest(AsyncWebServerRequest *request)
{
    eventHandle.connectWifi();
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}

void WebService::handlePSUInfoRequest(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", pPSU->getPSUDataJson()); // 发送带有 JSON 内容
}

void WebService::handlePSUSettingRequest(AsyncWebServerRequest *request)
{
    request->send(200, "application/json", pPSU->getPSUSettingJson());
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
    if(doc["POWER_ON"])
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

    eventHandle.setSpeed(doc["FAN_SPEED"]);
    request->send(200, "application/json", "{\"result\":\"OK\"}");
}
