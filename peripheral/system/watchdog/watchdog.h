#ifndef WATCHDOG_H
#define	WATCHDOG_H

#define disable_wdt()   WDTCONbits.ON = 0
#define enable_wdt()    WDTCONbits.ON = 1
#define clear_wdt()     WDTCONbits.WDTCLR = 1

#endif	/* WATCHDOG_H */

