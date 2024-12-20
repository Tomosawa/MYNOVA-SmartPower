<div id="top">
<p align="center">
  <img src="images/icon.png" width="48px" style="vertical-align:middle;display: inline-block;">

  <h1 align="center">MYNOVA-SmartPower</h1>
</p>
</div>
<div align="center">

[![stars](https://img.shields.io/github/stars/Tomosawa/MYNOVA-SmartPower.svg)](https://github.com/Tomosawa/MYNOVA-SmartPower)
[![forks](https://img.shields.io/github/forks/Tomosawa/MYNOVA-SmartPower.svg)](https://github.com/Tomosawa/MYNOVA-SmartPower)
[![open issues](https://img.shields.io/github/issues-raw/Tomosawa/MYNOVA-SmartPower.svg)](https://github.com/Tomosawa/MYNOVA-SmartPower/issues)
[![issue resolution](https://img.shields.io/github/issues-closed-raw/Tomosawa/MYNOVA-SmartPower.svg)](https://github.com/Tomosawa/MYNOVA-SmartPower/issues)
[![GitHub contributors](https://img.shields.io/github/contributors/Tomosawa/MYNOVA-SmartPower)](https://github.com/Tomosawa/MYNOVA-SmartPower/graphs/contributors)
[![LICENSE](https://img.shields.io/github/license/Tomosawa/MYNOVA-SmartPower)](https://github.com/Tomosawa/MYNOVA-SmartPower/blob/master/LICENSE)

[English](README.md) | 简体中文 | [日本語](README_ja-JP.md)

</div>

# 简介

这是一个开源硬件项目。旨在打造一个适用于DELL服务器PSU电源的智能取电盒。它能够控制电源的开关，读取电源的输入和输出的电压、电流、功率值以及温度在OLED屏幕上显示。它可以通过DC5.5的接口和XT60的接口来输出12V的电源供普通的12V的设备来使用。本开源项目中包含芯片的固件程序代码、Web端的程序代码、电路原理图（AltiumDesigner和PADS）、以及PCB的Gerber文件。还开源了外壳的3D文件（STL），可以进行3D打印。可编辑的STEP文件也公开了，大家可以任意修改模型。

制作好的成品见下图：

<center>

<p float="left">
  <img src="images/7.jpg" alt="Image 1" width="45%" style="margin-right:10px;"/>
  <img src="images/8.jpg" alt="Image 2" width="45%" />
</p>
<p float="left">
  <img src="images/9.jpg" alt="Image 1" width="45%" style="margin-right:10px;"/>
  <img src="images/10.jpg" alt="Image 2" width="45%" />
</p>
<p float="left">
  <img src="images/1.jpg" alt="Image 1" width="45%" style="margin-right:10px;"/>
  <img src="images/2.jpg" alt="Image 2" width="45%" />
</p>
<p float="left">
  <img src="images/3.jpg" alt="Image 1" width="45%" style="margin-right:10px;"/>
  <img src="images/4.jpg" alt="Image 2" width="45%" />
</p>
<p float="left">
  <img src="images/5.jpg" alt="Image 1" width="45%" style="margin-right:10px;"/>
  <img src="images/6.jpg" alt="Image 2" width="45%" />
</p>

</center>

## 功能介绍

- 控制电源的开和关
- 读取和显示电源的输入、输出的电压、电流、功率值
- 读取电源温度
- 设定风扇转速（暂时无效，会被电源本身改写覆盖）
- 可连接WIFI，通过手机或电脑输入IP后在Web页面查看
- 通过Web页面控制电源的开和关

## V1.1新增功能（20241126）
- 兼容E495E-S1和E750E-S1电源
- 设备端增加设置页面：通过在网络页面，长按右键进入。
- 支持WIFI的开关、热点的开关、屏幕亮度设置
- 支持设置上电自动开机
- 倒计时自动关机功能
- 定时开关机功能，支持时间段开关机、单独开机、关机以及重复周期策略设置
- 条件关机功能，输入或输出的电流、功率大于或者小于一定值后关机

## 适用芯片

- ESP32-S3-WROOM-N16
- ESP32-S3-WROOM-N8

或者S3相关系列的芯片。根据芯片Flash容量的大小，请自行定义好分区表后烧录。

## 测试电源

本程序在DELL的以下型号电源测试通过：
- F495E-S0
- E750E-S0
- F750E-S0
- D750E-S2
- E1100E-S0
- L1100E-S0
- D2000E-S2(EPP)
- E495E-S1(EPP)
- E750E-S1(EPP)

其它型号也类似，由于我手头没有更多的型号了，所以请大家自行测试。

## 目录说明

- MYNOVA_POWER： 芯片中的固件程序代码
- ESPVue： Web相关的程序代码
- paritions：分区表文件
- Hardware：硬件相关的开源资料，包括电路图，AlitumDesigner和PADS的设计文件，以及PCB的Geber文件
- 3DModels：外壳的3D模型文件，包括STL和可编辑的STEP文件

## 代码编译环境

- 固件程序：Arduino IDE 1.8.19
- Web程序：Node.js v20.15.1

## 固件编译依赖

- 安装ESP32官方的开发板（推荐V2.0.11否则会编译失败）
- 安装U8g2lib
- 安装ESPAsyncWebServer服务器组件
- 安装ArduinoJson库
- 安装ESP32TimerInterrupt库

## 固件烧录方法
1. 根据您的ESP32-S3芯片的闪存大小（16MB或8MB）的不同，在[Release](https://github.com/Tomosawa/MYNOVA-SmartPower/releases)页面下载对应的bin文件。
2. 下载官方的[flash download tool](https://www.espressif.com.cn/zh-hans/support/download/other-tools)或者使用[esptool.py](https://github.com/espressif/esptool)命令行工具。
3. 选择对应的bin固件文件，设定烧录偏移地址为0x0000。
4. 请不要勾选`DoNotChgBin`选项，然后点击开始烧录即可。

![flashdownloadtools](images/flashtools.png)

## 贡献指南

欢迎对本项目进行贡献，请遵循以下指南：
- Fork本仓库并创建你的特性分支。
- 提交你的更改并推送到你的Fork。
- 提交Pull Request。

## 版权声明

本项目禁止用于任何商业用途、仅供学习和自己DIY使用。

## 许可证 [LICENSE](LICENSE)

本项目采用[GPLv3](LICENSE)许可证。[GPLv3（GNU General Public License version 3）](LICENSE)是一个自由、开源的软件许可证，它保证了用户运行、学习、分享和修改软件的自由。
完整的[GPLv3](LICENSE)许可证文本包含在本项目的[LICENSE](LICENSE)文件中。在使用、修改或分发本项目的代码之前，请确保你已阅读并理解了[GPLv3](LICENSE)许可证的全部内容。

## 鸣谢

- [Multibutton](https://github.com/0x1abin/MultiButton)

