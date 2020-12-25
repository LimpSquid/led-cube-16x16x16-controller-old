#ifndef SPI_MAP_H
#define	SPI_MAP_H

#include "../../../lib/types/register.h"

struct SpiSfr
{
    atomic_reg(spicon);
    atomic_reg(spistat);
    volatile reg_t spibuf;
    volatile reg_t spibufPadding[3]; // Padding to align registers correctly
    atomic_reg(spibrg);
    atomic_reg(spicon2);
};

struct SpiMap 
{
    struct SpiSfr* spiSfr;
};

extern const struct SpiMap spiMappingTable[];

#endif	/* SPI_MAP_H */

