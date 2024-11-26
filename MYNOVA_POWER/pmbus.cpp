/*
 * Copyright (c) 2024 Tomosawa
 * https://github.com/Tomosawa/
 * All rights reserved
 */

#pragma GCC diagnostic warning "-Wunused-variable"

#define DIAGNOSTICS 1

#include "pmbus.h"
#include <ArduinoJson.h>
// PMBus I2C Address define
#define PMBUS_ADDRESS 0x58
// PMBus 命令定义
// 基本信息
#define MFR_ID 0x99
#define MFR_MODEL 0x9A
#define MFR_REVISION 0x9B
#define MFR_LOCATION 0x9C
#define MFR_DATE 0x9D
#define MFR_SERIAL 0x9E
// 规格信息
#define MFR_VIN_MIN 0xA0
#define MFR_VIN_MAX 0xA1
#define MFR_IIN_MAX 0xA2
#define MFR_PIN_MAX 0xA3
#define MFR_VOUT_MIN 0xA4
#define MFR_VOUT_MAX 0xA5
#define MFR_IOUT_MAX 0xA6
#define MFR_POUT_MAX 0xA7
#define MFR_TAMBIENT_MAX 0xA8
#define MFR_TAMBIENT_MIN 0xA9
// 输入输出信息
#define READ_EIN 0x86
#define READ_EOUT 0x87
#define READ_VIN 0x88
#define READ_IIN 0x89
#define READ_VCAP 0x8A
#define READ_VOUT 0x8B
#define READ_IOUT 0x8C
#define READ_POUT 0x96
#define READ_PIN 0x97
// 温度相关
#define READ_TEMPERATURE_1 0x8D
#define READ_TEMPERATURE_2 0x8E
#define READ_TEMPERATURE_3 0x8F
#define READ_FAN_SPEED_1 0x90
// 参数设置
#define VOUT_COMMAND 0x21
#define VOUT_MAX 0x24
#define FAN_COMMAND_1 0x3B
#define VOUT_OV_FAULT_LIMIT 0x40
#define VOUT_OV_FAULT_RESPONSE 0x41
#define VOUT_OV_WARN_LIMIT 0x42
#define VOUT_UV_FAULT_LIMIT 0x44
#define VOUT_UV_FAULT_RESPONSE 0x45
#define IOUT_OC_FAULT_LIMIT 0x46
#define IOUT_OC_FAULT_RESPONSE 0x47
#define IOUT_OC_WARN_LIMIT 0x4A
#define OT_FAULT_LIMIT 0x4F
#define OT_FAULT_RESPONSE 0x50
#define OT_WARN_LIMIT 0x51
#define VIN_OV_FAULT_LIMIT 0x55
#define VIN_UV_W ARN_LIMIT 0x58
#define VIN_UV_FAULT_LIMIT 0x59

#define COEFFICIENTS 0x30
#define VOUT_MODE 0x20

/*
 */

PMBus::PMBus()
{
  I2C = &Wire;
}

/**
 * 初始化PMBus模块。
 *
 * @param pson_pin PSON引脚的编号。
 * @param pskill_pin PSKILL引脚的编号。
 * @param Debug_Serial 用于调试信息的串行流对象。
 * @param i2c I2C通信接口对象，如果为null则不进行I2C通信。
 * @return 返回0表示初始化成功。
 */
int PMBus::init(int pson_pin, int pskill_pin, Stream *Debug_Serial, TwoWire *i2c)
{
  PSON_PIN = pson_pin;
  PSKILL_PIN = pskill_pin;
  Debug = Debug_Serial;

  if (i2c)
  {
    I2C = i2c;
  }

  pinMode(PSON_PIN, OUTPUT);
  pinMode(PSKILL_PIN, OUTPUT);

  delay(500);

  if (Debug)
  {
    Debug->print("\r\nPMBus::init() complete\r\n");
  }

  return 0;
}

/*
 *  关闭电源
 */
void PMBus::off()
{
  digitalWrite(PSON_PIN, LOW);
  digitalWrite(PSKILL_PIN, LOW);
  if (Debug)
    Debug->println("PSU Power Off");
}

/*
 *  开启电源
 */
void PMBus::on()
{
  digitalWrite(PSKILL_PIN, HIGH);
  digitalWrite(PSON_PIN, HIGH);
  if (Debug)
    Debug->println("PSU Power On");
}

/*
 *  获取数据
 */
int PMBus::scan()
{
  // 判断时间间隔1秒才执行
  static u_long last_scan = 0;
  u_long msecs = millis();
  if ((msecs - last_scan) < 1000)
  {
    return 0;
  }
  last_scan = msecs;

  // 判断设备是否在线
  if (!checkDeviceOnline())
  {
    return 0;
  }

  // 读取厂商信息，仅一次
  if (!bReadMFR)
  {
    readMFR();
    // 同时读取只需要读一次的参数
    isVOutLinear = GetVOutFormat();
    Debug->print("VOUT isLinearFormat=");
    Debug->println(isVOutLinear);

    exponent = readVoutMode(VOUT_MODE);
    Debug->print("VOUT exponent=");
    Debug->println(exponent);

    // 读取 COEFFICIENTS
    readCoefficients(COEFFICIENTS, &coeff);

    uint8_t fanconfig = read_byte(0x3A);
    Debug->println(String(fanconfig,BIN));
  }

  delay(1);
  E_in = read_linear(READ_EIN);

  delay(1);
  E_out = read_linear(READ_EOUT);

  delay(1);
  V_in = read_linear(READ_VIN);

  delay(1);
  I_in = read_linear(READ_IIN);

  delay(1);
  W_in = read_linear(READ_PIN);

  delay(1);
  V_out = read_linear16(READ_VOUT) / 1000.0;

  delay(1);
  I_out = read_linear(READ_IOUT);

  delay(1);
  W_out = read_linear(READ_POUT);

  delay(1);
  T[0] = read_linear(READ_TEMPERATURE_1);

  delay(1);
  T[1] = read_linear(READ_TEMPERATURE_2);

  delay(1);
  T[2] = read_linear(READ_TEMPERATURE_3);

  delay(1);
  fan[0] = read_linear(READ_FAN_SPEED_1);

  delay(1);
  readFanCommand();

  return 1;
}

String PMBus::getPSUDataJson()
{
  JsonDocument json;
  json["MFR_ID"] = mfr_id;
  json["MFR_MODEL"] = mfr_model;
  json["MFR_REVISION"] = mfr_revision;
  json["MFR_LOCATION"] = mfr_location;
  json["MFR_DATE"] = mfr_date;
  json["MFR_SERIAL"] = mfr_serial;

  char text[12];
  sprintf(text, "%.3f", V_in);
  json["READ_VIN"] = text;
  sprintf(text, "%.3f", I_in);
  json["READ_IIN"] = text;
  sprintf(text, "%.1f", W_in);
  json["READ_PIN"] = text;
  sprintf(text, "%.3f", V_out);
  json["READ_VOUT"] = text;
  sprintf(text, "%.3f", I_out);
  json["READ_IOUT"] = text;
  sprintf(text, "%.1f", W_out);
  json["READ_POUT"] = text;
  json["READ_TEMPERATURE_1"] = T[0];
  json["READ_TEMPERATURE_2"] = T[1];
  json["READ_TEMPERATURE_3"] = T[2];
  json["READ_FAN_SPEED_1"] = fan[0];

  String strJson;
  serializeJson(json, strJson);
  return strJson;
}
/*
 *  清除状态
 */
void PMBus::clear_faults()
{

  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(0x03);
  I2C->endTransmission(true);
}

void PMBus::write_byte(uint8_t reg, uint8_t value)
{
  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg);
  I2C->write(value);
  I2C->endTransmission(true);
}

uint8_t PMBus::read_byte(uint8_t reg)
{
  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg);
  I2C->endTransmission(false);

  I2C->requestFrom(PMBUS_ADDRESS, 1, 1);

  return I2C->read();
}

uint16_t PMBus::read_word(uint8_t reg)
{
  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg);
  I2C->endTransmission(false);

  I2C->requestFrom(PMBUS_ADDRESS, 2);

  uint16_t dataRaw = 0;
  if (I2C->available() >= 2)
  {
    // 读取高字节和低字节
    uint8_t lowByte = I2C->read();
    uint8_t highByte = I2C->read();
    // 将高字节和低字节组合为16位的原始值
    dataRaw = (highByte << 8) | lowByte;
  }
  return dataRaw;
}

void PMBus::write_word(uint8_t reg, uint16_t value)
{
  // 将16位值分成两个字节
  uint8_t value_lo = value & 0xFF;        // 低位字节
  uint8_t value_hi = (value >> 8) & 0xFF; // 高位字节

  // 开始I2C传输
  I2C->beginTransmission(PMBUS_ADDRESS);

  // 发送寄存器地址
  I2C->write(reg);

  // 发送数据，先发送高位字节，再发送低位字节
  I2C->write(value_lo);
  I2C->write(value_hi);

  // 结束I2C传输，确保数据被发送
  I2C->endTransmission();
}

String PMBus::read_string(uint8_t reg)
{
  int len = 0;
  len = (int)read_byte(reg);
  String result;

  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg);
  I2C->endTransmission(false);

  I2C->requestFrom(PMBUS_ADDRESS, len, 1);

  for (int i = 0; i < len; i++)
  {
    int readData = I2C->read();
    if (isPrintable(readData))
      result += (char)readData;
  }

  return result;
}

void PMBus::read_block(uint8_t reg, int bytes, uint8_t *buffer)
{
  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg);
  I2C->endTransmission(false);

  I2C->requestFrom(PMBUS_ADDRESS, bytes, 1);

  for (int i = 0; i < bytes; ++i)
  {
    buffer[i] = I2C->read();
  }
}

float PMBus::read_linear(uint8_t reg)
{
  uint16_t linear;

  linear = read_word(reg);
  // Debug->print("Read register: 0x");
  // Debug->println(reg, HEX);

  return linear11ToFloat(linear);
}

uint16_t PMBus::convertHex2Dec(uint16_t hexData)
{
  // 将输入的十进制数视为十六进制数，直接返回其对应的十进制值
  uint16_t decValue = (hexData / 1000) * 4096 + ((hexData % 1000) / 100) * 256 + ((hexData % 100) / 10) * 16 + (hexData % 10);
  return decValue;
}

float PMBus::read_linear16(uint8_t reg)
{
  uint16_t linear;
  linear = read_word(reg);
  // 根据VOUT_MODE中的 4:0 的值是-9进行转换
  return ulinear16ToFloat(linear);
}

float PMBus::readVout(uint8_t reg)
{
  // 读取 VOUT 寄存器
  uint16_t rawVout;
  rawVout = read_word(reg);

  // Debug->print("VOut_raw:");
  // Debug->println(rawVout);
  // 判断格式
  if (isVOutLinear)
  {
    // 计算 LINEAR 格式电压
    float voltage = rawVout * pow(2, exponent); // LINEAR11
    return voltage;
  }
  else
  {
    // 计算 DIRECT 格式电压
    // 计算实际电压
    float actualVoltage = (rawVout * coeff.m / pow(2, coeff.R)) + coeff.b;
    return actualVoltage;
  }
}
/*
 * 将LINEAR11格式转换为浮点数
 */
float PMBus::linear11ToFloat(uint16_t data)
{

  // Debug->print("linear11ToFloat raw_data=");
  // Debug->println(data);
  int16_t exponent11 = (data >> 11) & 0x1F; // 获取5位指数
  // Debug->print("linear11ToFloat exponent11=");
  // Debug->println(exponent11);
  if (exponent11 > 15)
  {
    exponent11 = exponent11 - 32; // 如果指数是负数，进行调整
  }
  int16_t mantissa = data & 0x7FF; // 获取11位尾数
  if (mantissa > 1023)
  {
    mantissa = mantissa - 2048; // 处理尾数的符号
  }
  // Debug->print("linear11ToFloat mantissa=");
  // Debug->println(mantissa);
  // Debug->print("linear11ToFloat result=");
  // Debug->println(mantissa * pow(2, exponent11));
  return mantissa * pow(2, exponent11); // 计算最终的浮点数值
}

// 根据VOUT_MODE中的比例因子转换ULINEAR16格式
float PMBus::ulinear16ToFloat(uint16_t data)
{
  uint16_t dec_data = convertHex2Dec(data);
  return dec_data * pow(2, exponent);
}

void PMBus::write_linear(uint8_t reg, float value)
{
  uint16_t linear = float2linear(value);
  write_word(reg, linear);
}

float PMBus::read_direct(uint8_t reg)
{
  Coefficient_t coeff;
  readCoefficients(COEFFICIENTS, &coeff);
  uint16_t data;
  data = read_word(reg);

  return (data * pow(10, 0 - coeff.R) - coeff.b) / coeff.m;
}

// 读取VOUT_MODE寄存器
int8_t PMBus::readVoutMode(uint8_t reg)
{

  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg); // 请求VOUT_MODE寄存器
  I2C->endTransmission(false);

  I2C->requestFrom(PMBUS_ADDRESS, 1); // 请求1字节数据

  // 确保有数据可读，避免潜在的问题
  if (I2C->available() < 1)
  {
    Serial.println("Error: No data available from VOUT_MODE register");
    return 0; // 错误处理，返回 0 或其他默认值
  }

  uint8_t voutMode = I2C->read(); // 读取VOUT_MODE寄存器的值

  // VOUT_MODE 的低5位包含比例因子指数（通常表示为2的幂）
  uint8_t exponent = voutMode & 0x1F;
  if (exponent > 15)
  {
    exponent = exponent - 32; // 如果指数是负数，进行调整
  }
  return exponent;
}

bool PMBus::GetVOutFormat()
{
  // 读取 VOUT_MODE 寄存器
  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(VOUT_MODE); // VOUT_MODE 寄存器地址
  I2C->endTransmission(false);
  I2C->requestFrom(PMBUS_ADDRESS, 1); // 请求 1 字节

  if (I2C->available() < 1)
  {
    Debug->println("Error: Insufficient data available for VOUT_MODE");
    return false; // 错误处理
  }

  uint8_t voutMode = I2C->read(); // 读取 VOUT_MODE 值

  // 检查 VOUT_MODE 的位来判断格式
  // 假设 VOUT_MODE 的位 0 表示是否使用 LINEAR 格式
  // 你需要根据具体的设备文档调整这部分逻辑
  if (voutMode & 0x01)
  {              // 假设低位为 1 表示 LINEAR 格式
    return true; // LINEAR 格式
  }
  else
  {
    return false; // DIRECT 格式
  }
}

/*
 *  读COEFFICIENTS参数
 */
void PMBus::readCoefficients(uint8_t reg, Coefficient_t *coeff)
{
  // byte regCS = ((0xFF - (reg + (PMBUS_ADDRESS << 1))) + 1) & 0xFF;
  // I2C->beginTransmission(PMBUS_ADDRESS);
  // I2C->write(reg);
  // I2C->write(regCS);
  // I2C->endTransmission(false);

  // I2C->requestFrom(PMBUS_ADDRESS, 5, 1);

  // uint8_t byte1 = I2C->read();
  // uint8_t byte2 = I2C->read();
  // uint8_t byte3 = I2C->read();
  // uint8_t byte4 = I2C->read();
  // uint8_t byte5 = I2C->read();

  // coeff->m = byte1 | (byte2 << 8);
  // coeff->b = byte3 | (byte4 << 8);
  // coeff->R = byte5;
  I2C->beginTransmission(PMBUS_ADDRESS);
  I2C->write(reg); // COEFFICIENTS 命令地址
  I2C->endTransmission(false);

  I2C->requestFrom(PMBUS_ADDRESS, 6); // 请求 6 字节的数据

  if (I2C->available() < 6)
  {
    Serial.println("Error: Insufficient data available for COEFFICIENTS");
    return;
  }

  // 读取 M, B 和 R 值
  coeff->m = (I2C->read() | (I2C->read() << 8)); // 读取 2 字节 M
  coeff->b = (I2C->read() | (I2C->read() << 8)); // 读取 2 字节 B
  coeff->R = I2C->read();                        // 读取 1 字节 R
  I2C->read();

  Debug->print("Coefficient_M: ");
  Debug->println(coeff->m);
  Debug->print("Coefficient_B: ");
  Debug->println(coeff->b);
  Debug->print("Coefficient_R: ");
  Debug->println(coeff->R);
}

float PMBus::linear2float(uint16_t u16)
{
  float value = 0;
  UniLinear uniLinear;
  uniLinear.u16 = u16;

  value = uniLinear.linear.y * pow(2.0f, uniLinear.linear.n);

  return value;
}

uint16_t PMBus::float2linear(float value)
{
  UniLinear uniLinear;
  // 初始化为避免未初始化的警告
  uniLinear.u16 = 0;

  // 计算n的值，即所需的偏移量
  // 因为n是5位，所以它的范围是从-15到15（包括0）
  int n = floor(log2(value));
  if (n < -15)
    n = -15; // 如果n太小，则将其限制在最小值
  if (n > 15)
    n = 15; // 如果n太大，则将其限制在最大值

  // 计算y的值，即偏移量调整后的值
  // y应该是有符号的，所以需要将其转换为11位有符号整数
  int y = round(value / pow(2.0f, n));
  if (y < -2048)
    y = -2048; // 如果y太小，则将其限制在最小值
  if (y > 2047)
    y = 2047; // 如果y太大，则将其限制在最大值

  // 将计算出的y和n值放入联合体中
  uniLinear.linear.y = y;
  uniLinear.linear.n = n;

  // 返回联合体中的u16值
  return uniLinear.u16;
}

uint16_t PMBus::convertlinear11(uint16_t value)
{
  // int16_t mantissa = value << (-exponent);  // 计算 mantissa，根据 exponent 进行位移

  // // 将 exponent 和 mantissa 打包为 16-bit 数据
  // uint16_t linear11Value = (mantissa & 0x07FF) | ((exponent & 0x1F) << 11);
  // return linear11Value;

  // 计算 mantissa，根据 exponent 进行位移或缩放
  int16_t mantissa = (int16_t)(value * pow(2, -exponent));

  // 将 exponent 和 mantissa 打包为 16-bit 数据
  uint16_t linear11Value = (mantissa & 0x07FF) | ((exponent & 0x1F) << 11);
  return linear11Value;
}

bool PMBus::checkOnOffState()
{
  if (V_out < 1)
    return false;
  else
    return true;
}

bool PMBus::checkDeviceOnline()
{
  I2C->beginTransmission(PMBUS_ADDRESS);
  byte error = I2C->endTransmission();

  if (error != 0)
  {
    return false;
  }
  return true;
}

void PMBus::readMFR()
{
  mfr_id = read_string(MFR_ID);
  delay(1);
  mfr_model = read_string(MFR_MODEL);
  delay(1);
  mfr_revision = read_string(MFR_REVISION);
  delay(1);
  mfr_location = read_string(MFR_LOCATION);
  delay(1);
  mfr_date = read_string(MFR_DATE);
  delay(1);
  mfr_serial = read_string(MFR_SERIAL);
  delay(1);

  bReadMFR = true;

#if DIAGNOSTICS
  if (Debug)
  {
    Debug->println("manf.:    " + mfr_id);
    Debug->println("model:    " + mfr_model);
    Debug->println("revision: " + mfr_revision);
    Debug->println("location: " + mfr_location);
    Debug->println("date:     " + mfr_date);
    Debug->println("serial:   " + mfr_serial);
  }
#endif
}

void PMBus::recordTurnOnTime()
{
  turn_on_time = millis();
}

void PMBus::getRunTime()
{
  total_power_on = millis() - turn_on_time;
  int days, years;

  days = (int)((long int)total_power_on / 86400l);
  years = days / 365;
  char text[50];
  sprintf(text, "on time:  %lu s (%08lx) \r\n", total_power_on, total_power_on);
  Debug->print(text);
  sprintf(text, "%d days, %d years\r\n", days, years);
  Debug->print(text);
}

float PMBus::readFanCommand()
{
  fanSpeed = read_linear(FAN_COMMAND_1);
  return fanSpeed;
}

void PMBus::writeFanCommand(int value)
{
  //write_linear(FAN_COMMAND_1, value);
  
  //new method
  uint16_t linear11 = convertlinear11(value*100);
  write_word(FAN_COMMAND_1, linear11);
}
