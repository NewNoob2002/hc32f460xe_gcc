#include "mcu_config.h"
#include "core_debug.h"
#include "delay.h"
#include "mp2762a.h"

#include "charger_control_new.h"

bool mp2762a_begin(BatteryState *batteryState)
{
    if (mp2762aBegin(wire)) {
        online_devices.mp2762 = true;
        CORE_DEBUG_PRINTF("mp2762a found\n");
    } else {
        CORE_DEBUG_PRINTF("mp2762a not found\n");
        return false;
    }
    // Reset all configure
    mp2762registerReset();
    // Set Charger Current
    mp2762setFastChargeVoltageMv(6600);
    // Setting precharge current to 880mA
    mp2762setPrechargeCurrentMa(500);
    // Setting fast charge current to 1600mA
    mp2762setFastChargeCurrentMa(1600);
    // get charge status
    uint8_t charge_status      = mp2762getChargeStatus();
    batteryState->chargeStatus = charge_status;

    return true;
}

void mp2762a_update(BatteryState *batteryState)
{
    uint8_t charge_status = mp2762getChargeStatus();
    if (CM_VALUE_IN_RANGE(charge_status, 0, 2)) {
        batteryState->chargeStatus = charge_status;
    }

    static uint32_t last_charge_status_time = 0;
    // 非充电时电压监控
    if(millis() - last_charge_status_time > 60 * 1000)
    {
        last_charge_status_time = millis();
        if (!isCharging(batteryState)) {

            const float packVoltage = mp2762getBatteryVoltageMv() / 1000.0f;
            if (batteryState->batteryVoltage < 7.0f) {
                CORE_DEBUG_PRINTF("Pack voltage %0.2fV, resetting safety timer\n",
                                  packVoltage);
                mp2762resetSafetyTimer();
            }
        }
    }
}