#include <Arduino.h>
#include "bq40z50/bq40z50.h"
#include "LCD.h"
#include "MYSPI.h"

[[noreturn]] int main()
{
    core_init();

    pinMode(PC13, OUTPUT);
    pinMode(PB3, OUTPUT);
    Spi_Config();
    Spi_DmaConfig();
    LCD_Simple_init();
    delay_ms(1000);
    show_logo();

    digitalWrite(PB3, HIGH);
    delay_ms(100);

    Wire.begin();
    Wire.scanDevices();
    bq40z50Begin(Wire);
    while (true)
    {
        static uint32_t last_time = 0;
        if (const uint32_t now = millis(); now - last_time > 1000) {
            last_time = now;
            digitalToggle(PC13);
            printf("Battery percent: %d%%\r\n", bq40z50getRelativeStateOfCharge());
        }
    }
    
}