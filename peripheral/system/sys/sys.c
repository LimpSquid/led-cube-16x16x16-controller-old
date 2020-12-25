#include "sys.h"
#include <xc.h>

void sys_lock()
{
    SYSKEY = 0x33333333;
}

void sys_unlock()
{
	SYSKEY = 0x33333333;
	SYSKEY = 0xAA996655;
	SYSKEY = 0x556699AA;
}
