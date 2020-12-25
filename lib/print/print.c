#include "print.h"
#include <stddef.h>

#define PRINT_CONVERT_STR_SIZE  50
#define PRINT_MAX_ZERO_PAD      31 // Make sure it fits in the padZero field

#define charIsNumber(ch)        (ch >= '0' && ch <= '9')
#define charNumberToValue(ch)   (ch - '0')
#define nextc(ptr)              (*(++ptr))

struct PrintOptions
{
    unsigned char upperCase     :1;
    unsigned char negative      :1;
    unsigned char alignment     :2; // Not implemented yet
    unsigned char padZero       :5; 
};

enum Alignment 
{
    ALIGN_LEFT = 0,
    ALIGN_RIGHT,
    ALIGN_CENTER
};

static int print_format(const char* format, va_list arg, void* data, Puts puts);
static void print_full_str(const char* str, int size, void* data, Puts puts);

static int print_strlen32(const char* str);
static int print_itoa(int value, char* str, int base, const struct PrintOptions* opt);

static const struct PrintStream* printStream = NULL;

bool print_init_stream(const struct PrintStream* stream)
{
    bool result = false;
    if(stream != NULL) {
        // Clean up old stream
        if(printStream != NULL)
            printStream->close();
        
        printStream = stream;
        printStream->open();
        result = true;
    }
    return result;
}

int print_f(const char* format, ...)
{
    int result = 0;
    if(printStream != NULL) {
        va_list arg;
        va_start(arg, format);
        result = print_format(format, arg, printStream->data, printStream->puts);
        va_end(arg);
    }
    return result;
}

int print_fs(char* str, const char* format, ...)
{
    
}

int print_vf(const char* format, va_list arg)
{
    int result = 0;
    if(printStream != NULL)
        result = print_format(format, arg, printStream->data, printStream->puts);
    return result;
}

int print_vfs(char* str, const char* format, va_list arg)
{
    
}

int print_format(const char* format,  va_list arg, void* data, Puts puts)
{
    struct PrintOptions printOptions = { 0 };
    
    int charsWritten = 0;
    
    const char* marker = format;
    char str[PRINT_CONVERT_STR_SIZE];
    char current = *format;
    
    while(current != '\0') {
        
        if(current == '%') {
            // Write a part of the string
            print_full_str(marker, (format - marker), data, puts);
            charsWritten += (format - marker);
            
            // Advance to next character
            current = nextc(format);
            
            // Zero padding
            if(current == '0') {
                unsigned int factor = 1;
                unsigned int padding = 0;
                current = nextc(format);
                while(current != '\0') {
                    if(charIsNumber(current)) {
                        padding *= factor;
                        padding += charNumberToValue(current);
                        factor *= 10;
                        current = nextc(format);
                    } else
                        break;
                }
                printOptions.padZero = (padding < PRINT_MAX_ZERO_PAD) ? padding : PRINT_MAX_ZERO_PAD;
            }
            
            switch(current) {
                // Unsupported fall through NULL case
                default:
                case '\0':
                    break;
                
                // Signed decimal
                case 'd':
                case 'i': {
                    printOptions.negative = 1;
                    int size = print_itoa(va_arg(arg, int), str, 10, &printOptions);
                    print_full_str(str, size, data, puts);
                    charsWritten += size;
                    current = nextc(format);
                    break;
                }
                
                // Unsigned hexadecimal
                case 'x':
                case 'X': {
                    printOptions.negative = 0;
                    printOptions.upperCase = (current == 'X');
                    int size = print_itoa(va_arg(arg, int), str, 16, &printOptions);
                    print_full_str(str, size, data, puts);
                    charsWritten += size;
                    current = nextc(format);
                    break;
                }
                
                // String
                case 's': {
                    char* str = va_arg(arg, char*);
                    int size = print_strlen32(str);
                    print_full_str(str, size, data, puts);
                    charsWritten += size;
                    current = nextc(format);
                    break;
                }
            }
            
            // Reset marker position
            marker = format;
        } else
            current = nextc(format);
    }
    
    print_full_str(marker, (format - marker), data, puts);
    charsWritten += (format - marker);
    
    return charsWritten;
}

void print_full_str(const char* str, int size, void* data, Puts puts)
{
    const char* start = str;
    const char* end = str + size;
    while(start != end)
        start += (*puts)(data, start, (end - start));
}

int print_strlen32(const char* str)
{
    int length = 0;
    while(1) {
        unsigned int x = *(unsigned int*)str;
        if((x & 0xff) == 0) 
            return length;
        if((x & 0xff00) == 0) 
            return length + 1;
        if((x & 0xff0000) == 0) 
            return length + 2;
        if((x & 0xff000000) == 0) 
            return length + 3;
        
        str += 4;
        length += 4;
    }
}

int print_itoa(int value, char* str, int base, const struct PrintOptions* opt)
{ 
    // Check boundaries of base, default to 10
    if(base < 2 || base > 16)
        base = 10;
    
    // Only base 10 can be negative
    unsigned char negative = (value < 0) && opt->negative && (base == 10);
    unsigned int posValue = negative ? -value : value;
    unsigned int number = posValue;
    unsigned int nDigits;
    
    // Determine how many digits the number has
    for(nDigits = 1; number /= base; nDigits++);
    
    unsigned char zeroPadding = (opt->padZero >= (nDigits + negative)) ? opt->padZero - (nDigits + negative) : 0;
    int length = (zeroPadding + nDigits + negative);
    char* stringBuilder = str + length;
    
    // Insert NULL terminator
    *(stringBuilder--) = '\0';
     
    // Build up the digits
    do {
		int digit = posValue % base;
		*(stringBuilder--) = (digit < 10 ? '0' + digit : (opt->upperCase ? 'A' : 'a') + digit - 10);
		posValue /= base;
    } while(posValue > 0);
    
    // Zero padding
    while(zeroPadding-- > 0)
        *(stringBuilder--) = '0';
    
    // Insert negative sign 
    if(negative)
        *(stringBuilder--) = '-';
    
    return length;
}