#ifndef MCU_CONFIG_H
#define MCU_CONFIG_H

#include "mcu_define.h"

#define SOFTWARE_NAME "X1_Panel"
#define SOFTWARE_VERSION "v1.0.0"
#define SOFTWARE_BUILD_DATE __DATE__
#define SOFTWARE_BUILD_TIME __TIME__
#define HARDWARE_VERSION "v1.0.0"

/*Function Key*/
#define FUNCTION_KEY_PIN PA15
/*power_control_pin*/
#define MCU_ON_OFF_BATTERY_PIN PB3
/*power_key_pin*/
#define POWER_KEY_PIN PA0

/*charger_control*/
#define CHARGER_ENABLE_PIN PA4
#define CHARGER_CTRL_PIN1 PB7
#define CHARGER_CTRL_PIN2 PB6
#define CHARGER_SWITCH_PIN PB10

#define CHARGER_ADC_DETECT_PIN PA1

#define USB_SWITCH_PIN PB8

/*watchdog_pin*/
#define WATCHDOG_PIN PA6

/*Status LED*/
#define POWER_LED_PIN PC13
#define CHARGE_LED_PIN PH2
#define FUNCTION_LED_PIN PB5

extern present_device present_devices;
extern online_device online_devices;
extern SystemParameter DisplayPannelParameter;
extern BatteryState batteryState;

#endif