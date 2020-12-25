#ifndef IO_MAP_H
#define	IO_MAP_H

#include "../../../lib/types/register.h"

struct PortSfr
{
    atomic_reg(tris);
    atomic_reg(port);
    atomic_reg(lat);
    atomic_reg(odc);
    atomic_reg(cnpug);
    atomic_reg(cnpdg);
};

struct PortMap 
{
    struct PortSfr* portSfr;
    atomic_regptr(analog);
    reg_t digitalMask;
    reg_t analogMask;
};

extern const struct PortMap portMappingTable[];

#endif	/* IO_MAP_H */

