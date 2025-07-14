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

static MillisTaskManager taskManager;

present_device present_devices;
online_device online_devices;
SystemParameter DisplayPannelParameter;

void loopTask()
{
    battery_update();
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

    memset(&DisplayPannelParameter, 0, sizeof(SystemParameter));
    memcpy(DisplayPannelParameter.hw_version, HARDWARE_VERSION, strlen(HARDWARE_VERSION));
    memcpy(DisplayPannelParameter.sw_version, SOFTWARE_VERSION, strlen(SOFTWARE_VERSION));

    /* Initialize I2C peripheral and enable function*/
    if (Ok != Slave_Initialize()) {
        delay_ms(1200);
        Led_Power_switch(LOW);
        NVIC_SystemReset();
        Led_Power_switch(HIGH);
    }
    rxBufferClear();
    txBufferClear();

    taskManager.Register(loopTask, 100);
    taskManager.Register(Led_Update, 20);

    printf("Setup done, begin task loop ........................\n");
    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}