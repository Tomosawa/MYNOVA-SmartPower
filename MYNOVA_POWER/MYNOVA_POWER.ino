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
#include "Settings.h"
#include "Version.h"
#include "PageTimer.h"
#include "TimeSync.h"
#include "TaskManager.h"
//类声明
PMBus psu;
GUIRender guiRender;
WIFINetwork wifiNetwork;
WebService webService;
ButtonHandle buttonHandle;
Settings settings;
PageTimer pageTimer;
TimeSync timeSync;
TaskManager taskManager;
//全局变量
//开机确认页面[-1]关机确认页面[-2]风扇速度设置[4]
//关机OFF页面[0]输入输出页面[1]温度风扇[2]网络信息[3]
int displayPage = 0;
bool powerOn = false;
u_long pageTimeOut = 5000;//页面超时5s
//是否扫描WIFI中
bool bScanWifi = true;

//程序初始化
void setup()
{
    Serial.begin(115200);
    
    //版本信息
    String version_info = "MYNOVA Firmware " + String(FIRMWARE_VERSION) + " \r\nBuild " + String(__DATE__);
    Serial.println(version_info);

    //I2C PMBus initial
    Serial.println("I2C PMBus initial Wire.begin");
    Wire1.begin(PIN_PMBUS_SDA, PIN_PMBUS_SCL, I2C_PMBUS_CLOCK);

    //按钮初始化
    buttonHandle.init();
    
    // Read Settings
    settings.ReadSettings();

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

    //开机是否启动电源
    if(settings.AC_PowerON)
    {
        psu.on();
    }

    // 初始化任务管理器    
    taskManager.setGetValueCallback([](ConditionType type, ConditionQuantity quantity) -> float {
        switch(type) {
            case ConditionType::TYPE_INPUT:
                return quantity == ConditionQuantity::QUANTITY_CURRENT ? psu.I_in : psu.W_in;
            case ConditionType::TYPE_OUTPUT:
                return quantity == ConditionQuantity::QUANTITY_CURRENT ? psu.I_out : psu.W_out;
            default:
                return 0.0f;
        }
    });
    
    // 恢复保存的任务
    //settings.RestoreTasks(taskManager);

    // 根据WiFi设置决定任务还原时机
    if (!settings.enable_ST || settings.ST_ssid.isEmpty()) {
        // 如果未启用WiFi或没有配置WiFi，直接还原任务
        Serial.println("WiFi disabled, restore tasks immediately");
        settings.RestoreTasks(taskManager);
    } else {
        // 如果启用了WiFi，等待时间同步后再还原任务
        Serial.println("WiFi enabled, will restore tasks after time sync");
        // 在时间同步完成后的回调中还原任务
        timeSync.setTimeSyncCallback([]() {
            Serial.println("Time synced, restoring tasks");
            settings.RestoreTasks(taskManager);
        });
    }
    
    // 启动任务管理器
    taskManager.start();

    Serial.println("\r\n===setup() complete===\r\n");
    Serial.print("WIFI Scan.");
}
/*
 * 初始化WIFI、AP、Web服务，在扫描WIFI完成后调用
 */
void InitWIFIAPService()
{
    bool bStartWebService = false;
    //启动WIFI
    if(settings.enable_ST && !settings.ST_ssid.isEmpty())
    {
        Serial.println("WIFI ST Enabled\nStart Connect WIFI\nSSID:"+settings.ST_ssid+"\nPWD:"+settings.ST_password);
        wifiNetwork.startWIFI();
        bStartWebService = true;
    }
    else
    {
        Serial.println("WIFI ST Disabled");
    }
    
    wifiNetwork.wifi_json = wifiNetwork.getAllSSIDJson();

    //启动热点
    if(settings.enable_AP && !settings.AP_ssid.isEmpty() && !settings.AP_password.isEmpty())
    {
        Serial.println("WIFI AP Started");
        wifiNetwork.startAP();
        bStartWebService = true;
    }
    else
    {
        Serial.println("WIFI AP Disabled");
    }

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
        Serial.print(".");
        int networksCount = WiFi.scanComplete();
        if(networksCount >= 0)
        {
            Serial.print("\nWIFI Scan OK. Count:");
            Serial.println(networksCount);
            bScanWifi = false;
            InitWIFIAPService();
            
        }else if(networksCount == WIFI_SCAN_FAILED)
        {
            Serial.println("\nWIFI Scan Failed");
            bScanWifi = false;
            InitWIFIAPService();
        }
        delay(500);
        yield();
    }
    //时间同步
    if (WiFi.status() == WL_CONNECTED && !timeSync.timeSynced) {
        timeSync.checkIfNeedSync();
    }
   
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
      if(pageTimer.checkTimeOut(pageTimeOut))
      {
        //返回->温度风扇页面[2]
        displayPage = 2;
      }
    }

    yield();
    guiRender.drawOLED(displayPage);

    yield();
    webService.processRequest();

    yield();

    //delay(100);
}
