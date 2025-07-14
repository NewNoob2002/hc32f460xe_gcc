#ifndef SLAVE_I2C_H
#define SLAVE_I2C_H

#include <stdint.h>

#ifdef __cplusplus
#include "RingBuffer.h"
#endif

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Define I2C unit used for the example */
#define SLAVE_I2C_UNIT    (M4_I2C1)
#define SLAVE_I2C_FCG_USE (PWC_FCG1_PERIPH_I2C1)
/* Define slave device address for example */
#define SLAVE_DEVICE_ADDR (0x11U)
/* If I2C 10 bit address, open the define for I2C_10BITS_ADDR */
// #define I2C_10BITS_ADDR               (1U)

/* Define port and pin for SDA and SCL */

// #define SLAVE_I2C_SCL_PORT                    (GPIO_PORT_A)
#define SLAVE_I2C_SCL_PIN (PA3)
// #define SLAVE_I2C_SDA_PORT                    (GPIO_PORT_A)
#define SLAVE_I2C_SDA_PIN       (PA2)
#define SLAVE_I2C_GPIO_SCL_FUNC (Func_I2c1_Scl)
#define SLAVE_I2C_GPIO_SDA_FUNC (Func_I2c1_Sda)

// #define SLAVE_I2C_EEI_IRQN_DEF                (INT001_IRQn)
// #define SLAVE_I2C_RXI_IRQN_DEF                (INT002_IRQn)
// #define SLAVE_I2C_TXI_IRQN_DEF                (INT003_IRQn)
// #define SLAVE_I2C_TEI_IRQN_DEF                (INT004_IRQn)

#define SLAVE_I2C_INT_EEI_DEF (INT_I2C1_EEI)
#define SLAVE_I2C_INT_RXI_DEF (INT_I2C1_RXI)
#define SLAVE_I2C_INT_TXI_DEF (INT_I2C1_TXI)
#define SLAVE_I2C_INT_TEI_DEF (INT_I2C1_TEI)

#define TIMEOUT               (0x40000UL)

/* Define Write and read data length for the example */
#define SLAVE_I2C_RXBUF_LEN 512
#define SLAVE_I2C_TXBUF_LEN 128
/* Define i2c baudrate */
#define SLAVE_I2C_BAUDRATE (400000UL)

/**
 * @brief I2c communication mode enum
 */
typedef enum {
    MD_TX = 0U,
    MD_RX = 1U,
} stc_i2c_com_mode_t;

/**
 * @brief I2c communication status enum
 */
typedef enum {
    SLAVE_I2C_COM_BUSY = 0U,
    SLAVE_I2C_COM_IDLE = 1U,
} stc_i2c_com_status_t;

/**
 * @brief I2c communication structure
 */
typedef struct {
    stc_i2c_com_mode_t enMode; /*!< I2C communication mode*/
    uint32_t u32RxLen;         /*!< I2C communication data length*/
    uint32_t u32TxLen;
    uint8_t *pRxBuf; /*!< I2C communication data buffer pointer*/
    uint8_t *pTxBuf;
    volatile uint32_t u32RecvDataWriteIndex; /*!< I2C communication data transfer index*/
    volatile uint32_t u32RecvDataReadIndex;
    volatile uint32_t u32TransDataWriteIndex;
    volatile uint32_t u32TransDataReadIndex;
    volatile stc_i2c_com_status_t enComStatus; /*!< I2C communication status*/
} stc_i2c_communication_t;

extern stc_i2c_communication_t stcI2cCom;

#ifdef __cplusplus
extern "C" {
#endif

int32_t I2C_Slave_Receive_IT();
int32_t I2C_Slave_Transmit_IT();
bool WriteByte(uint8_t u8Data);
size_t WriteBytes(uint8_t *buffer, size_t size);
int ReadByte();
size_t ReadBytes(uint8_t *buffer, size_t size);

int32_t Slave_Initialize(void);

bool isRxBufferEmpty();
bool isTxBufferEmpty();
void rxBufferClear();
void txBufferClear();

#ifdef __cplusplus
}
#endif

#endif