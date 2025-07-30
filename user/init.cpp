#include "init.h"
#include "drivers/sysclock/sysclock.h"
#include "core_debug.h"
#include <Arduino.h>
// #include <cm_backtrace.h>
#include "mcu_config.h"
#include "lwmem/lwmem.h"

uint8_t region1_data[1024 * LWMEM_SIZE];

static lwmem_region_t
    regions[] = {
        {region1_data, sizeof(region1_data)},
        /* Add more regions if needed */
        {nullptr, 0}};

/**
 * @brief check if the last reset was caused by a
 *        configuration fault (e.g. XTAL fault) that could be reoccuring
 */
inline void check_reoccuring_reset_fault()
{
    // get reset cause
    stc_rmu_rstcause_t cause;
    RMU_GetResetCause(&cause);

    // check for possibly reoccurring faults:
#define CHECK_RSTCAUSE(cause, msg) \
    if (cause == Set)              \
    {                              \
        RMU_ClrResetFlag();        \
        panic(msg);                \
    }

    // - XTAL error could be caused by an invalid XTAL config or a bad circuit
    CHECK_RSTCAUSE(cause.enXtalErr, "XTAL error Reset, check XTAL config and circuit");
    // CHECK_RSTCAUSE_NOT_PANIC(cause.enRstPin, "RST pin Reset");
}

void core_init()
{
    PORT_DebugPortSetting(TDI, Disable);
    PORT_DebugPortSetting(TDO_SWO, Disable);
    PORT_DebugPortSetting(TRST, Disable);
    // setup system clock
    clock_init();
    // setup usart
    #ifdef __CORE_DEBUG
    Serial.begin(115200);
    #endif
    // check if the last reset could be reoccurring
    check_reoccuring_reset_fault();
    // setup systick
    delay_init();
    // setup backtrace
    // cm_backtrace_init(SOFTWARE_NAME, HARDWARE_VERSION, SOFTWARE_VERSION " " SOFTWARE_BUILD_DATE " " SOFTWARE_BUILD_TIME);

// // enable flash ICACHE
#if !defined(CORE_DONT_ENABLE_ICACHE)
    EFM_InstructionCacheCmd(Enable);
#endif
    // setup lwmem
    const size_t region_count = lwmem_assignmem(regions);
    CORE_ASSERT(region_count > 0, "LwMEM initialization failed");
    printf("LwMEM initialized and ready to use, region count: %d\r\n", region_count);
}

extern "C" int io_putChar(const char ch)
{
    Serial.write(ch);
    return 1;
}