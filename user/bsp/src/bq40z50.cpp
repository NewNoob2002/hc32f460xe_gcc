#include "bq40z50.h"
#include <stdio.h>

const uint8_t bq40z50deviceAddress = 0x0B;

TwoWire *bq40z50I2c = nullptr;

bool bq40z50_begin(TwoWire *i2c)
{
    if(i2c == nullptr)
        return false;
    bq40z50I2c = i2c;
    return true;
}

uint8_t bq40z50_read8(uint8_t reg)
{
    uint8_t data;
    if(bq40z50I2c->beginTransmission(bq40z50deviceAddress) != I2C_SUCCESS)
    {
        printf("bq40z50_read8: beginTransmission failed\n");
        return 0;
    }
    bq40z50I2c->write(reg);
    if(bq40z50I2c->endTransmission(false) != I2C_SUCCESS)
    {    
        printf("bq40z50_read8: endTransmission failed\n");
        return 0;
    }
    bq40z50I2c->requestFrom(bq40z50deviceAddress, 1);
    data = bq40z50I2c->read();
    #ifdef TWO_WIRE_DEBUG
    printf("bq40z50_read8: reg=%02X, data=%02X\n", reg, data);
    #endif
    return data;
}

uint16_t bq40z50_read16(uint8_t reg)
{
    uint16_t byte;
    if(bq40z50I2c->beginTransmission(bq40z50deviceAddress) != I2C_SUCCESS)
    {
        printf("bq40z50_read16: beginTransmission failed\n");
        return 0;
    }
    bq40z50I2c->write(reg);
    if(bq40z50I2c->endTransmission(false) != I2C_SUCCESS)
    {
        printf("bq40z50_read16: endTransmission failed\n");
        return 0;
    }
    bq40z50I2c->requestFrom(bq40z50deviceAddress, 2);
    uint8_t lsb = bq40z50I2c->read();
    uint8_t msb = bq40z50I2c->read();
    byte = (msb << 8) | lsb;
    #ifdef TWO_WIRE_DEBUG
    printf("bq40z50_read16: reg=%02X, data=%04X\n", reg, byte);
    #endif
    return byte;
}

float bq40z50getTemperatureC() {
    uint16_t temperature = bq40z50_read16(BQ40Z50_TEMPERATURE); // In 0.1 K
    float tempC = temperature / 10.0;
    tempC -= 273.15;
    return (tempC);
}






