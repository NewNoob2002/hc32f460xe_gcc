#include <Arduino.h>
#include <cstddef>
#include "../gpio/gpio.h"
#include "../irqn/irqn.h"

#include "slave_i2c.h"
static RingBuffer<uint8_t> *rxBuffer = nullptr;
static RingBuffer<uint8_t> *txBuffer = nullptr;

stc_i2c_communication_t stcI2cCom;

/**
 * @brief  Slave receive data
 *
 * @param  pu8RxData             Pointer to the data buffer
 * @param  u32Size               Data size
 * @retval int32_t:
 *            - LL_OK:           Success
 *            - LL_ERR_BUSY:     Busy
 */
int32_t I2C_Slave_Receive_IT()
{
    int32_t i32Ret = Ok;

    if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
        stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;

        stcI2cCom.enMode = MD_RX;

        I2C_Cmd(SLAVE_I2C_UNIT, Enable);
        /* Config slave address match and receive full interrupt function*/
        I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_SLADDR0EN | I2C_CR2_RFULLIE, Enable);
    } else {
        i32Ret = OperationInProgress;
    }

    return i32Ret;
}

/**
 * @brief  Slave transmit data
 *
 * @param  pu8TxData             Pointer to the data buffer
 * @param  u32Size               Data size
 * @retval int32_t:
 *            - LL_OK:           Success
 *            - LL_ERR_BUSY:     Busy
 */
int32_t I2C_Slave_Transmit_IT()
{
    int32_t i32Ret = Ok;

    if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
        stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;

        stcI2cCom.enMode = MD_TX;

        I2C_Cmd(SLAVE_I2C_UNIT, Enable);
        /* Config slave address match interrupt function*/
        I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_SLADDR0EN, Enable);
    } else {
        i32Ret = OperationInProgress;
    }

    return i32Ret;
}

/**
 * @brief   static function for buffer write.
 * @param   [in] u8Data         the data to be write.
 * @retval  None
 */
bool WriteByte(uint8_t u8Data)
{
    bool txOverrun;
    if (stcI2cCom.txBuffer->push(u8Data, true, txOverrun)) {
        if (txOverrun) {
            printf("tx overrun\n");
        }
        return true;
    }
    return false;
}

size_t WriteBytes(uint8_t *buffer, size_t size)
{
    size_t n = 0;
    while (size) {
        if (WriteByte(*buffer++))
            n++;
        else
            break;
    }
    return n;
}

int ReadByte()
{
    uint8_t ch;
    if (stcI2cCom.rxBuffer->pop(ch)) {
        return ch;
    } else {
        return -1;
    }
}

size_t ReadBytes(uint8_t *buffer, size_t size)
{
    size_t count = 0;
    while (count < size) {
        int c = ReadByte();
        if (c == -1) break;
        *buffer++ = c;
        count++;
    }
    return count;
}

bool isRxBufferEmpty()
{
    return stcI2cCom.rxBuffer->isEmpty();
}

bool isTxBufferEmpty()
{
    return stcI2cCom.txBuffer->isEmpty();
}

void rxBufferClear()
{
    stcI2cCom.rxBuffer->clear();
}

void txBufferClear()
{
    stcI2cCom.txBuffer->clear();
}

/**
 * @brief   I2C EEI(communication error or event) interrupt callback function
 * @param   None
 * @retval  None
 */
void I2C_EEI_Callback(void)
{
    /* If address interrupt occurred */
    if (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_SLADDR0F)) {
        I2C_ClearStatus(SLAVE_I2C_UNIT, I2C_CLR_SLADDR0FCLR | I2C_CLR_NACKFCLR);
        printf("I2C_EEI_Callback: address interrupt occurred\n");
        if ((MD_TX == stcI2cCom.enMode) && (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_TRA))) {
            /* Enable tx end interrupt function*/
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_TENDIE, Enable);
            /* Write the first data to DTR immediately */
            uint8_t ch;
            if (stcI2cCom.txBuffer->pop(ch)) {
                I2C_WriteData(SLAVE_I2C_UNIT, ch);
            }

            /* Enable stop and NACK interrupt */
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
        } else if ((MD_RX == stcI2cCom.enMode) && (Reset == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_TRA))) {
            /* Config rx buffer full interrupt function*/
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_RFULLIE, Enable);
            /* Enable stop and NACK interrupt */
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
        } else {
        }
    } else if (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_NACKF)) {
        /* If NACK interrupt occurred */
        /* clear NACK flag*/
        I2C_ClearStatus(SLAVE_I2C_UNIT, I2C_CLR_NACKFCLR);
        /* Stop tx or rx process*/
        if (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_TRA)) {
            /* Config tx end interrupt function disable*/
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_TENDIE, Disable);
            I2C_ClearStatus(SLAVE_I2C_UNIT, I2C_CLR_TENDFCLR);

            /* Read DRR register to release */
            (void)I2C_ReadData(SLAVE_I2C_UNIT);
        } else {
            /* Config rx buffer full interrupt function disable */
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_RFULLIE, Disable);
        }
    } else if (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_STOPF)) {
        /* If stop interrupt occurred */
        /* Disable all interrupt enable flag except SLADDR0IE*/
        I2C_IntCmd(SLAVE_I2C_UNIT,
                   I2C_CR2_TENDIE | I2C_CR2_RFULLIE |
                       I2C_CR2_STOPIE | I2C_CR2_NACKIE,
                   Disable);

        I2C_ClearStatus(SLAVE_I2C_UNIT, I2C_CLR_STOPFCLR);
        I2C_Cmd(SLAVE_I2C_UNIT, Disable);
        /* Communication finished */
        stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;
    } else {
        printf("I2C_EEI_Callback: unknown event\n");
    }
}

/**
 * @brief   I2C TEI(transfer end) interrupt callback function
 * @param   None
 * @retval  None
 */
void I2C_TEI_Callback(void)
{
    if ((Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_TENDF)) &&
        (Reset == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_ACKRF))) {
        uint8_t ch;
        if (stcI2cCom.txBuffer->pop(ch)) {
            I2C_WriteData(SLAVE_I2C_UNIT, ch);
        } else {
            printf("I2C_TEI_Callback: tx buffer empty\n");
        }
    }
}

/**
 * @brief   I2C RXI(receive buffer full) interrupt callback function
 * @param   None
 * @retval  None
 */
void I2C_RXI_Callback(void)
{
    if (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_RFULLF)) {
        bool rxOverrun;
        if (stcI2cCom.rxBuffer->push(I2C_ReadData(SLAVE_I2C_UNIT), true, rxOverrun)) {
            if (rxOverrun) {
                printf("rx overrun\n");
            }
        }
    }
}
/**
 * @brief   Initialize the I2C peripheral for slave
 * @param   None
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR_INVD_PARAM:  Invalid parameter
 */
int32_t Slave_Initialize(void)
{
    GPIO_SetFunc(SLAVE_I2C_SCL_PIN, SLAVE_I2C_GPIO_SCL_FUNC);
    GPIO_SetFunc(SLAVE_I2C_SDA_PIN, SLAVE_I2C_GPIO_SDA_FUNC);

    PWC_Fcg1PeriphClockCmd(SLAVE_I2C_FCG_USE, Enable);

    int32_t i32Ret = Error;
    stc_i2c_init_t stcI2cInit;
    float32_t fErr;

    I2C_DeInit(SLAVE_I2C_UNIT);

    stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;

    MEM_ZERO_STRUCT(stcI2cInit);
    stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
    stcI2cInit.u32Baudrate = SLAVE_I2C_BAUDRATE;
    stcI2cInit.u32SclTime  = 5U;
    i32Ret                 = I2C_Init(SLAVE_I2C_UNIT, &stcI2cInit, &fErr);

    if (Ok == i32Ret) {
        if (rxBuffer == nullptr) {
            rxBuffer           = new RingBuffer<uint8_t>(SLAVE_I2C_RXBUF_LEN);
            stcI2cCom.rxBuffer = rxBuffer;
        }
        if (txBuffer == nullptr) {
            txBuffer           = new RingBuffer<uint8_t>(SLAVE_I2C_TXBUF_LEN);
            stcI2cCom.txBuffer = txBuffer;
        }

        /* Set slave address*/
#ifdef I2C_10BITS_ADDR
        I2C_SlaveAdr0Config(SLAVE_I2C_UNIT, Enable, I2C_ADDR_10BIT, SLAVE_DEVICE_ADDR);
#else
        I2C_SlaveAdr0Config(SLAVE_I2C_UNIT, Enable, Adr7bit, SLAVE_DEVICE_ADDR);
#endif
        { // get auto-assigned irqn and set in irq struct
            IRQn_Type irqn;
            irqn_aa_get(irqn, "SLAVE_I2C_EEI_IRQN_DEF");

            // create irq registration struct
            stc_irq_regi_conf_t irqConf = {
                .enIntSrc    = SLAVE_I2C_INT_EEI_DEF,
                .enIRQn      = irqn,
                .pfnCallback = I2C_EEI_Callback,
            };
            enIrqRegistration(&irqConf);
            NVIC_ClearPendingIRQ(irqConf.enIRQn);
            NVIC_SetPriority(irqConf.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
            NVIC_EnableIRQ(irqConf.enIRQn);
        }

        {
            IRQn_Type irqn;
            irqn_aa_get(irqn, "SLAVE_I2C_RXI_IRQN_DEF");

            stc_irq_regi_conf_t irqConf2 = {
                .enIntSrc    = SLAVE_I2C_INT_RXI_DEF,
                .enIRQn      = irqn,
                .pfnCallback = I2C_RXI_Callback,
            };
            enIrqRegistration(&irqConf2);
            NVIC_ClearPendingIRQ(irqConf2.enIRQn);
            NVIC_SetPriority(irqConf2.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
            NVIC_EnableIRQ(irqConf2.enIRQn);
        }

        {
            IRQn_Type irqn;
            irqn_aa_get(irqn, "SLAVE_I2C_TEI_IRQN_DEF");

            stc_irq_regi_conf_t irqConf3 = {
                .enIntSrc    = SLAVE_I2C_INT_TEI_DEF,
                .enIRQn      = irqn,
                .pfnCallback = I2C_TEI_Callback,
            };
            enIrqRegistration(&irqConf3);
            NVIC_ClearPendingIRQ(irqConf3.enIRQn);
            NVIC_SetPriority(irqConf3.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
            NVIC_EnableIRQ(irqConf3.enIRQn);
        }
    }
    return i32Ret;
}