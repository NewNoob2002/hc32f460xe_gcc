#ifndef MCU_CONFIG_H
#define MCU_CONFIG_H

#include "mcu_define.h"

#define SOFTWARE_NAME "E1_Panel"
#define SOFTWARE_VERSION "v1.0.0"
#define SOFTWARE_BUILD_DATE __DATE__
#define SOFTWARE_BUILD_TIME __TIME__
#define HARDWARE_VERSION "v1.0.0"

extern ledState powerLed;
extern ledState chargerLed;
extern ledState functionKeyLed;
extern ledState gnssLed;
extern ledState dataLed;

extern uint8_t chargeStatus;

extern present_device present_devices;
extern online_device online_devices;
extern SystemParameter DisplayPannelParameter;

#endif