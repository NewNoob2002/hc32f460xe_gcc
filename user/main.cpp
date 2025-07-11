#include <Arduino.h>
#include "init/init.h"
#include "MillisTaskManager.h"
#include "soft_i2c.h"
#include "bq40z50.h"
#include "mp2762a.h"
#include "led.h"

static MillisTaskManager taskManager;

void loopTask()
{
    uint8_t status = mp2762getChargeStatus();
    printf("charge status: %d\n", status);
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
    Led_Init();
    wire.begin();
    wire.setClock(200000);
    bq40z50Begin(wire);
    if(mp2762aBegin(wire) == false)
    {
        //TODO: set a flag to change firmware to V1.3.0
    }

    taskManager.Register(loopTask, 1000);
    taskManager.Register(Led_Update, 20);
    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}