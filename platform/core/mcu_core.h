#ifndef MCU_CORE_H
#define MCU_CORE_H

//#ifndef DONT_USE_STDLIB
#include <stdio.h>
//#include <stdarg.h>
//#include <string.h>
//#endif

#include "delay.h"
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif
void HardFault_Init();
	
#ifdef __cplusplus
}
#endif

#endif