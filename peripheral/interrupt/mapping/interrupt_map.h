#ifndef INTERRUPT_MAP_H
#define	INTERRUPT_MAP_H

#include "../../../lib/types/register.h"

struct InterruptMap
{
    volatile regptr_t flag;
    volatile regptr_t enable;
    volatile regptr_t priority;
    volatile regptr_t subPriority;
    
    reg_t flagMask;
    reg_t enableMask;
    reg_t priorityMask;
    reg_t subPriorityMask;
};

extern const struct InterruptMap interruptMappingTable[];

#endif	/* INTERRUPT_MAP_H */

