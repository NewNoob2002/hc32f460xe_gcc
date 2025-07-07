#include <Arduino.h>
#include "init/init.h"

int main()
{
    core_init();
    pinMode(PC13, OUTPUT);
    while (true)
    {
        digitalToggle(PC13);
        Serial.println("Hello, World!");
        delay_ms(500);
    }
    return 0;
}