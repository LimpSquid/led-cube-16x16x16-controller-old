#include "../include/assert.h"
#include "../include/print.h"

static inline void __attribute__((always_inline)) assert_printer(const char* format, va_list arg);
static inline unsigned char __attribute__((always_inline)) assert_halt(); 

void __assert_print(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    assert_printer(format, arg);
    va_end(arg);
    assert_halt();
}

void __assert_print_no_block(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    assert_printer(format, arg);
    va_end(arg);
}

inline void __attribute__((always_inline)) assert_printer(const char* format, va_list arg)
{
    print_vf(format, arg);
}

inline unsigned char __attribute__((always_inline)) assert_halt()
{
    while(1) {
        Nop();
        Nop();
        Nop();
    }
}