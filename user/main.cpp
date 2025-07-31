#include <Arduino.h>
#include "mcu_config.h"
#include "bq40z50/bq40z50.h"
#include "i2c_slave.h"
#include "LCD.h"
#include "MYSPI.h"

online_device online_devices;
SystemInfo_t systemInfo;

volatile uint32_t tick = 0;
volatile uint32_t IIC_ERROR_COUNT = 0;
extern "C" void systick_handle() {
    tick++;
    IIC_ERROR_COUNT++;
}

/*
 * GLOBAL FLAG
 */
uint8_t POWER_OFF_FLAG = 0;

[[noreturn]] int main()
{
    core_init();

    pinMode(PC13, OUTPUT);
    pinMode(PB3, OUTPUT);
    Spi_Config();
    Spi_DmaConfig();
    LCD_Simple_init();

    if (Slave_Initialize() != Ok) {
        CORE_DEBUG_PRINTF("I2c_slave Init failed\n");
    }
    delay_ms(1000);
    show_logo();

    digitalWrite(PB3, HIGH);
    delay_ms(100);

    Wire.begin();
    Wire.scanDevices();
    bq40z50Begin(Wire);
    batteryState.Percent = 55;
    while (true)
    {
        IIC_deal();
        if (tick >= 1000) {
            tick = 0;
            digitalToggle(PC13);
        }
        if(IIC_ERROR_COUNT>=2000)
        {
            CORE_DEBUG_PRINTF("IIC_ERROR_COUNT Reach top\r\n");
            Slave_Initialize();
            Slave_I2C_Mode = MD_RX;
            stcI2cCom.u32RecvDataWriteIndex = 0;
            stcI2cCom.u32TransDataWriteIndex = 0;
            memset(u8SlaveI2CRxBuf, 0, SLAVE_I2C_RX_BUF_LEN);
            memset(&PMessage, 0, sizeof(PanelMessage));
            IIC_ERROR_COUNT = 0;
        }
    }
    
}