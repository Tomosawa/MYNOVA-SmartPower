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

English | [简体中文](README_zh-CN.md) | [日本語](README_ja-JP.md)

</div>

# Introduction

This is the firmware program for the intelligent power extraction device that manages the DELL server PSU(Power Supply Unit), reads relevant PMBus parameters, and providing 12V power output through DC5.5 and XT60 interfaces. The program includes firmware code for the chip and web-based application code.

Completed product can be seen in the following image:

<center>
<figure>
<img src="images/1.jpg" width="45%" />
<img src="images/2.jpg" width="45%" />
<img src="images/3.jpg" width="45%" />
<img src="images/4.jpg" width="45%" />
<img src="images/5.jpg" width="45%" />
<img src="images/6.jpg" width="45%" />
</figure>
</center>

## Features

- Control power on and off.
- Read and display input, output voltage, current, and power values.
- Read power supply temperature.
- Set fan speed (currently ineffective, overridden by the PSU itself).
- WiFi connectivity to view data via web interface by entering IP on mobile or computer.
- Control power on and off via web interface.

## Supported Chips

- ESP32-S3-WROOM-N16
- ESP32-S3-WROOM-N8

Or chips from the ESP32-S3 series. Depending on flash capacity of the chip, define partition table before download firmware.

## Tested PSU

This firmware has been tested on the following DELL PSU models:
- E750E-S0
- E1100E-S0
- D2000E-S2(EPP)

Other models should be similar; further testing required as I lack additional models.

## Directory Structure

- MYNOVA_POWER: Firmware code for the chip.
- ESPVue: Web-related application code.
- partitions: Partition table files.

## Compilation Environment

- Firmware: Arduino IDE 1.8.19
- Web: Node.js v20.15.1

## Firmware Dependencies

- Install ESP32 official development board (recommended V2.0.11 to avoid compilation failures).
- Install U8g2lib.
- Install ESPAsyncWebServer server component.
- Install ArduinoJson library.
- Install ESP32TimerInterrupt library.

## Contribution Guidelines

Contributions to this project are welcome. Please follow these guidelines:
- Fork the repository and create your feature branch.
- Commit your changes and push to your fork.
- Submit a Pull Request.

## Copyright Statement

This project is prohibited for any commercial use and is intended for learning and DIY purposes only.

## License [LICENSE](LICENSE)

This project is licensed under the [GPLv3](LICENSE). [GPLv3 (GNU General Public License version 3)](LICENSE) is a free, open-source software license that guarantees users the freedoms to run, study, share, and modify the software.
The complete text of the [GPLv3](LICENSE) license is included in the [LICENSE](LICENSE) file of this project. Before using, modifying, or distributing the code of this project, make sure you have read and understood the entire [GPLv3](LICENSE) license.

## Acknowledgments

- [Multibutton](https://github.com/0x1abin/MultiButton)