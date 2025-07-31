#ifndef CORE_TYPES_H_
#define CORE_TYPES_H_

#ifdef __cplusplus
#include <cstdbool>
#include <cstdint>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#else
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

typedef bool boolean;
typedef uint8_t byte;
typedef uint16_t word;

typedef void (*voidFuncPtr)();
typedef void (*voidFuncPtrWithArg)(void*);

typedef int16_t gpio_pin_t;

#endif // CORE_TYPES_H_
