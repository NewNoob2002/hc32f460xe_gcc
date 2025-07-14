#include <Arduino.h>
#include <cstdint>
#include "slave_i2c.h"

uint8_t u8SlaveI2CRxBuf[SLAVE_I2C_RXBUF_LEN];
uint8_t u8SlaveI2CTxBuf[SLAVE_I2C_TXBUF_LEN];
stc_i2c_communication_t stcI2cCom;

uint8_t Slave_I2C_Mode = MD_RX;
uint32_t Slave_I2C_TX_Len = 0;

uint8_t Slave_i2c_start_flag = 0;

// PannelMessage PMessage;
/**
 * @brief  Slave receive data
 *
 * @param  pu8RxData             Pointer to the data buffer
 * @param  u32Size               Data size
 * @retval int32_t:
 *            - LL_OK:           Success
 *            - LL_ERR_BUSY:     Busy
 */
int32_t I2C_Slave_Receive_IT(uint8_t *pu8RxData, uint32_t u32Size)
{
    int32_t i32Ret = Ok;

    if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
        stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;

        /// stcI2cCom.u32DataIndex = 0U;
        stcI2cCom.enMode   = MD_RX;
        stcI2cCom.u32RxLen = u32Size;
        stcI2cCom.pRxBuf   = pu8RxData;

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
int32_t I2C_Slave_Transmit_IT(uint8_t *pu8TxData, uint32_t u32Size)
{
    int32_t i32Ret = Ok;

    if (SLAVE_I2C_COM_IDLE == stcI2cCom.enComStatus) {
        stcI2cCom.enComStatus = SLAVE_I2C_COM_BUSY;

        /// stcI2cCom.u32DataIndex = 0U;
        stcI2cCom.enMode   = MD_TX;
        stcI2cCom.u32TxLen = u32Size;
        stcI2cCom.pTxBuf   = pu8TxData;

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
void BufWrite(uint8_t u8Data)
{
    /// if (stcI2cCom.u32DataIndex < stcI2cCom.u32Len)
    {
        u8SlaveI2CRxBuf[stcI2cCom.u32RecvDataWriteIndex] = u8Data;
        stcI2cCom.u32RecvDataWriteIndex++;
        if (SLAVE_I2C_RXBUF_LEN == stcI2cCom.u32RecvDataWriteIndex) { 
            stcI2cCom.u32RecvDataWriteIndex = 0; 
        }
    }
}

/**
 * @brief   Static function for buffer read.
 * @param   None
 * @retval  uint8_t             The data read out from buffer.
 */
uint8_t BufRead(void)
{
    uint8_t temp = 0x55;
    if (stcI2cCom.u32TransDataWriteIndex < stcI2cCom.u32TxLen) {
        temp = u8SlaveI2CTxBuf[stcI2cCom.u32TransDataWriteIndex];
        stcI2cCom.u32TransDataWriteIndex++;
    }
    if (stcI2cCom.u32TransDataWriteIndex >= stcI2cCom.u32TxLen) {
        /// temp = 0xFFU;
        stcI2cCom.u32TransDataWriteIndex = 0;
    }

    return temp;
}

/**
 * @brief   I2C EEI(communication error or event) interrupt callback function
 * @param   None
 * @retval  None
 */
void I2C_EEI_Callback(void)
{
    /* If address interrupt occurred */
    if (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_CR2_SLADDR0EN)) {
        I2C_ClearStatus(SLAVE_I2C_UNIT, I2C_CR2_SLADDR0EN | I2C_CR2_NACKIE);
        printf("I2C_EEI_Callback\n");
        if ((MD_TX == stcI2cCom.enMode) && (Set == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_CR2_TENDIE))) {
            /* Enable tx end interrupt function*/
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_TENDIE, Enable);
            /* Write the first data to DTR immediately */
            I2C_WriteData(SLAVE_I2C_UNIT, BufRead());

            /* Enable stop and NACK interrupt */
            I2C_IntCmd(SLAVE_I2C_UNIT, I2C_CR2_STOPIE | I2C_CR2_NACKIE, Enable);
        } else if ((MD_RX == stcI2cCom.enMode) && (Reset == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_TRA))) {
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
                I2C_CR2_TENDIE | 
            I2C_CR2_RFULLIE | 
            I2C_CR2_STOPIE | 
            I2C_CR2_NACKIE, Disable);
        /* Clear STOPF flag */
        I2C_ClearStatus(SLAVE_I2C_UNIT, I2C_CLR_STOPFCLR);
        I2C_Cmd(SLAVE_I2C_UNIT, Disable);
        /* Communication finished */
        stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;
    } else {
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
        (Reset == I2C_GetStatus(SLAVE_I2C_UNIT, I2C_SR_NACKF))) {
        I2C_WriteData(SLAVE_I2C_UNIT, BufRead());
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
        BufWrite(I2C_ReadData(SLAVE_I2C_UNIT));
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
    int32_t i32Ret;
    stc_i2c_init_t stcI2cInit;
    stc_irq_regi_conf_t stcIrqRegCfg;
    float32_t fErr;

    I2C_DeInit(SLAVE_I2C_UNIT);

    stcI2cCom.enComStatus = SLAVE_I2C_COM_IDLE;

    MEM_ZERO_STRUCT(stcI2cInit);
    stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
    stcI2cInit.u32Baudrate = SLAVE_I2C_BAUDRATE;
    stcI2cInit.u32SclTime  = 5U;
    i32Ret                 = I2C_Init(SLAVE_I2C_UNIT, &stcI2cInit, &fErr);

    if (Ok == i32Ret) {
        /* Set slave address*/
#ifdef I2C_10BITS_ADDR
        I2C_SlaveAddrConfig(I2C_UNIT, I2C_ADDR0, I2C_ADDR_10BIT, DEVICE_ADDR);
#else
        I2C_SlaveAdr0Config(SLAVE_I2C_UNIT, Enable, Adr7bit, SLAVE_DEVICE_ADDR);
#endif
        stcIrqRegCfg.enIRQn      = SLAVE_I2C_EEI_IRQN_DEF;
        stcIrqRegCfg.enIntSrc    = SLAVE_I2C_INT_EEI_DEF;
        stcIrqRegCfg.pfnCallback = &I2C_EEI_Callback;
        enIrqRegistration(&stcIrqRegCfg);
        NVIC_ClearPendingIRQ(stcIrqRegCfg.enIRQn);
        NVIC_SetPriority(stcIrqRegCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
        NVIC_EnableIRQ(stcIrqRegCfg.enIRQn);

        stcIrqRegCfg.enIRQn      = SLAVE_I2C_RXI_IRQN_DEF;
        stcIrqRegCfg.enIntSrc    = SLAVE_I2C_INT_RXI_DEF;
        stcIrqRegCfg.pfnCallback = &I2C_RXI_Callback;
        enIrqRegistration(&stcIrqRegCfg);
        NVIC_ClearPendingIRQ(stcIrqRegCfg.enIRQn);
        NVIC_SetPriority(stcIrqRegCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
        NVIC_EnableIRQ(stcIrqRegCfg.enIRQn);

        stcIrqRegCfg.enIRQn      = SLAVE_I2C_TEI_IRQN_DEF;
        stcIrqRegCfg.enIntSrc    = SLAVE_I2C_INT_TEI_DEF;
        stcIrqRegCfg.pfnCallback = &I2C_TEI_Callback;
        enIrqRegistration(&stcIrqRegCfg);
        NVIC_ClearPendingIRQ(stcIrqRegCfg.enIRQn);
        NVIC_SetPriority(stcIrqRegCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
        NVIC_EnableIRQ(stcIrqRegCfg.enIRQn);
    }
    return i32Ret;
}