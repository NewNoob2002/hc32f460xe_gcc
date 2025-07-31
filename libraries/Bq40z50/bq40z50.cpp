#include "bq40z50.h"

TwoWire *bq40z50I2c = nullptr;

bool bq40z50Begin(TwoWire &i2cBus)
{
    if(i2cBus.isDeviceOnline(BQ40Z50_DEVICE_ADDRESS) == false)
        return false;
    bq40z50I2c = &i2cBus;
    return true;
}

uint8_t bq40z50ReadRegister8(const uint8_t reg)
{
    uint8_t data = 0;

    if(bq40z50I2c->beginTransmission(BQ40Z50_DEVICE_ADDRESS))
    {
        bq40z50I2c->requestFrom(BQ40Z50_DEVICE_ADDRESS, reg, &data, 1);
    }
    return data;
}

uint16_t bq40z50ReadRegister16(const uint8_t reg)
{
    uint8_t buf[2];

    if(bq40z50I2c->beginTransmission(BQ40Z50_DEVICE_ADDRESS))
    {
        bq40z50I2c->requestFrom(BQ40Z50_DEVICE_ADDRESS, reg, buf, 2);
    }
    const uint16_t value = (buf[1] << 8) | buf[0];
    return value;
}

float bq40z50getTemperatureC()
{
    const uint16_t temperature = bq40z50ReadRegister16(BQ40Z50_TEMPERATURE); // In 0.1 K
    // float tempC = temperature / 10.0;
    // tempC -= 273.15;
    return (temperature);
}

uint16_t bq40z50getVoltageMv()
{
    return bq40z50ReadRegister16(BQ40Z50_VOLTAGE);
}

uint16_t bq40z50getCurrentMa()
{
    return bq40z50ReadRegister16(BQ40Z50_CURRENT);
}

uint16_t bq40z50getAverageCurrentMa()
{
    return bq40z50ReadRegister16(BQ40Z50_AVERAGE_CURRENT);
}

uint8_t bq40z50getMaxError()
{
    return bq40z50ReadRegister8(BQ40Z50_MAX_ERROR);
}

float bq40z50getRelativeStateOfCharge_float()
{
    const uint16_t nowBatteryCapacity = bq40z50getRemainingCapacityMah();
    const uint16_t fullBatteryCapacity = bq40z50getFullChargeCapacityMah();
    return (nowBatteryCapacity * 100.0 / fullBatteryCapacity);
}

uint16_t bq40z50getRelativeStateOfCharge()
{
    return bq40z50ReadRegister16(BQ40Z50_RELATIVE_STATE_OF_CHARGE);
}

uint8_t bq40z50getAbsoluteStateOfCharge()
{
    return bq40z50ReadRegister8(BQ40Z50_ABSOLUTE_STATE_OF_CHARGE);
}

uint16_t bq40z50getRemainingCapacityMah()
{
    return bq40z50ReadRegister16(BQ40Z50_REMAINING_CAPACITY);
}

uint16_t bq40z50getFullChargeCapacityMah()
{
    return bq40z50ReadRegister16(BQ40Z50_FULL_CHARGE_CAPACITY);
}

uint16_t bq40z50getRunTimeToEmptyMin()
{
    return bq40z50ReadRegister16(BQ40Z50_RUNTIME_TO_EMPTY);
}

uint16_t bq40z50getAverageTimeToEmptyMin()
{
    return bq40z50ReadRegister16(BQ40Z50_AVERAGE_TIME_TO_EMPTY);
}

uint16_t bq40z50getAverageTimeToFullMin()
{
    return bq40z50ReadRegister16(BQ40Z50_AVERAGE_TIME_TO_FULL);
}

uint16_t bq40z50getChargingCurrentMa()
{
    return bq40z50ReadRegister16(BQ40Z50_CHARGING_CURRENT);
}

uint16_t bq40z50getChargingVoltageMv()
{
    return bq40z50ReadRegister16(BQ40Z50_CHARGING_VOLTAGE);
}

uint16_t bq40z50getCycleCount()
{
    return bq40z50ReadRegister16(BQ40Z50_CYCLE_COUNT);
}

uint16_t bq40z50getCellVoltage1Mv()
{
    return bq40z50ReadRegister16(BQ40Z50_CELL_VOLTAGE_1);
}

uint16_t bq40z50getCellVoltage2Mv()
{
    return bq40z50ReadRegister16(BQ40Z50_CELL_VOLTAGE_2);
}

uint16_t bq40z50getCellVoltage3Mv()
{
    return bq40z50ReadRegister16(BQ40Z50_CELL_VOLTAGE_3);
}

uint16_t bq40z50getCellVoltage4Mv()
{
    return bq40z50ReadRegister16(BQ40Z50_CELL_VOLTAGE_4);
}