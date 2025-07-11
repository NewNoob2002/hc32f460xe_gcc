#ifndef _CORE_TYPES_H_
#define _CORE_TYPES_H_
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef bool boolean;
typedef uint8_t byte;
typedef uint16_t word;

typedef void (*voidFuncPtr)(void);

typedef int16_t gpio_pin_t;

#endif // _CORE_TYPES_H_
