#ifndef REGISTER_H
#define	REGISTER_H

#include <limits.h>

#define reg_t                       unsigned int
#define regptr_t                    reg_t*
#define atomic_regptr_cast(reg)     (void*)(&reg)

#define REG_T_MAX                   UINT_MAX

#define atomic_reg(name)            struct                  \
                                    {                       \
                                        volatile reg_t reg; \
                                        volatile reg_t clr; \
                                        volatile reg_t set; \
                                        volatile reg_t inv; \
                                    }name
    
#define atomic_regptr(name)         struct                  \
                                    {                       \
                                        volatile reg_t reg; \
                                        volatile reg_t clr; \
                                        volatile reg_t set; \
                                        volatile reg_t inv; \
                                    }*name

#define atomic_reg_clr(reg, mask)   *(((regptr_t)&reg) + 1) = mask
#define atomic_reg_set(reg, mask)   *(((regptr_t)&reg) + 2) = mask
#define atomic_reg_inv(reg, mask)   *(((regptr_t)&reg) + 3) = mask

#define atomic_ptr_clr(ptr, mask)   *(((regptr_t)ptr) + 1) = mask
#define atomic_ptr_set(ptr, mask)   *(((regptr_t)ptr) + 2) = mask
#define atomic_ptr_inv(ptr, mask)   *(((regptr_t)ptr) + 3) = mask

#endif	/* REGISTER_H */

