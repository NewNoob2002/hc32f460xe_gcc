#include <Arduino.h>
#include "init.h"

[[noreturn]] int main()
{
    core_init();
    pinMode(PC13, OUTPUT);
    while (true)
    {
        digitalWrite(PC13, HIGH);
        delay_ms(1000);
        digitalWrite(PC13, LOW);
        delay_ms(1000);
    }
    
}