#ifndef INIT_H__
#define INIT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void core_init();

/**
 * @brief 获取Flash起始地址(LD_FLASH_START参数值)
 * @return Flash起始地址
 */
uint32_t get_flash_start_address(void);

/**
 * @brief 获取当前中断向量表偏移地址
 * @return 中断向量表偏移地址
 */
uint32_t get_vector_table_offset(void);

#ifdef __cplusplus
}
#endif

#endif