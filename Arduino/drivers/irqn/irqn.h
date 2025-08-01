#pragma once
#include <hc32_ddl.h>
#include "../../core_debug.h"

#define IRQN_AA_FIRST_IRQN 0                    // 第一种方式：IRQ0-IRQ31
#define IRQN_AA_FIRST_GROUP_COUNT 32            // 第一种方式：32个中断向量
#define IRQN_AA_SECOND_GROUP_START 32           // 第二种方式：IRQ32-IRQ127
#define IRQN_AA_SECOND_GROUP_COUNT 96           // 第二种方式：96个中断向量
#define IRQN_AA_THIRD_GROUP_START 128           // 第三种方式：IRQ128-IRQ143
#define IRQN_AA_THIRD_GROUP_COUNT 16            // 第三种方式：16个中断向量

#define IRQN_AA_AVAILABLE_COUNT (IRQN_AA_FIRST_GROUP_COUNT + IRQN_AA_SECOND_GROUP_COUNT + IRQN_AA_THIRD_GROUP_COUNT)
#define IRQN_AA_LAST_IRQN (IRQN_AA_THIRD_GROUP_START + IRQN_AA_THIRD_GROUP_COUNT - 1)

#ifdef __cplusplus
extern "C"
{
#endif
en_result_t irqn_aa_get(IRQn_Type &irqn, const char *name);
en_result_t irqn_aa_resign(const IRQn_Type &irqn, const char *name);

#ifdef __cplusplus
}
#endif
