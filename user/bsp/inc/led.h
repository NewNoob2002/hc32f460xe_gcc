#ifndef __LED_H__
#define __LED_H__
#include "mcu_config.h"
#include <stdint.h>

extern ledState powerLed;
extern ledState chargerLed;
extern ledState functionKeyLed;

void Led_Init();
void Led_Update();
void Led_Update_Power();
void Led_Update_Charge();
void Led_Update_Function();

void Led_Power_switch(uint8_t level);
void Led_Charge_switch(uint8_t level);
void Led_Function_switch(uint8_t level);

inline void SetPowerLedBlink(uint32_t rate)
{
    powerLed.currentRate = rate;
}

inline void SetChargeLedBlink(uint32_t rate)
{
    chargerLed.currentRate = rate;
}

inline void SetFunctionLedBlink(uint32_t rate)
{
    functionKeyLed.currentRate = rate;
}

#endif