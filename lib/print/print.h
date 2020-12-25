#ifndef PRINT_H
#define	PRINT_H

#include "../std/stdtypes.h"
#include <stdarg.h>

typedef int (*Puts)(void* data, const char* str, unsigned int size);

struct PrintStream 
{
    void* data;
    
    // Stream functions
    void (*open)();
    void (*close)();
    Puts puts;
};

/**
 * Redirects the output to another stream
 * @return Returns 'true' on success, otherwise 'false'
 */
bool print_init_stream(const struct PrintStream* stream);

/**
 * Prints formatted data to the stream
 * @param format The formatted data to write
 * @param ... Parameters
 * @return Returns the number of characters written
 */
int print_f(const char* format, ...);

/**
 * Prints formatted data to a string
 * @param str Output string
 * @param format The formatted data to write
 * @param ... Parameters
 * @return Returns the number of characters written
 */
int print_fs(char* str, const char* format, ...);


/**
 * Prints formatted data to the stream
 * @param format The formatted data to  write
 * @param arg Argument list
 * @return Returns the number of characters written
 */
int print_vf(const char* format, va_list arg);

/**
 * Prints formatted data to a string
 * @param str Output string
 * @param format The formatted data to write
 * @param arg Argument list
 * @return Returns the number of characters written
 */
int print_vfs(char* str, const char* format, va_list arg);

#endif	/* PRINT_H */

