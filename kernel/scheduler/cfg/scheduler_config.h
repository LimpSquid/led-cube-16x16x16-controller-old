#ifndef SCHEDULER_CONFIG_H
#define SCHEDULER_CONFIG_H

//#define SCHEDULER_CUSTOM_TIMER        // Indicates custom hardware timer settings should be used
//#define TMR_REG               TMR5    // Hardware timer, must be a 16 bit timer
//#define TMR_CFG_REG           T5CON   // Hardware timer register
//#define TMR_CFG_WORD          0xA040  // Hardware timer configuration word
//#define TMR_PRESCALER         16      // Hardware timer prescaler
//#define TMR_CFG_EN_BIT        15      // Hardware timer enable bit of the configuration word

//#define PBCLK_FREQUENCY       80000000LU  // Frequency of the peripheral bus, only needed when _SYS_CLK and _PB_DIV are not defined

#define EVENT_POOL_SIZE         5
#define ROBIN_TASK_POOL_SIZE    5

#endif /* SCHEDULER_CONFIG_H */