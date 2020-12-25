#ifndef STDSTREAM_H
#define	STDSTREAM_H

#include "stdtypes.h"

typedef int (*Puts)(void* data, const char* str, unsigned int size);
typedef int (*Gets)(void* data, const char* buf, unsigned int size);

struct StdStream 
{
    void* data;
    
    // Stream functions
    void (*open)();
    void (*close)();
    Puts puts;
    Gets gets;
};

extern const struct StdStream stdio;

#endif	/* STDSTREAM_H */

