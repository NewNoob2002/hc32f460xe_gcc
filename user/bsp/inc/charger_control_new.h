#pragma once

#include "mcu_define.h"

bool mp2762a_begin(BatteryState *batteryState);
void mp2762a_update(BatteryState *batteryState);

inline bool isCharging(BatteryState *batteryState)
{
    return (batteryState->chargeStatus == 1 || batteryState->chargeStatus == 2);
}

inline bool isfastCharging(BatteryState *batteryState)
{
    return (batteryState->chargeStatus == 2);
}

inline bool isSlowCharging(BatteryState *batteryState)
{
    return (batteryState->chargeStatus == 1);
}