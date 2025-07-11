#include "charger_control.h"
#include "mcu_config.h"
#include <Arduino.h>

void Charger_Control_Init()
{
    /*CHARGER_ENABLE_PIN is used to enable the charger*/
    pinMode(CHARGER_ENABLE_PIN, OUTPUT);
    pinMode(CHARGER_SWITCH_PIN, OUTPUT);
    /*USB_SWITCH_PIN is used to control the USB power*/
    pinMode(USB_SWITCH_PIN, OUTPUT);

    /*CHARGER_CTRL_PIN1 and CHARGER_CTRL_PIN2 are used to control the charger*/
    pinMode(CHARGER_CTRL_PIN1, INPUT_PULLUP);
    pinMode(CHARGER_CTRL_PIN2, INPUT_PULLUP);
    /*CHARGER_ADC_DETECT_PIN is used to detect the charger*/
    pinMode(CHARGER_ADC_DETECT_PIN, INPUT_PULLUP);
}

void USB_Switch_Control(uint8_t state)
{
    digitalWrite(USB_SWITCH_PIN, state);
}

void Charger_Enable_Control(uint8_t state)
{
    digitalWrite(CHARGER_ENABLE_PIN, state);
}

void Charger_Switch_Control(unsigned short select)
{
    if (500 == select)
    {
        digitalWrite(CHARGER_SWITCH_PIN, LOW);
    }
    else if (3000 == select)
    {
        digitalWrite(CHARGER_SWITCH_PIN, HIGH);
    }
    else
    {
        digitalWrite(CHARGER_SWITCH_PIN, LOW);
    }
}