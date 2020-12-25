#include "config.h"
#include "../sys/sys.h"
#include <xc.h>

static bool oscillator_init();

bool config_cpu_init()
{
    bool success = true;
    success &= oscillator_init(); // If some long initialization takes place after the oscillator init, we might want to disable the watchdog or atleast clear it once in a while
    return success;
}

bool oscillator_init()
{   
    sys_unlock();
    // Some registers may be defined by the configuration word
    OSCCONbits.FRCDIV = 1; // Divided by 2, default
    OSCCONbits.CLKLOCK = 1; // Lock clock and PLL selections
    OSCCONbits.SLPEN = 0; // Enter idle mode upon WAIT instruction
    OSCCONbits.UFRCEN = 0; // Use primary oscillator or USB PLL as USB clock source
    OSCCONbits.SOSCEN = 0; // Disable secondary oscillator
    
    OSCTUNbits.TUN = 0; // Calibrated frequency
    sys_lock();
    return !OSCCONbits.CF;
}