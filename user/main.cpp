#include <Arduino.h>
#include "delay.h"
#include "init/init.h"
#include "MillisTaskManager.h"
#include "soft_i2c.h"
#include "bq40z50.h"
#include "mp2762a.h"
#include "led.h"
#include "power_control.h"

static MillisTaskManager taskManager;

void loopTask()
{
    uint8_t status = mp2762getChargeStatus();
    printf("charge status: %d\n", status);
    Adc1_polling();
    if (status == 0)
    {
        SetChargeLedBlink(0);
        Led_Charge_switch(LOW);
    }
    else if (status == 1 || status == 2)
    {
        SetChargeLedBlink(1000);
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
    if(cause.enSoftware == Set)
    {
        MCU_ON_OFF_PIN_HIGH();
        printf("software reset\n");
    }
    if(cause.enRstPin == Set)
    {
        printf("reset pin reset\n");
    }
    if(cause.enPowerOn== Set)
    {
        printf("power on reset\n");
    }
    wire.begin();
    wire.setClock(200000);
    bq40z50Begin(wire);
    if(mp2762aBegin(wire) == false)
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

        printf("power_off_trigger_count: %d\n", get_power_off_trigger_count());
        Adc1_polling();
        delay_ms(300);
    }
    reset_power_off_trigger_count();
    Led_Power_switch(HIGH);
    MCU_ON_OFF_PIN_HIGH();

    taskManager.Register(loopTask, 1000);
    taskManager.Register(Led_Update, 20);
    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}