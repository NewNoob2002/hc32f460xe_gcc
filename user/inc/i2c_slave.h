//
// Created by gtc on 25-7-31.
//

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include "hc32_ddl.h"
#include "mcu_define.h"

#define I2C_UNIT (M4_I2C1)
/* Define slave device address, for example */
#define SLAVE_ADDRESS (0x11u)
// #define I2C_10BITS_ADDRESS              (1u)

/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT (PortA)
#define I2C_SCL_PIN (Pin03)
#define I2C_SDA_PORT (PortA)
#define I2C_SDA_PIN (Pin02)
#define I2C_GPIO_SCL_FUNC (Func_I2c1_Scl)
#define I2C_GPIO_SDA_FUNC (Func_I2c1_Sda)

#define I2C_INT_EEI_DEF (INT_I2C1_EEI)
#define I2C_INT_RXI_DEF (INT_I2C1_RXI)
#define I2C_INT_TXI_DEF (INT_I2C1_TXI)
#define I2C_INT_TEI_DEF (INT_I2C1_TEI)

#define I2C_FCG_USE (PWC_FCG1_PERIPH_I2C1)
/* Define i2c baud rate */
#define I2C_BAUDRATE (400000ul)

#define TIMEOUT (0x40000UL)

#ifdef __cplusplus
extern "C" {
#endif

void BufWrite(uint8_t u8Data);
uint8_t BufRead(void);
void I2C_TEI_Callback(void);
void I2C_RXI_Callback(void);
void I2C_EEI_Callback(void);
en_result_t I2C_Slave_Receive_IT(uint8_t *pu8RxData, uint32_t u32Size);
en_result_t I2C_Slave_Transmit_IT(uint8_t *pu8TxData, uint32_t u32Size);
en_result_t Slave_Initialize(void);
void IIC_deal(void);

#ifdef __cplusplus
}
#endif
#endif // I2C_SLAVE_H
