#include "mp2762.h"

const uint8_t mp2762deviceAddress = 0x5C;
TwoWire *mp2762I2c = nullptr;

bool mp2762_begin(TwoWire *i2c)
{
    if(i2c == nullptr)
        return false;
    mp2762I2c = i2c;
    return true;
}

bool mp2762writeRegister(uint8_t reg, const uint8_t value) {
    // Perform I2C transmit operation
    

    if (mp2762I2c == nullptr)
      return false;
    mp2762I2c->beginTransmission(mp2762deviceAddress);
    mp2762I2c->write(reg);
    mp2762I2c->write(value);
    #ifdef TWO_WIRE_DEBUG
      printf("MP2762 error: Device did not ack write\n");
    #endif
    return true;
}

en_result_t mp2762ReadRegister8(uint8_t reg, uint8_t *value) {

    mp2762I2c->beginTransmission(mp2762deviceAddress);
    mp2762I2c->write(reg);
    if (mp2762I2c->endTransmission() != 0)
      return Error;
  
    mp2762I2c->requestFrom(mp2762deviceAddress, 1);
    if (mp2762I2c->available())
      *value = mp2762I2c->read();
    #ifdef TWO_WIRE_DEBUG
      printf("mp2762readRegister : 0x%x, 0x%02x\r\n", reg, *value);
    #endif
    return Ok;
  }
  
  uint16_t mp2762ReadRegister16(uint8_t reg) {
    uint16_t value = 0;
    if (mp2762I2c == nullptr)
      return 0;
    mp2762I2c->beginTransmission(mp2762deviceAddress);
    mp2762I2c->write(reg);
    if (mp2762I2c->endTransmission() != 0)
      return 0;
    mp2762I2c->requestFrom(mp2762deviceAddress, 2);
    if (mp2762I2c->available()) {
      uint8_t data[2] = {0};
      data[0] = mp2762I2c->read();
      data[1] = mp2762I2c->read();
      value = (data[1] << 8) | data[0];
    }
    #ifdef TWO_WIRE_DEBUG
      printf("mp2762readRegister : 0x%x, 0x%04x\r\n", reg, value);
    #endif
    return value;
  }
  
  // Given a bit field, and a startingBitValue
  // Example: Battery voltage is bit 12.5mV per bit
  float convertBitsToDoubler(uint16_t bitField, float startingBitValue) {
    float totalMv = 0;
    for (int x = 0; x < 16; x++) {
      if (bitField & 0x0001)
        totalMv += startingBitValue;
  
      bitField >>= 1;
  
      startingBitValue *= 2;
    }
    return (totalMv);
  }
  
  // Set the Precharge threshold
  // 5.8V, 6.0, 6.2, 6.4, 6.6, 6.8, 7.4, 7.2 (oddly out of order)
  void mp2762setFastChargeVoltageMv(uint16_t mVoltLevel) {
    // Default to 6.8V (requires option '2')
    uint8_t option = 1;         // This is option 2 confusingly
    uint8_t newVbattPre = 0b01; // Default to 6.8V
  
    if (mVoltLevel <= 5800) {
      option = 0;
      newVbattPre = 0b00; // 5.8V
    } else if (mVoltLevel <= 6000) {
      option = 0;
      newVbattPre = 0b01; // 6.0V
    } else if (mVoltLevel <= 6200) {
      option = 0;
      newVbattPre = 0b10; // 6.2V
    } else if (mVoltLevel <= 6400) {
      option = 0;
      newVbattPre = 0b11; // 6.4V
    } else if (mVoltLevel <= 6600) {
      option = 1;
      newVbattPre = 0b00; // 6.6V
    } else if (mVoltLevel <= 6800) {
      option = 1;
      newVbattPre = 0b01; // 6.8V
    } else if (mVoltLevel <= 7200) {
      option = 1;
      newVbattPre = 0b11; // 7.2V
    } else if (mVoltLevel <= 7400) {
      option = 1;
      newVbattPre = 0b10; // 7.4V
    }
  
    // Set the Precharge bits
    uint8_t status = 0;
    mp2762ReadRegister8(MP2762A_PRECHARGE_THRESHOLD, &status);
    status &= ~(0b11 << 4); // Clear bits 4 and 5
    newVbattPre <<= 4;      // Shift to correct position
    status |= newVbattPre;  // Set bits accordingly
    mp2762writeRegister(MP2762A_PRECHARGE_THRESHOLD, status);
  
    // Set the option bit
    mp2762ReadRegister8(MP2762A_PRECHARGE_THRESHOLD_OPTION, &status);
    status &= ~(1 << 3); // Clear bit 3
    option <<= 3;        // Shift to correct position
    status |= option;    // Set bit accordingly
    mp2762writeRegister(MP2762A_PRECHARGE_THRESHOLD_OPTION, status);
  }
  
  void mp2762setFastChargeCurrentMa(uint16_t currentLevelMa) {
    // defualt to 1A
    //  uint8_t oldBit = 0x00;
    //  mp2762ReadRegister8(MP2762A_SETCHARGE_CURRENT, &oldBit);
    uint8_t newIFast = 0b00100001;
    mp2762writeRegister(MP2762A_SETCHARGE_CURRENT, newIFast);
  }
  
  void mp2762setPrechargeCurrentMa(uint16_t currentLevelMa) {
    uint8_t newIPre = 0b0011; // Default to 180mA
  
    if (currentLevelMa <= 180)
      newIPre = 0b0011; // 180mA
    else {
      uint8_t steps = (currentLevelMa - 240) / 60; //(480 - 240)/ 60 = 4
      newIPre = 0b0101 + steps;
    }
  
    // Set the Precharge current bits
    uint8_t status = 0;
    mp2762ReadRegister8(MP2762A_PRECHARGE_CURRENT, &status);
    status &= ~(0b1111 << 4); // Clear bits 7, 6, 5, 4
    newIPre <<= 4;            // Shift to correct position
    status |= newIPre;        // Set bits accordingly
    mp2762writeRegister(MP2762A_PRECHARGE_CURRENT, status);
  }
  
  float mp2762getBatteryVoltageMv() {
    uint16_t voltage = mp2762ReadRegister16(MP2762A_BATTERY_VOLTAGE);
    float batteryVoltage = convertBitsToDoubler(
        voltage >>= 6,
        12.5); // Battery voltage is bit 15:6 so we need a 6 bit shift
    return (batteryVoltage);
  }
  
  float mp2762getSystemVoltageMv() {
    uint16_t voltage = mp2762ReadRegister16(MP2762A_SYSTEM_VOLTAGE);
    float batteryVoltage = convertBitsToDoubler(
        voltage >>= 6,
        12.5); // Battery voltage is bit 15:6 so we need a 6 bit shift
    return (batteryVoltage);
  }
  
//   uint8_t mp2762getChargeStatus(BatteryState *batteryState) {
//     uint8_t status = 0;
//     mp2762ReadRegister8(MP2762A_STATUS, &status);
//     status >>= 2;
//     status &= 0b11;
//     if (status == 0)
//       batteryState->chargeStatus = notCharge;
//     else if (status == 1)
//       batteryState->chargeStatus = normalCharge;
//     else if (status == 2)
//       batteryState->chargeStatus = fastCharge;
//     return (status);
//   }
  
  float mp2762getChargeCurrentMa() {
    uint8_t CurrentL = 0, CurrentH = 0;
    mp2762ReadRegister8(MP2762A_CHARGE_CURRENT_L, &CurrentL);
    mp2762ReadRegister8(MP2762A_CHARGE_CURRENT_H, &CurrentH);
  
    uint16_t Current = (CurrentH << 8) | CurrentL;
    Current = (Current >> 6) & 0x03FF;
  
    float charge_current_mA = Current * 12.5;
    return charge_current_mA;
  }
  
  float mp2762getInputVolatgeMv() {
    uint8_t VoltageL = 0, VoltageH = 0;
    mp2762ReadRegister8(MP2762A_INPUT_VOLTAGE_L, &VoltageL);
    mp2762ReadRegister8(MP2762A_INPUT_VOLTAGE_H, &VoltageH);
  
    uint16_t Voltage = (VoltageL << 8) | VoltageH;
    Voltage = (Voltage >> 6) & 0x03FF;
  
    float Input_voltage_mV = Voltage * 25;
    return Input_voltage_mV;
  }
  
  float mp2762getInputCurrentMa() {
    uint8_t CurrentL = 0, CurrentH = 0;
    mp2762ReadRegister8(MP2762A_INPUT_CURRENT_L, &CurrentL);
    mp2762ReadRegister8(MP2762A_INPUT_CURRENT_L, &CurrentH);
  
    uint16_t Current = (CurrentH << 8) | CurrentL;
    Current = (Current >> 6) & 0x03FF;
  
    float charge_current_mA = Current * 6.25;
    return charge_current_mA;
  }
  
//   void mp2762resetSafetyTimer() {
//     uint8_t status = 0;
//     mp2762ReadRegister8(MP2762A_CONFIG_0, &status);
  
//     status &= ~(1 << 4); // Clear the CHG_EN bit
//     mp2762writeRegister(MP2762A_CONFIG_0, status);
  
//     status |= (1 << 4); // Set the CHG_EN bit
//     mp2762writeRegister(MP2762A_CONFIG_0, status);
//   }
  
  void mp2762registerReset() {
    uint8_t status = 0;
    mp2762ReadRegister8(MP2762A_CONFIG_0, &status);
    status |= 1 << 7; // Set REG_RST
    mp2762writeRegister(MP2762A_CONFIG_0, status);
  }