/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "WIFINetwork.h"
#include <Preferences.h>
#include <ArduinoJson.h>

WIFINetwork::WIFINetwork() : AP_local_ip(10, 0, 10, 1),  // IP地址
                             AP_gateway(10, 0, 10, 1),   // 网关地址
                             AP_subnet(255, 255, 255, 0) // 子网掩码
{
    AP_ssid = "MYNOVA";
    AP_password = "MyNova123";
}

void WIFINetwork::init()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(AP_local_ip, AP_gateway, AP_subnet);
}

void WIFINetwork::startAP()
{
    AP_State = WiFi.softAP(AP_ssid, AP_password);
}

void WIFINetwork::startWIFI()
{
    WiFi.begin(wifi_ssid, wifi_password); // 连接WIFI
}

IPAddress WIFINetwork::getWIFIIP()
{
    return WiFi.localIP();
}

IPAddress WIFINetwork::getAPIP()
{
    return WiFi.softAPIP();
}

void WIFINetwork::reconnectWIFI()
{
    WiFi.reconnect();
}
void WIFINetwork::disconnectWIFI()
{
    WiFi.disconnect();
}
void WIFINetwork::disconnectAP()
{
    AP_State = !WiFi.softAPdisconnect();
}

wl_status_t WIFINetwork::getWIFIState()
{
    return WiFi.status();
}

String WIFINetwork::getWIFIStateInfo()
{
    String state;
    switch (getWIFIState())
    {
    case WL_NO_SHIELD: // for compatibility with WiFi Shield library
        state = "NO_SHIELD";
        break;
    case WL_IDLE_STATUS:
        state = "IDLE_STATUS";
        break;
    case WL_NO_SSID_AVAIL:
        state = "NO_SSID_AVAIL";
        break;
    case WL_SCAN_COMPLETED:
        state = "SCAN_COMPLETED";
        break;
    case WL_CONNECTED:
        state = "CONNECTED";
        break;
    case WL_CONNECT_FAILED:
        state = "CONNECT_FAILED";
        break;
    case WL_CONNECTION_LOST:
        state = "CONNECTION_LOST";
        break;
    case WL_DISCONNECTED:
        state = "DISCONNECTED";
        break;
    default:
        state = "";
        break;
    }
    if (wifi_ssid.isEmpty())
        state = "NOT_CONFIG";
    return state;
}

void WIFINetwork::SaveConfig()
{
    Preferences prefs;
    prefs.begin("MYNOVA");

    prefs.putString("WIFI_SSID", wifi_ssid);
    prefs.putString("WIFI_PASSWORD", wifi_password);

    prefs.end();
}

bool WIFINetwork::ReadConfig()
{
    Preferences prefs;
    prefs.begin("MYNOVA");

    wifi_ssid = prefs.getString("WIFI_SSID", "");
    wifi_password = prefs.getString("WIFI_PASSWORD", "");

    prefs.end();

    if (wifi_ssid.isEmpty())
        return false;

    return true;
}

String WIFINetwork::getAllSSIDJson()
{
    // 开始扫描周围的Wi-Fi网络
    // WiFi.scanNetworks(true);
    // delay(2000);
    int networksCount = WiFi.scanComplete();

    JsonDocument doc;
    doc["WIFI_SSID"] = wifi_ssid;
    doc["WIFI_PASSWORD"] = wifi_password;
    // 创建JsonArray
    JsonArray networks = doc["WIFI_SCAN"].to<JsonArray>();

    // 遍历扫描结果，并将SSID添加到JSON数组中
    for (int i = 0; i < networksCount; ++i)
    {
        // 添加第一个网络信息
        JsonObject network = networks.add<JsonObject>();
        network["SSID"] = WiFi.SSID(i);
    }
    // 将JSON数据转换为字符串
    String jsonString;
    serializeJson(doc, jsonString);
    // 返回JSON格式的字符串
    return jsonString;
}