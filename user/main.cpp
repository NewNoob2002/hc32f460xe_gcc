#include <Arduino.h>
#include "delay.h"
#include "init/init.h"
#include "MillisTaskManager.h"
#include "mcu_define.h"
#include "soft_i2c.h"
#include "battery.h"
#include "led.h"
#include "power_control.h"

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
    if(cause.enSoftware == Set)
    {
        MCU_ON_OFF_PIN_HIGH();
        printf("software reset\n");
    }
    wire.begin();
    wire.setClock(100000);
    bq40z50_begin();
    if(mp2762a_begin(&batteryState) == false)
    {
        //TODO: set a flag to change firmware to V1.3.0
    }
        // get reset cause
    while(true)
    {
        if(cause.enSoftware == Set)
            break;
        else if(get_power_off_trigger_count() > 6)
            break;
        
        Adc1_polling();
        delay_ms(300);
    }
    reset_power_off_trigger_count();
    Led_Power_switch(HIGH);
    MCU_ON_OFF_PIN_HIGH();

    taskManager.Register(loopTask, 100);
    taskManager.Register(Led_Update, 20);
    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}