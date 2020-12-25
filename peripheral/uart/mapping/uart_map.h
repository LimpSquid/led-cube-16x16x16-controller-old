#ifndef UART_MAP_H
#define	UART_MAP_H

#include "../../../lib/types/register.h"

struct UartSfr
{
    atomic_reg(umode);
    atomic_reg(usta);
    volatile reg_t txreg;
    volatile reg_t txregPadding[3]; // Padding to align registers correctly
    volatile reg_t rxreg;
    volatile reg_t rxregPadding[3]; // Padding to align registers correctly
    atomic_reg(ubrg);
};

struct UartMap 
{
    struct UartSfr* uartSfr;
};

extern const struct UartMap uartMappingTable[];

#endif	/* UART_MAP_H */

