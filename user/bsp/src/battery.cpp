#include <Arduino.h>
#include "mcu_config.h"
#include "bq40z50.h"
#include "core_debug.h"

#include "battery.h"

BatteryState batteryState;

const char* getBatteryChargeStatus(uint8_t &chargeStatus)
{
    switch (chargeStatus)
    {
        case 0:
            return "Not Charging";
        case 1:
            return "Normal Charging";
        case 2:
            return "Fast Charging";
        default:
            return "Unknown";
    }
}

void checkBatteryLevels(BatteryState *batteryState)
{
    if (online_devices.bq40z50) {
        /* code */
        batteryState->batteryLevelPercent           = bq40z50getRelativeStateOfCharge();
        batteryState->batteryVoltage                = (bq40z50getVoltageMv() / 1000.0);
        batteryState->batteryTempC                  = bq40z50getTemperatureC();
        // batteryState->batteryChargingPercentPerHour = (float)bq40z50getAverageCurrentMa() /
        //                                               bq40z50getFullChargeCapacityMah() * 100.0;
    }
    if (batteryState->batteryLevelPercent == 0) {
        batteryState->batteryLevelPercent = 50.0;
    }
    if (batteryState->batteryLevelPercent > 98.50 && batteryState->batteryVoltage >= 8.31) {
        batteryState->batteryLevelPercent = 100;
    }
}

bool bq40z50_begin()
{
    if (bq40z50Begin(wire)) {
        online_devices.bq40z50 = true;
        CORE_DEBUG_PRINTF("bq40z50 found\n");
    } else {
        CORE_DEBUG_PRINTF("bq40z50 not found\n");
        online_devices.bq40z50 = false;
        return false;
    }
    checkBatteryLevels(&batteryState);
    return true;
}

void battery_update()
{
    static uint32_t lastBatteryChargerUpdate   = 0;
    static uint32_t lastBatteryFuelGaugeUpdate = 0;
    const uint32_t now                         = millis();
    // 统一处理所有定时任务
    if (now - lastBatteryChargerUpdate >= 1000) {
        lastBatteryChargerUpdate = now;
        if (!online_devices.mp2762) {
            // do nothing
        }
        else {
            mp2762a_update(&batteryState);
        }
    }
    if (now - lastBatteryFuelGaugeUpdate >= 3000) {
        lastBatteryFuelGaugeUpdate = now;
        if (!online_devices.bq40z50) {
            // do nothing
        } else {
            checkBatteryLevels(&batteryState);
        }
#ifdef __DEBUG
        char buffer[128];
        snprintf(buffer, sizeof(buffer),
                 "Batt(%0.02f%%): %0.02fV Type:%s Temp:%0.02f C\n",
                 batteryState.batteryLevelPercent, batteryState.batteryVoltage,
                 getBatteryChargeStatus(batteryState.chargeStatus), batteryState.batteryTempC);
        CORE_DEBUG_PRINTF("%s", buffer);
#endif
    }
}