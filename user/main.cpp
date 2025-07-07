#include <Arduino.h>

int main()
{
    PORT_DebugPortSetting(TDI, Disable);
	PORT_DebugPortSetting(TDO_SWO, Disable);
	PORT_DebugPortSetting(TRST, Disable);
    clock_init();
    Serial.begin(115200);
    systick_init();
    pinMode(PC13, OUTPUT);
    while (true)
    {
        digitalToggle(PC13);
        Serial.println("Hello, World!");
        delay_ms(500);
    }
    return 0;
}