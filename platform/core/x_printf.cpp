#include "x_printf.h"
#include <Arduino.h>
#include <stdio.h>
#include <inttypes.h> // for PRIu32, etc.
#include <stdarg.h>   // for va_list, va_start, va_end

extern "C" {
void x_printf(const char *__format, ...) {
    char printf_buff[256];

    va_list args;
    va_start(args, __format);
    vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    
    Serial.print(printf_buff);
}

void x_putchar(char c)
{
    Serial.write(c);
}

} // extern "C"

