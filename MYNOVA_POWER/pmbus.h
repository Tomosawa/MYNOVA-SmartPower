/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */

#ifndef PMBUS_H
#define PMBUS_H

#include <Arduino.h>
#include <Wire.h>
#include <time.h>

#define STATUS_REGISTERS 10
// I2C通讯频率
#define I2C_PMBUS_CLOCK 100000

// 与电源PMBus通讯类
class PMBus
{

public:
  PMBus();
  int init(int pson_pin, int pskill_pin, Stream *Debug_Serial, TwoWire *i2c);
  int scan();
  void clear_faults();
  void off();
  void on();
  bool checkOnOffState();
  bool checkDeviceOnline();
  void getRunTime();
  void recordTurnOnTime();
  float readFanCommand();
  void writeFanCommand(int value);
  String getPSUDataJson();
  String getPSUSettingJson();

  String mfr_id, mfr_model, mfr_revision, mfr_location, mfr_date, mfr_serial;
  float V_in = 0.0, I_in = 0.0, V_out = 0.0, I_out = 0.0, T[3] = {0.0, 0.0, 0.0}, fan[2] = {0.0, 0.0},
        W_in = 0.0, W_out = 0.0, fanSpeed = 0.0, E_in = 0.0, E_out = 0.0;
  uint8_t pmbus_revision = 0, vout_mode = 0;
  uint8_t status_u8 = 0, status_vout = 0, status_iout = 0, status_input = 0, status_temperature = 0,
          status_cml = 0, status_other = 0, status_mfr_specific = 0, status_fans = 0, *status_byte[STATUS_REGISTERS];
  uint16_t status_word = 0, vout_command = 0;
  long turn_on_time = 0, total_power_on = 0;

  typedef struct linear
  {
    int y : 11;
    int n : 5;
  } linear_t;

  typedef union
  {
    linear_t linear;
    uint16_t u16;
  } UniLinear;

  typedef struct Coefficient
  {
    uint16_t m;
    uint16_t b;
    uint16_t R;
  } Coefficient_t;
private:
  uint8_t read_byte(uint8_t reg);
  void write_byte(uint8_t reg, uint8_t value);
  uint16_t read_word(uint8_t reg);
  void write_word(uint8_t reg, uint16_t value);
  String read_string(uint8_t reg);
  void read_block(uint8_t reg, int bytes, uint8_t *buffer);
  float read_linear(uint8_t reg);
  void write_linear(uint8_t reg, float value);
  float read_linear16(uint8_t reg);
  float read_direct(uint8_t reg);
  void readCoefficients(uint8_t reg, Coefficient_t *coeff);
  float linear2float(uint16_t u16);
  float linear11ToFloat(uint16_t data);
  float ulinear16ToFloat(uint16_t data);
  int8_t readVoutMode(uint8_t reg);
  uint16_t float2linear(float value);
  uint16_t convertlinear11(uint16_t value);
  float readVout(uint8_t reg);
  bool GetVOutFormat();
  void readMFR(); // 读取电源厂商信息
  uint16_t convertHex2Dec(uint16_t hexData);
private:
  bool bReadMFR = false; // 是否读取了厂商信息了
  short int PSON_PIN = 6;
  short int PSKILL_PIN = 7;
  int8_t exponent = 0;// uLinear16计算用
  bool isVOutLinear = true;// 输出电压的格式
  Coefficient_t coeff;
  Stream *Debug = NULL;
  TwoWire *I2C = NULL;
};

#endif
