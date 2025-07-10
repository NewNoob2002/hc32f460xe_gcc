#include <Arduino.h>
#include "init/init.h"

int main()
{
    core_init();
    pinMode(PC13, OUTPUT);

    while (true) {
        digitalToggle(PC13);
        delay_ms(1000);
    }
    return 0;
}