#pragma once

#include <hc32_ddl.h>
#include <core_types.h>

#define WIRE_TIMEOUT 0x40000UL

#ifndef WIRE_RX_BUFF_SIZE
#define WIRE_RX_BUFF_SIZE 1024
#endif

/* I2C address mode */
#define I2C_ADDR_MD_7BIT                (1U)
#define I2C_ADDR_MD_10BIT               (0U)

#define I2C_ADDR_MD                     (I2C_ADDR_MD_7BIT)
/**
 * @brief I2C peripheral configuration
 */
struct i2c_peripheral_config_t
{
  /**
   * @brief I2C peripheral register base address
   */
  M4_I2C_TypeDef *register_base;

  /**
   * @brief I2C peripheral clock id
   * @note in FCG1
   */
  uint32_t clock_id;

  /**
   * @brief pin function for i2c scl pin
   */
  en_port_func_t scl_pin_function;

  /**
   * @brief pin function for i2c sda pin
   */
  en_port_func_t sda_pin_function;
};

typedef enum slave_work_mode_t{
  SLAVE_MODE_RX,
  SLAVE_MODE_TX
} slave_work_mode_t;

class TwoWire{
public:
  TwoWire(i2c_peripheral_config_t *config, gpio_pin_t scl_pin, gpio_pin_t sda_pin);

  void begin(uint32_t clockFreq = 100 * 1000);

  void end() const;

  void setClock(uint32_t clockFreq){
    this->_clock_frequency = clockFreq;
  }

  [[nodiscard]] bool beginTransmission(uint8_t address) const;
  void endTransmission(bool stopBit = true) const;

  [[nodiscard]] size_t write(uint8_t data) const;
  size_t write(const uint8_t * data, size_t quantity) const;
  size_t requestFrom(uint8_t address, uint8_t register_address, uint8_t *buffer, uint8_t quantity , bool sendStop = true) const;

  [[nodiscard]] bool isDeviceOnline(uint8_t address) const;
  void scanDevices(voidFuncPtrWithArg callback = nullptr) const;
private:
  i2c_peripheral_config_t *_config;
  bool isInitialized;

  gpio_pin_t _scl_pin;
  gpio_pin_t _sda_pin;

  uint32_t _clock_frequency;
};

extern TwoWire Wire;