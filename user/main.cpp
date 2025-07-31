#include <Arduino.h>
#include "init.h"
#include "LCD.h"
#include "MYSPI.h"

[[noreturn]] int main()
{
    core_init();

    pinMode(PC13, OUTPUT);
    Spi_Config();
    Spi_DmaConfig();
    LCD_Simple_init();
    delay_ms(1000);
    show_logo();
    while (true)
    {
        digitalWrite(PC13, HIGH);
        delay_ms(1000);
        digitalWrite(PC13, LOW);
        delay_ms(1000);
    }
    
}