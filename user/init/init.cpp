#include "init.h"
#include "../drivers/sysclock/sysclock.h"
#include "../drivers/sysclock/sysclock_util.h"
#include "../drivers/sysclock/systick.h"
#include "../core_debug.h"
#include "../core_hooks.h"
#include <Arduino.h>

/**
 * @brief check if the last reset was caused by a
 *        configuration fault (e.g. XTAL fault) that could be reoccuring
 */
inline void check_reoccuring_reset_fault()
{
    // get reset cause
    stc_rmu_rstcause_t cause;
    RMU_GetResetCause(&cause);

    // check for possibly reoccuring faults:
#define CHECK_RSTCAUSE(cause, msg)                                                                    \
    if (cause == Set) {                                                                               \
        x_printf("panic: %s, enter infinite loop\n", msg); \
        while (1) {                                                                                   \
            ;                                                                                         \
        }                                                                                             \
    }
#define CHECK_RSTCAUSE_NOT_PANIC(cause, msg)                                                                    \
    if (cause == Set) {                                                                               \
        x_printf("%s\n", msg);                                                                       \
    }

    // - XTAL error, could be caused by a invalid XTAL config or a bad circuit
    CHECK_RSTCAUSE(cause.enXtalErr, "XTAL error Reset, check XTAL config and circuit");
    CHECK_RSTCAUSE_NOT_PANIC(cause.enRstPin, "RST pin Reset, check RST pin config and circuit");
    CHECK_RSTCAUSE_NOT_PANIC(cause.enPowerOn, "Power on Reset, check Power on config and circuit");
    CHECK_RSTCAUSE_NOT_PANIC(cause.enSoftware, "Software Reset, check Software config and circuit");
}

void core_init()
{
    PORT_DebugPortSetting(TDI, Disable);
    PORT_DebugPortSetting(TDO_SWO, Disable);
    PORT_DebugPortSetting(TRST, Disable);
    // setup vector table offset
    SCB->VTOR = (uint32_t(LD_FLASH_START) & SCB_VTOR_TBLOFF_Msk);
    // setup system clock
    clock_init();
    update_system_clock_frequencies();
    // setup usart
    Serial.begin(115200);
    // check if last reset could be reoccuring
    check_reoccuring_reset_fault();
    // setup systick
    systick_init();

// // enable flash ICACHE
#if !defined(CORE_DONT_ENABLE_ICACHE)
    EFM_InstructionCacheCmd(Enable);
#endif
}