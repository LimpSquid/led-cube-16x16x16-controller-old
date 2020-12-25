#ifndef ASSERT_H
#define	ASSERT_H

#include <xc.h>
#include <stdarg.h>

#ifdef __DEBUG
#undef ASSERT
#undef INT_ASSERT
#undef SOFT_ASSERT

#undef __assert_failed
#undef __assert_int_failed
#undef __assert_soft_failed

#define ASSERT(expression)          ((expression) ? expression : __assert_failed(expression))
#define INT_ASSERT(expression)      ((expression) ? expression : __assert_int_failed(expression))
#define SOFT_ASSERT(expression)     ((expression) ? expression : __assert_soft_failed(expression))

// Assertion formats
#define __assert_failed(expression)         __assert_print("Assertion failed: %s at line %d in %s of %s", #expression, __LINE__, __FUNCTION__, __FILE__)
#define __assert_int_failed(expression)     __assert_print("Assertion failed: %s at line %d in %s of %s", #expression, __LINE__, __FUNCTION__, __FILE__)
#define __assert_soft_failed(expression)    __assert_print_no_block("Assertion failed: %s at line %d in %s of %s. Continuing...", #expression, __LINE__, __FUNCTION__, __FILE__)

/**
 * Prints the assertion message and blocks the processor
 * @param format The format of an assertion failure
 * @param ... Parameters
 * @note Interrupts are not disabled.
 */
void __assert_print(const char* format, ...);

/**
 * Prints the assertion message and continues operation
 * @param format The format of an assertion failure
 * @param ... Parameters
 */
void __assert_print_no_block(const char* format, ...);

#else
#define ASSERT(expression)          ((void)0)
#define INT_ASSERT(expression)      ((void)0)
#define SOFT_ASSERT(expression)     ((void)0)
#endif

#endif	/* ASSERT_H */

