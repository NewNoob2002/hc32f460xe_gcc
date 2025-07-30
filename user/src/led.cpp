#include <Arduino.h>
#include "battery.h"
#include "charger_control_new.h"
#include "mcu_config.h"

#include "led.h"
ledState powerLed = {false, 0, 0};
ledState chargerLed = {false, 0, 0};
ledState functionKeyLed = {false, 0, 0};

void Led_Init()
{
    pinMode(POWER_LED_PIN, OUTPUT);
    pinMode(CHARGE_LED_PIN, OUTPUT);
    pinMode(FUNCTION_LED_PIN, OUTPUT);

    digitalWrite(POWER_LED_PIN, LOW);
    digitalWrite(CHARGE_LED_PIN, LOW);
    digitalWrite(FUNCTION_LED_PIN, LOW);
}

//Led Update
void Led_Update()
{
    Led_Update_Power();
    Led_Update_Charge();
    Led_Update_Function();
}

//Led Update
void Led_Update_Power()
{
    if (powerLed.currentRate > 0)
    {
        const uint32_t now = millis();
        if (now - powerLed.lastToggleTime >= powerLed.currentRate)
        {
            powerLed.isOn = !powerLed.isOn;
            digitalWrite(POWER_LED_PIN, powerLed.isOn ? HIGH : LOW);
            powerLed.lastToggleTime = now;
        }
    }
}

//Led Update
void Led_Update_Charge()
{
    if (chargerLed.currentRate > 0)
    {
        const uint32_t now = millis();
        if (now - chargerLed.lastToggleTime >= chargerLed.currentRate)
        {
            chargerLed.isOn = !chargerLed.isOn;
            digitalWrite(CHARGE_LED_PIN, chargerLed.isOn ? HIGH : LOW);
            chargerLed.lastToggleTime = now;
        }
    }

    if(isCharging(&batteryState))
    {
        if(isfastCharging(&batteryState))
        {
            chargerLed.currentRate = 300;
        }else{
            chargerLed.currentRate = 500;
        }
    }else{
        chargerLed.currentRate = 0;
    }
}

//Led Update
void Led_Update_Function()
{
    if (functionKeyLed.currentRate > 0)
    {
        const uint32_t now = millis();
        if (now - functionKeyLed.lastToggleTime >= functionKeyLed.currentRate)
        {
            functionKeyLed.isOn = !functionKeyLed.isOn;
            digitalWrite(FUNCTION_LED_PIN, functionKeyLed.isOn ? HIGH : LOW);
            functionKeyLed.lastToggleTime = now;
        }
    }
}

//Led Control interface
void Led_Power_switch(uint8_t level)
{
    digitalWrite(POWER_LED_PIN, level);
}

void Led_Charge_switch(uint8_t level)
{
    digitalWrite(CHARGE_LED_PIN, level);
}

void Led_Function_switch(uint8_t level)
{
    digitalWrite(FUNCTION_LED_PIN, level);
}