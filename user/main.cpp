#include <Arduino.h>
#include "init/init.h"
#include "MillisTaskManager.h"
#include "bq40z50.h"
#include "mp2762.h"

static MillisTaskManager taskManager;

void loopTask()
{
    digitalToggle(PC13);
    printf("bq40z50_getTemperatureC: %f\n", mp2762getBatteryVoltageMv());
}

int main()
{
    core_init();
    Wire.begin();
    bq40z50_begin(&Wire);
    mp2762_begin(&Wire);
    pinMode(PC13, OUTPUT);
    taskManager.Register(loopTask, 1000);

    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}