#pragma once

#include "mcu_define.h"
#include "charger_control_new.h"
#include "charger_control.h"

bool bq40z50_begin();
void checkBatteryLevels(BatteryState *batteryState);

void battery_update();