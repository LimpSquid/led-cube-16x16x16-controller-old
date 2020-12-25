#ifndef SCHEDULER_TIMER_DEF_H
#define SCHEDULER_TIMER_DEF_H

#define SCHEDULER_NO_HW_TIMER

#if defined(__PIC32MX__)
    #if (__PIC32_FEATURE_SET__ == 330)  ||  \
        (__PIC32_FEATURE_SET__ == 350)  ||  \
        (__PIC32_FEATURE_SET__ == 370)  ||  \
        (__PIC32_FEATURE_SET__ == 430)  ||  \
        (__PIC32_FEATURE_SET__ == 450)  ||  \
        (__PIC32_FEATURE_SET__ == 470)

            #define _TMR_REG                TMR5
            #define _TMR_CFG_REG            T5CON
            #define _TMR_CFG_WORD           0xA040 // Timer on; Discontinue in idle mode; 1:16 prescaler
            #define _TMR_PRESCALER          16
            #define _TMR_CFG_EN_BIT         15
            #undef SCHEDULER_NO_HW_TIMER
    #endif
#endif

#endif /* SCHEDULER_TIMER_DEF_H */