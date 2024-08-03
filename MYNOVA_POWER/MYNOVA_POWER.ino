/*
 Name:		MYNOVA_POWER.ino
 Created:	2024/1/22 13:40:30
 Author:	Tomosawa
 GitHub:    https://github.com/Tomosawa/MYNOVA-SmartPower
*/
#pragma GCC diagnostic warning "-Wunused-variable"

#include <Arduino.h>
#include <Wire.h>
#include "multi_button.h"
#include "pmbus.h"
#include "GUIRender.h"
#include "WIFINetwork.h"
#include "WebService.h"
#include "ButtonHandle.h"
#include "IOPIn.h"

//类声明
PMBus psu;
GUIRender guiRender;
WIFINetwork wifiNetwork;
WebService webService;
ButtonHandle buttonHandle;

//全局变量
//开机确认页面[-1]关机确认页面[-2]风扇速度设置[4]
//关机OFF页面[0]输入输出页面[1]温度风扇[2]网络信息[3]
int displayPage = 0;
bool powerOn = false;
bool switchOn = false;//硬件开关是否按下
int pageTimeOut = 0;//页面超时
//是否扫描WIFI中
bool bScanWifi = true;

//程序初始化
void setup()
{
    Serial.begin(115200);
    
    //版本信息
    String version_info = "MYNOVA Ver1.0 \r\nBuild " + String(__DATE__);
    Serial.println(version_info);

    //I2C PMBus initial
    Serial.println("I2C PMBus initial Wire.begin");
    Wire1.begin(PIN_PMBUS_SDA, PIN_PMBUS_SCL, I2C_PMBUS_CLOCK);

    pinMode(PIN_SW_SIGNAL, INPUT_PULLUP);

    //按钮初始化
    buttonHandle.init();

    // Init PSU
    Serial.println("Init PSU and read info");
    psu.init(PIN_SW_PSON, PIN_SW_PSKILL, &Serial, &Wire1);

    // Display Init
    Serial.println("OLED GUI Init");
    guiRender.init();

    //WIFI初始化
    Serial.println("WIFI Network Init");
    wifiNetwork.init();
   
    //异步扫描WIFI
    WiFi.scanNetworks(true);

    Serial.println("\r\n===setup() complete===\r\n\n");
}
/*
 * 初始化WIFI、AP、Web服务，在扫描WIFI完成后调用
 */
void InitWIFIAPService()
{

    if(wifiNetwork.ReadConfig())
    {
        Serial.println("Read WIFI Config: OK\n Start Connect WIFI");
        wifiNetwork.startWIFI();
    }
    else
    {
        Serial.println("Read WIFI Config: Failed");
    }
    
    wifiNetwork.wifi_json =  wifiNetwork.getAllSSIDJson();

    //启动热点
    Serial.println("WIFI AP Start");
    wifiNetwork.startAP();

    //初始化 启动Web服务器
    Serial.println("WebService Start");
    webService.init(wifiNetwork.getAPIP(), &wifiNetwork, &psu);
    webService.serverStart();
}
/*
 * 程序循环
 */
void loop()
{
    if(bScanWifi)
    {
        Serial.println("WIFI Scan...");
        int networksCount = WiFi.scanComplete();
        if(networksCount >= 0)
        {
            Serial.print("WIFI Scan OK. Count:");
            Serial.println(networksCount);
            bScanWifi = false;
            InitWIFIAPService();
            
        }else if(networksCount == WIFI_SCAN_FAILED)
        {
            Serial.println("WIFI Scan Failed");
            bScanWifi = false;
            InitWIFIAPService();
        }
    }
    //硬件开关按下情况
    if(digitalRead(PIN_SW_SIGNAL) == 0)
        switchOn = true;
    else
        switchOn = false;
    
    if (psu.scan())
    {
        Serial.println("PSU Scan OK");
        //判断电源是否启动
        bool bState = psu.checkOnOffState();

        //与之前状态有变化，进行处理
        if(powerOn != bState)
        {
            //记录状态
            powerOn = bState;
            if(powerOn){
                psu.recordTurnOnTime();
                //输入输出页面[1]
                displayPage = 1;
            }
            else
            {
                //关机OFF页面[0]
                displayPage = 0;
            }
        }

        guiRender.setValues(&psu,&wifiNetwork);
        
    }

    //风扇设置页面[4]:超时计时
    if(displayPage == 4)
    {
        ++pageTimeOut;
        if(pageTimeOut >= 50)
        {
            pageTimeOut = 0;
            //返回->温度风扇页面[2]
            displayPage = 2;
        }
    }

    yield();
    guiRender.drawOLED(displayPage);

    yield();

    webService.processRequest();

    yield();

    delay(100);
}
