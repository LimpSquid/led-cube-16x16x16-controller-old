#include "io.h"
#include "mapping/io_map.h"
#include "../system/sys/sys.h"
#include "../../lib/print/assert.h"

enum IoBit io_configure(const enum IoPort port, const enum IoBit mask, const enum IoMode mode)
{
    enum IoBit rMask = 0;
    if(port >= IO_PORT_COUNT)
        return rMask;
    
    const struct PortMap* portMap = &portMappingTable[port];
    struct PortSfr* portSfr = portMap->portSfr;
    
    if(portSfr != NULL) {
        switch(mode) {
            case IO_DIGITAL_OUTPUT:
                rMask = mask & portMap->digitalMask;
                portSfr->odc.clr = rMask;
                portSfr->tris.clr = rMask;
                if(portMap->analog != NULL)
                    portMap->analog->clr = mask & portMap->analogMask;
                break;
            case IO_DIGITAL_INPUT:
                rMask = mask & portMap->digitalMask;
                portSfr->odc.clr = rMask;
                portSfr->tris.set = rMask;
                if(portMap->analog != NULL)
                    portMap->analog->clr = mask & portMap->analogMask;
                break;
            case IO_ANALOG_OUTPUT:
                rMask = mask & portMap->analogMask;
                if(portMap->analog != NULL) {
                    portSfr->odc.clr = rMask;
                    portMap->analog->set = rMask;
                    portSfr->tris.clr = mask & portMap->digitalMask;
                }
                break;
            case IO_ANALOG_INPUT:
                rMask = mask & portMap->analogMask;
                if(portMap->analog != NULL) {
                    portSfr->odc.clr = rMask;
                    portMap->analog->set = rMask;
                    portSfr->tris.set = mask & portMap->digitalMask;
                }
                break;
            case IO_OPEN_DRAIN:
                rMask = mask & portMap->analogMask;
                portSfr->odc.set = rMask;
                break;
            default:
                break;
        }
    }
    return rMask;
}

void io_digital_write(const enum IoPort port, const enum IoBit mask, const enum IoDirection direction)
{
    ASSERT(port < IO_PORT_COUNT);
    ASSERT(direction < IO_DIRECTION_COUNT);
    
    const struct PortMap* portMap = &portMappingTable[port];
    struct PortSfr* portSfr = portMap->portSfr;
    ASSERT(portSfr != NULL);
    
    volatile regptr_t regClr = &portSfr->lat.clr; 
    *(regClr + direction) = mask & portMap->digitalMask;
}

enum IoBit io_digital_read(const enum IoPort port, const enum IoBit mask)
{
    ASSERT(port < IO_PORT_COUNT);
    
    const struct PortMap* portMap = &portMappingTable[port];
    struct PortSfr* portSfr = portMap->portSfr;
    ASSERT(portSfr != NULL);
    
    return (portSfr->port.reg & mask & portMap->digitalMask);
}

void io_unlock_pps()
{
    sys_unlock();
    CFGCONbits.IOLOCK = 0;
}

void io_lock_pps()
{
    CFGCONbits.IOLOCK = 1;
    sys_lock();
}