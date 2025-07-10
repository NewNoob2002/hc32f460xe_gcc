#ifndef __MP2762_H__
#define __MP2762_H__
#include <Wire.h>

#define MP2762A_SETCHARGE_CURRENT 0x02
#define MP2762A_PRECHARGE_CURRENT 0x03
#define MP2762A_PRECHARGE_THRESHOLD 0x07
#define MP2762A_CONFIG_0 0x08
#define MP2762A_CONFIG_1 0x09
#define MP2762A_STATUS 0x13
#define MP2762A_FAULT_REGISTER 0x14
#define MP2762A_BATTERY_VOLTAGE 0x16
#define MP2762A_SYSTEM_VOLTAGE 0x18
#define MP2762A_CHARGE_CURRENT_L 0x1A
#define MP2762A_CHARGE_CURRENT_H 0x1B
#define MP2762A_INPUT_VOLTAGE_L 0x1C
#define MP2762A_INPUT_VOLTAGE_H 0x1D
#define MP2762A_INPUT_CURRENT_L 0x1E
#define MP2762A_INPUT_CURRENT_H 0x1F
#define MP2762A_PRECHARGE_THRESHOLD_OPTION 0x30

bool mp2762_begin(TwoWire *i2cBus);
bool mp2762writeRegister(uint8_t reg, const uint8_t value);
en_result_t mp2762ReadRegister8(uint8_t reg, uint8_t *value);
uint16_t mp2762ReadRegister16(uint8_t reg);
float convertBitsToDoubler(uint16_t bitField, float startingBitValue);
// Set the Precharge threshold
// 5.8V, 6.0, 6.2, 6.4, 6.6, 6.8, 7.4, 7.2 (oddly out of order)
void mp2762setFastChargeVoltageMv(uint16_t mVoltLevel);
void mp2762setFastChargeCurrentMa(uint16_t currentLevelMa);
// Set the current limit during precharge phase, in mA
void mp2762setPrechargeCurrentMa(uint16_t currentLevelMa);
float mp2762getBatteryVoltageMv();
float mp2762getSystemVoltageMv();
// uint8_t mp2762getChargeStatus(BatteryState *batteryState);
float mp2762getChargeCurrentMa();
float mp2762getInputVolatgeMv();
float mp2762getInputCurrentMa();






#endif