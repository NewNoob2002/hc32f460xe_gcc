#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MCU_ON_OFF_PIN_HIGH()  digitalWrite(MCU_ON_OFF_BATTERY_PIN, HIGH)
#define MCU_ON_OFF_PIN_LOW()   digitalWrite(MCU_ON_OFF_BATTERY_PIN, LOW)

/*power_off_trigger*/
uint8_t get_power_off_trigger_count();
void reset_power_off_trigger_count();
void set_power_off_trigger_count(uint8_t value);
/*init*/
void Adc1_Init();
void MCU_ON_OFF_PIN_Init();
void Power_Control_Init();
void WatchDog_Init();

/*polling*/
void Adc1_polling();

/*watchdog*/
void WatchDog_feed();

#ifdef __cplusplus
}
#endif