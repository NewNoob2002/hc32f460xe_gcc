#include "mcu_config.h"
#include "core_debug.h"
#include "delay.h"
#include "mp2762a/mp2762a.h"

#include "charger_control_new.h"

bool mp2762a_begin(BatteryState *batteryState)
{
    // if (mp2762aBegin(wire)) {
    //     online_devices.mp2762 = true;
    // } else {
    //     return false;
    // }
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
    batteryState->chargeStatus = static_cast<chargeStatus_t>(charge_status);

    return true;
}

void mp2762a_update(BatteryState *batteryState)
{
    const uint8_t charge_status = mp2762getChargeStatus();
    if (CM_VALUE_IN_RANGE(charge_status, 0, 2)) {
        batteryState->chargeStatus = static_cast<chargeStatus_t>(charge_status);
    }
}