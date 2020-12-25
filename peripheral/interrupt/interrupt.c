#include "interrupt.h"
#include "mapping/interrupt_map.h"
#include "../../lib/types/register.h"
#include "../../lib/utils/bitwise.h"
#include "../../lib/print/assert.h"

#define INTCON_MVEC_BIT     BIT_SHIFT(12)

void interrupt_enable(const enum InterruptRequest intReq, const enum InterruptPriority priority)
{
    ASSERT(intReq < INTERRUPT_REQUEST_COUNT);
    ASSERT(priority < INTERRUPT_PRIORITY_COUNT);

    const struct InterruptMap* interruptMap = &interruptMappingTable[intReq];
    volatile regptr_t iec = interruptMap->enable;
    volatile regptr_t ipc = interruptMap->priority;
    ASSERT(iec != NULL && ipc != NULL);
   
    atomic_ptr_set(iec, interruptMap->enableMask);
    atomic_ptr_clr(ipc, interruptMap->priorityMask); // Clear register before set
    atomic_ptr_set(ipc, interruptMap->priorityMask  & (priority << compute_lsb_pos(interruptMap->priorityMask)));
}

void interrupt_disable(const enum InterruptRequest intReq)
{
    ASSERT(intReq < INTERRUPT_REQUEST_COUNT);
    
    const struct InterruptMap* interruptMap = &interruptMappingTable[intReq];
    volatile regptr_t iec = interruptMap->enable;
    volatile regptr_t ipc = interruptMap->priority;
    ASSERT(iec != NULL && ipc != NULL);
    
    atomic_ptr_clr(iec, interruptMap->enableMask);
    atomic_ptr_clr(ipc, interruptMap->priorityMask);
}

void interrupt_set_priority(const enum InterruptRequest intReq, const enum InterruptPriority priority)
{
    ASSERT(intReq < INTERRUPT_REQUEST_COUNT);
    ASSERT(priority < INTERRUPT_PRIORITY_COUNT);

    const struct InterruptMap* interruptMap = &interruptMappingTable[intReq];
    volatile regptr_t ipc = interruptMap->priority;
    ASSERT(ipc != NULL);

    atomic_ptr_clr(ipc, interruptMap->priorityMask); // Clear register before set
    atomic_ptr_set(ipc, interruptMap->priorityMask & (priority << compute_lsb_pos(interruptMap->priorityMask)));
}

void interrupt_set_sub_riority(const enum InterruptRequest intReq, const enum InterruptSubPriority priority)
{
    ASSERT(intReq < INTERRUPT_REQUEST_COUNT);
    ASSERT(priority < INTERRUPT_SUB_PRIORITY_COUNT);
    
    const struct InterruptMap* interruptMap = &interruptMappingTable[intReq];
    volatile regptr_t ipc = interruptMap->subPriority;
    ASSERT(ipc != NULL);
    
    atomic_ptr_clr(ipc, interruptMap->subPriorityMask); // Clear register before set
    atomic_ptr_set(ipc, interruptMap->subPriorityMask & (priority << compute_lsb_pos(interruptMap->subPriorityMask)));
}

inline unsigned char __attribute__((always_inline))interrupt_get_flag(const enum InterruptRequest intReq)
{
    ASSERT(intReq < INTERRUPT_REQUEST_COUNT);
    
    const struct InterruptMap* interruptMap = &interruptMappingTable[intReq];
    volatile regptr_t ifs = interruptMap->flag;
    ASSERT(ifs != NULL);
    
    return !!(*ifs & interruptMap->flagMask);
}

inline void __attribute__((always_inline))interrupt_clr_flag(const enum InterruptRequest intReq)
{
    ASSERT(intReq < INTERRUPT_REQUEST_COUNT);
    
    const struct InterruptMap* interruptMap = &interruptMappingTable[intReq];
    volatile regptr_t ifs = interruptMap->flag;
    ASSERT(ifs != NULL);
    
    atomic_ptr_clr(ifs, interruptMap->flagMask);
}

inline void __attribute__((always_inline)) interrupt_enable_mvec()
{
    atomic_reg_set(INTCON, INTCON_MVEC_BIT);
}

inline void __attribute__((always_inline)) interrupt_disable_mvec()
{
    atomic_reg_clr(INTCON, INTCON_MVEC_BIT);
}

inline void __attribute__((always_inline)) interrupt_global_enable()
{
    __builtin_enable_interrupts();
}

inline void __attribute__((always_inline)) interrupt_global_disable()
{
    __builtin_disable_interrupts();
}