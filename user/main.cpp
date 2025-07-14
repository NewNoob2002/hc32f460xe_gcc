#include <Arduino.h>
#include "delay.h"
#include "hc32_common.h"
#include "init/init.h"
#include "MillisTaskManager.h"
#include "mcu_config.h"
#include "soft_i2c.h"
#include "battery.h"
#include "led.h"
#include "power_control.h"
#include "slave_i2c.h"
#include "nm_message.h"

static MillisTaskManager taskManager;

present_device present_devices;
online_device online_devices;
SystemParameter DisplayPannelParameter;

void loopTask()
{
    battery_update();
    if (Slave_i2c_start_flag)	
    {
        printf("Slave_i2c_start_flag\n");
        if ((SLAVE_I2C_COM_BUSY != stcI2cCom.enComStatus) && (MD_RX == Slave_I2C_Mode))
        {
            I2C_Slave_Receive_IT(u8SlaveI2CRxBuf, SLAVE_I2C_RXBUF_LEN);
        }
        Pannel_DecodeNMMessage(&stcI2cCom, &PMessage);
        if (MD_TX == Slave_I2C_Mode)
        {
            I2C_Slave_Transmit_IT(u8SlaveI2CTxBuf, Slave_I2C_TX_Len);
            Slave_I2C_Mode = MD_RX;
            slaveI2c_errorTick = 0;
        }
    }
    if (slaveI2c_errorTick >= 2000)
    {
        printf("slaveI2c_errorTick >= 2000\n");
        Slave_i2c_start_flag = 1;
        /* Initialize I2C peripheral and enable function*/
        Slave_Initialize();
        stcI2cCom.u32RecvDataWriteIndex = 0;
        stcI2cCom.u32TransDataWriteIndex = 0;
        memset(u8SlaveI2CRxBuf, 0, SLAVE_I2C_RXBUF_LEN);
        memset(&PMessage, 0, sizeof(PannelMessage));
        slaveI2c_errorTick = 0;
    }
}

int main()
{
    core_init();

    // init
    Led_Init();
    WatchDog_Init();
    Power_Control_Init();
    stc_rmu_rstcause_t cause;
    RMU_GetResetCause(&cause);
    if (cause.enSoftware == Set) {
        MCU_ON_OFF_PIN_HIGH();
        printf("software reset\n");
    }
    wire.begin();
    wire.setClock(100 * 1000);
    bq40z50_begin();
    if (mp2762a_begin(&batteryState) == false) {
        // TODO: set a flag to change firmware to V1.3.0
    }
    // get reset cause
    while (true) {
        if (cause.enSoftware == Set)
            break;
        else if (get_power_off_trigger_count() > 6)
            break;

        Adc1_polling();
        delay_ms(300);
    }
    reset_power_off_trigger_count();
    Led_Power_switch(HIGH);
    Led_Function_switch(HIGH);
    MCU_ON_OFF_PIN_HIGH();

    Led_Function_switch(LOW);
    delay_ms(1000);

    memset(&DisplayPannelParameter, 0, sizeof(SystemParameter));
    memcpy(DisplayPannelParameter.hw_version, HARDWARE_VERSION, strlen(HARDWARE_VERSION));
    memcpy(DisplayPannelParameter.sw_version, SOFTWARE_VERSION, strlen(SOFTWARE_VERSION));

    /* Initialize I2C peripheral and enable function*/
    if (Ok != Slave_Initialize()) {
        delay_ms(1200);
        Led_Power_switch(LOW);
        printf("Slave_Initialize failed\n");
        NVIC_SystemReset();
    }
    //Init success, set flag to start I2C slave
    Slave_i2c_start_flag = 1;
    stcI2cCom.u32RecvDataWriteIndex = 0;
	stcI2cCom.u32TransDataWriteIndex = 0;
	memset(u8SlaveI2CRxBuf, 0, SLAVE_I2C_RXBUF_LEN);
    stcI2cCom.enMode = MD_RX;

    taskManager.Register(loopTask, 100);
    taskManager.Register(Led_Update, 20);

    printf("Setup done, begin task loop ........................\n");
    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}