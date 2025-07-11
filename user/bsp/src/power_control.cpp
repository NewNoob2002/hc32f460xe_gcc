#include "power_control.h"
#include "mcu_config.h"
#include <Arduino.h>

static uint8_t POWER_OFF_TRIGGER = 0;

void Adc1_Init()
{
    pinMode(POWER_KEY_PIN, INPUT_ANALOG);
}

void MCU_ON_OFF_PIN_Init()
{
    pinMode(MCU_ON_OFF_BATTERY_PIN, OUTPUT);
}

void Power_Control_Init()
{
    Adc1_Init();
    MCU_ON_OFF_PIN_Init();
}

void WatchDog_Init()
{
    pinMode(WATCHDOG_PIN, OUTPUT);
}

void Adc1_polling()
{
    int adc_value = analogRead(POWER_KEY_PIN);
    printf("adc_value: %d\n", adc_value);
    if(adc_value > 800)
    {
        POWER_OFF_TRIGGER++;
    }
}

uint8_t get_power_off_trigger_count()
{
    return POWER_OFF_TRIGGER;
}

void reset_power_off_trigger_count()
{
    POWER_OFF_TRIGGER = 0;
}

void set_power_off_trigger_count(uint8_t value)
{
    POWER_OFF_TRIGGER = value;
}

void WatchDog_feed()
{
    digitalToggle(WATCHDOG_PIN);
}