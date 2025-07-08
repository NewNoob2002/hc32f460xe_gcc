#include <Arduino.h>
#include "init/init.h"
#include "LCD.h"
#include "MYSPI.h"

int main()
{
    core_init();
    pinMode(PC13, OUTPUT);

    Spi_Config();
    Spi_DmaConfig();
    LCD_Simple_inition();
    showpic_a();

    while (true) {
        digitalToggle(PC13);
        delay_ms(1000);
    }
    return 0;
}