/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */
#include "GUIRender.h"
#include <U8g2lib.h>

// OLED屏幕配置
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, /* clock=*/2, /* data=*/3);

// 风扇图标动画
int fan_width = 16;
static unsigned char fan[][32] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0xc0, 0x07,
     0xe0, 0x03, 0xc8, 0x09, 0x98, 0x1c, 0xbc, 0x3f, 0xfc, 0x3d, 0x38, 0x19,
     0x90, 0x13, 0xc0, 0x07, 0xe0, 0x03, 0x80, 0x01},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x38, 0x1c,
     0x78, 0x3e, 0x78, 0x1e, 0x70, 0x0f, 0xc0, 0x01, 0x80, 0x03, 0xf0, 0x0e,
     0x78, 0x1e, 0x7c, 0x1e, 0x38, 0x1c, 0x00, 0x08}};
int icon_f = 0;

// 温度计图标
int temperature_width = 16;
static unsigned char temperature[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x01, 0xa0, 0x06,
    0xa0, 0x02, 0xa0, 0x06, 0xa0, 0x02, 0xa0, 0x02, 0x90, 0x04, 0xc8, 0x09,
    0x48, 0x09, 0xd0, 0x05, 0x20, 0x02, 0xc0, 0x01};

GUIRender::GUIRender()
{
    
}

void GUIRender::init()
{
    u8g2.begin();
}

void GUIRender::setValues(PMBus *pmbus, WIFINetwork *wifiNetwork)
{
    V_in = pmbus->V_in;
    I_in = pmbus->I_in;
    W_in = pmbus->W_in;
    V_out = pmbus->V_out;
    I_out = pmbus->I_out;
    W_out = pmbus->W_out;
    Temp1 = pmbus->T[0];
    Temp2 = pmbus->T[1];
    Temp3 = pmbus->T[2];
    Fan1 = pmbus->fan[0];

    this->wifiNetwork = wifiNetwork;
}

// 绘制界面
void GUIRender::drawOLED(int displayPage)
{
    u8g2.clearBuffer();
    if (displayPage == -1)
    { //开机确认页面
        u8g2.drawRFrame(0,0,128,64,3);
        u8g2.drawRFrame(15,40,40,18,2);
        u8g2.drawRFrame(74,40,40,18,2);

        u8g2.setFont(u8g2_font_helvB14_tf);
        u8g2.drawStr(8, 25, "POWER ON?");
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(20, 55, "YES");
        u8g2.drawStr(84, 55, "NO");
    }
    else if (displayPage == -2)
    { //关机确认页面
        u8g2.drawRFrame(0,0,128,64,3);
        u8g2.drawRFrame(15,40,40,18,2);
        u8g2.drawRFrame(74,40,40,18,2);
        
        u8g2.setFont(u8g2_font_helvB14_tf);
        u8g2.drawStr(2, 25, "POWER OFF?");
        u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(20, 55, "YES");
        u8g2.drawStr(84, 55, "NO");
    }
    else if (displayPage == 0)
    { // 关机显示页
        u8g2.setFont(u8g2_font_helvB14_tf);
        u8g2.drawStr(50, 39, "OFF");
        u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
        u8g2.drawGlyph(34, 40, 235);
    }
    else if (displayPage == 1)
    { // 输入和输出显示信息
        u8g2.setFont(u8g2_font_helvB10_tf);
        int padding = 1;
        int fontHeight = 13; 
        int y = fontHeight + padding;
        u8g2.drawButtonUTF8(31, y, U8G2_BTN_BW0 | U8G2_BTN_HCENTER | U8G2_BTN_INV, 62, padding, padding, "IN");
        u8g2.drawButtonUTF8(64 + 33, y, U8G2_BTN_BW0 | U8G2_BTN_HCENTER | U8G2_BTN_INV, 62, padding, padding, "OUT");

        u8g2.setFont(u8g2_font_freedoomr10_mu);

        char text[12];
        sprintf(text, "%7.3fV", V_in);
        u8g2.drawStr(0, 16 + 16, text);
        sprintf(text, "%7.3fA", I_in);
        u8g2.drawStr(0, 32 + 16, text);
        if (W_in < 1000)
            sprintf(text, "%7.3fW", W_in);
        else
            sprintf(text, "%7.2fW", W_in);
        u8g2.drawStr(0, 48 + 16, text);

        sprintf(text, "%7.3fV", V_out);
        u8g2.drawStr(64, 16 + 16, text);
        sprintf(text, "%7.3fA", I_out);
        u8g2.drawStr(64, 32 + 16, text);
        if (W_out < 1000)
            sprintf(text, "%7.3fW", W_out);
        else
            sprintf(text, "%7.2fW", W_out);
        u8g2.drawStr(64, 48 + 16, text);
    }
    else if (displayPage == 2)
    { // 温度页
        u8g2.drawXBM(0, 0, temperature_width, temperature_width, temperature);
        u8g2.drawXBM(0, 16, temperature_width, temperature_width, temperature);
        u8g2.drawXBM(0, 32, temperature_width, temperature_width, temperature);

        u8g2.drawXBM(0, 48, fan_width, fan_width, fan[icon_f ^= 1]);

        u8g2.setFont(u8g2_font_unifont_t_symbols);
        char text[10];

        sprintf(text, "%5.1f℃", Temp1);
        u8g2.drawUTF8(16, 16, text);

        sprintf(text, "%5.1f℃", Temp2);
        u8g2.drawUTF8(16, 32, text);

        sprintf(text, "%5.1f℃", Temp3);
        u8g2.drawUTF8(16, 48, text);

        sprintf(text, "%5.0f", Fan1);
        u8g2.drawUTF8(16, 64, text);
        u8g2.setFont(u8g2_font_4x6_tf);
        u8g2.drawStr(57, 64, "RPM");

        // 绘制进度条————边框
        u8g2.drawFrame(70, 10, 56, 6);
        u8g2.drawFrame(70, 26, 56, 6);
        u8g2.drawFrame(70, 42, 56, 6);
        u8g2.drawFrame(70, 58, 56, 6);
        // 绘制进度条————文字
        u8g2.setFont(u8g2_font_5x7_tf);
        u8g2.drawStr(70, 9, "0       100");
        u8g2.drawStr(70, 25, "0       100");
        u8g2.drawStr(70, 41, "0       100");
        u8g2.drawStr(70, 57, "0     18000");
        // 绘制进度条————内容
        int val1 = Temp1 / 100 * 54;
        u8g2.drawBox(71, 11, val1, 4);
        int val2 = Temp2 / 100 * 54;
        u8g2.drawBox(71, 27, val2, 4);
        int val3 = Temp3 / 100 * 54;
        u8g2.drawBox(71, 43, val3, 4);
        int val4 = Fan1 / 18000 * 54;
        u8g2.drawBox(71, 59, val4, 4);
    }
    else if (displayPage == 3)
    { // 网络信息页
        // 左侧AP、ST标识
        u8g2.setFontMode(1);  /* activate transparent font mode */
        u8g2.setDrawColor(1); /* color 1 for the box */
        u8g2.drawRBox(0, 0, 19, 31, 3);
        u8g2.drawRBox(0, 32, 19, 31, 3);
        u8g2.setFont(u8g2_font_roentgen_nbp_tf);
        u8g2.setDrawColor(2);
        u8g2.drawStr(1, 20, "AP");
        u8g2.drawStr(1, 52, "ST");

        // 中间分行
        u8g2.drawFrame(20, 0, 19, 10);
        u8g2.drawFrame(20, 11, 19, 9);
        u8g2.drawFrame(20, 21, 19, 10);
        u8g2.drawFrame(20, 32, 19, 10);
        u8g2.drawFrame(20, 43, 19, 9);
        u8g2.drawFrame(20, 53, 19, 10);
        u8g2.setFont(u8g2_font_4x6_tf);
        u8g2.drawStr(22, 8, "SSID");
        u8g2.drawStr(22, 18, "STAT");
        u8g2.drawStr(22, 29, " IP ");
        u8g2.drawStr(22, 40, "SSID");
        u8g2.drawStr(22, 50, "STAT");
        u8g2.drawStr(22, 61, " IP ");

        // 右侧数据
        u8g2.setFont(u8g2_font_helvR08_tr);
        u8g2.drawStr(40, 9, "MYNOVA");
        u8g2.drawStr(40, 19, wifiNetwork->AP_State ? "Started" : "Stopped");
        u8g2.drawStr(40, 30, wifiNetwork->getAPIP().toString().c_str());
        u8g2.drawStr(40, 41, wifiNetwork->wifi_ssid.c_str());
        u8g2.drawStr(40, 51, wifiNetwork->getWIFIStateInfo().c_str());
        u8g2.drawStr(40, 62, wifiNetwork->getWIFIIP().toString().c_str());
    }
    else if(displayPage == 4)
    {   //风扇速度设定页面
        u8g2.setFont(u8g2_font_helvB12_tf);
        u8g2.drawStr(20, 25, "FAN SPEED");

        //进度条边框
        u8g2.drawFrame(5, 49, 96, 7);
        u8g2.setFont(u8g2_font_6x10_tf);
        char text[5];
        sprintf(text, "%d%%", FanSpeed);
        u8g2.drawStr(102, 56, text);
        u8g2.drawBox(6, 50, FanSpeed * 95.0 / 100, 6);
        
    }
    u8g2.sendBuffer();
}
