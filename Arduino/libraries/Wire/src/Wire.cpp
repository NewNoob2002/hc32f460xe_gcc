#include <Arduino.h>
#include "Wire.h"

i2c_peripheral_config_t I2C2_config = {
    .register_base    = M4_I2C2,
    .clock_id         = PWC_FCG1_PERIPH_I2C2,
    .scl_pin_function = Func_I2c2_Scl,
    .sda_pin_function = Func_I2c2_Sda,
};

TwoWire Wire(&I2C2_config, PA9, PA8);

#define REG_TO_I2Cx(reg) ((reg == CM_I2C1) ? "I2C1" : (reg == CM_I2C2) ? "I2C2" \
                                                  : (reg == CM_I2C3)   ? "I2C3" \
                                                                       : "Unknown")
//#define WIRE_ENABLE_DEBUG
#ifdef WIRE_ENABLE_DEBUG
#define WIRE_DEBUG_PRINTF(fmt, ...) CORE_DEBUG_PRINTF("[%s] " fmt, REG_TO_I2Cx(this->_config->register_base), ##__VA_ARGS__)
#else
#define WIRE_DEBUG_PRINTF(fmt, ...)
#endif
TwoWire::TwoWire(i2c_peripheral_config_t *config, const gpio_pin_t scl_pin, const gpio_pin_t sda_pin)
{
    this->_config  = config;
    this->_scl_pin = scl_pin;
    this->_sda_pin = sda_pin;
    this->_clock_frequency = 0;
    this->isInitialized = false;
#ifdef _WIRE_USE_RINGBUFFER
    this->rxbuff = (uint8_t *)lwmem_malloc(WIRE_BUFF_SIZE);
    this->txbuff = (uint8_t *)lwmem_malloc(WIRE_BUFF_SIZE);

    lwrb_init(&this->rx_rb_t, this->rxbuff, WIRE_BUFF_SIZE);
    lwrb_init(&this->tx_rb_t, this->txbuff, WIRE_BUFF_SIZE);
#endif
}

void TwoWire::begin(const uint32_t clockFreq)
{
    this->_clock_frequency = clockFreq;

    GPIO_SetFunc(this->_scl_pin, this->_config->scl_pin_function);
    GPIO_SetFunc(this->_sda_pin, this->_config->sda_pin_function);

    PWC_Fcg1PeriphClockCmd(this->_config->clock_id, Enable);

    stc_i2c_init_t stcI2cInit;
    float32_t fErr;

    (void)I2C_DeInit(this->_config->register_base);
    MEM_ZERO_STRUCT(stcI2cInit);
    if (this->_clock_frequency <= 100 * 1000) {
        stcI2cInit.u32ClockDiv = I2C_CLK_DIV8;
        stcI2cInit.u32Baudrate = this->_clock_frequency;
        stcI2cInit.u32SclTime  = 3UL;
    } else if (this->_clock_frequency == 400 * 1000) {
        stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
        stcI2cInit.u32Baudrate = this->_clock_frequency;
        stcI2cInit.u32SclTime  = 5UL;
    }

    if (const en_result_t result = I2C_Init(this->_config->register_base, &stcI2cInit, &fErr); result != Ok) {
        WIRE_DEBUG_PRINTF("Failed to initialize I2C, error:%f, ret = %d", fErr, result);
        return;
    }


    I2C_BusWaitCmd(this->_config->register_base, Enable);


    this->isInitialized = true;
    WIRE_DEBUG_PRINTF("I2c init success, in mode: master\n");
}

void TwoWire::end() const {
    I2C_DeInit(this->_config->register_base);
}

bool TwoWire::beginTransmission(const uint8_t address) const {
    bool result     = false;

    I2C_Cmd(this->_config->register_base, Enable);

    I2C_SoftwareResetCmd(this->_config->register_base, Enable);
    I2C_SoftwareResetCmd(this->_config->register_base, Disable);
    if (Ok == I2C_Start(this->_config->register_base, WIRE_TIMEOUT)) {
        if (Ok == I2C_TransAddr(this->_config->register_base, address, I2CDirTrans, WIRE_TIMEOUT)) {
            result = true;
        }
    }
    return result;
}

void TwoWire::endTransmission(const bool stopBit) const {
    if (stopBit) {
        // Stop by software
        I2C_Stop(this->_config->register_base, WIRE_TIMEOUT);
        // Disable I2C
        I2C_Cmd(this->_config->register_base, Disable);
    }
}

size_t TwoWire::write(const uint8_t data) const {
    if (Ok == I2C_TransData(this->_config->register_base, &data, 1, WIRE_TIMEOUT)) {
        return 1;
    }
    return 0;
}

size_t TwoWire::write(const uint8_t *data, const size_t quantity) const
{
    size_t len = 0;
    for (size_t i = 0; i < quantity; i++) {
        len += write(*data++);
    }
    return len;
}

size_t TwoWire::requestFrom(const uint8_t address, const uint8_t register_address, uint8_t *buffer, uint8_t quantity, const bool sendStop) const {
    if (write(register_address) == 0) {
        WIRE_DEBUG_PRINTF("I2c write register address failed\n");
        return 0;
    }
    en_result_t ret = I2C_Restart(this->_config->register_base, WIRE_TIMEOUT);
    if (ret != Ok) {
        WIRE_DEBUG_PRINTF("I2c restart failed, ret = %d\n", ret);
        return 0;
    }
    if (1UL == quantity) {
        I2C_AckConfig(this->_config->register_base, I2c_NACK);
    }
    ret = I2C_TransAddr(this->_config->register_base, address, I2CDirReceive, WIRE_TIMEOUT);
    if (ret != Ok) {
        WIRE_DEBUG_PRINTF("I2c restart trans addr failed, ret = %d\n", ret);
        return 0;
    }
    if (sendStop) {
        ret = I2C_MasterDataReceiveAndStop(this->_config->register_base, buffer, quantity, WIRE_TIMEOUT);
    }
    I2C_AckConfig(this->_config->register_base, I2c_ACK);
    if (ret != Ok) {
        I2C_Stop(this->_config->register_base, WIRE_TIMEOUT);
        WIRE_DEBUG_PRINTF("I2c read register 0x%02x form 0x%02x  %d bytes failed, return %d\n", register_address, address, quantity, ret);
        quantity = 0;
    } else {
        WIRE_DEBUG_PRINTF("I2c read register 0x%02x form 0x%02x %d bytes success\n", register_address, address, quantity);
    }
    I2C_Cmd(this->_config->register_base, Disable);
    return quantity;
}

bool TwoWire::isDeviceOnline(const uint8_t address) const
{
    if (beginTransmission(address)) {
        endTransmission();
    } else {
        endTransmission();
        return false;
    }
    return true;
}

void TwoWire::scanDevices(const voidFuncPtrWithArg callback) const
{
    for (uint8_t i = 0x01; i < 0x7F; i++) {
        if (isDeviceOnline(i)) {
            if (callback) {
                callback(&i);
            }else{
                CORE_DEBUG_PRINTF("Found device address: %02x\n", i);
            }
        }
        // else{
        //     CORE_DEBUG_PRINTF("Not found device address: %02x\n", i);
        // }
    }
}
