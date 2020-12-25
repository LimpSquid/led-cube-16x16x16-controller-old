#ifndef INTERRUPT_H
#define	INTERRUPT_H

#include <xc.h>
#include <sys/attribs.h>

enum InterruptRequest
{
    INTERRUPT_CORE_TIMER = 0,
    INTERRUPT_CORE_SOFTWARE0,
    INTERRUPT_CORE_SOFTWARE1,
    INTERRUPT_EXTERNAL,
#ifdef _TMR1
    INTERRUPT_TIMER1,
#endif
#ifdef _ICAP1
    INTERRUPT_INPUT_CAPTURE1_ERROR,
    INTERRUPT_INPUT_CAPTURE,
#endif
#ifdef _OCMP1
    INTERRUPT_OUTPUT_COMPARE1,
#endif
    INTERRUPT_EXTERNAL1,
#ifdef _TMR2
    INTERRUPT_TIMER2,
#endif
#ifdef _ICAP2
    INTERRUPT_INPUT_CAPTURE2_ERROR,
    INTERRUPT_INPUT_CAPTURE2,
#endif
#ifdef _OCMP2
    INTERRUPT_OUTPUT_COMPARE2,
#endif
    INTERRUPT_EXTERNAL2,
#ifdef _TMR3
    INTERRUPT_TIMER3,
#endif
#ifdef _ICAP3
    INTERRUPT_INPUT_CAPTURE3_ERROR,
    INTERRUPT_INPUT_CAPTURE3,
#endif
#ifdef _OCMP3
    INTERRUPT_OUTPUT_COMPARE3,
#endif
    INTERRUPT_EXTERNAL3,
#ifdef _TMR4
    INTERRUPT_TIMER4,
#endif
#ifdef _ICAP4
    INTERRUPT_INPUT_CAPTURE4_ERROR,
    INTERRUPT_INPUT_CAPTURE4,
#endif
#ifdef _OCMP4
    INTERRUPT_OUTPUT_COMPARE4,
#endif
    INTERRUPT_EXTERNAL4,
#ifdef _TMR5
    INTERRUPT_TIMER5,
#endif
#ifdef _ICAP5
    INTERRUPT_INPUT_CAPTURE5_ERROR,
    INTERRUPT_INPUT_CAPTURE5,
#endif
#ifdef _OCMP5
    INTERRUPT_OUTPUT_COMPARE5,
#endif
    INTERRUPT_ADC1_DONE,
    INTERRUPT_FAIL_SAFE_CLOCK_MONITOR,
    INTERRUPT_REAL_TIME_CLOCK,
    INTERRUPT_FLASH_CONTROL,
#ifdef _CMP1
    INTERRUPT_COMPARATOR1,
#endif
#ifdef _CMP2
    INTERRUPT_COMPARATOR2,
#endif
#ifdef _USB
    INTERRUPT_USB,
#endif
#ifdef _SPI1
    INTERRUPT_SPI1_FAULT,
    INTERRUPT_SPI1_RECEIVE_DONE,
    INTERRUPT_SPI1_TRANSMIT_DONE,
#endif
#ifdef _UART1
    INTERRUPT_UART1_FAULT,
    INTERRUPT_UART1_RECEIVE_DONE,
    INTERRUPT_UART1_TRANSFER_DONE,
#endif
#ifdef _I2C1
    INTERRUPT_IIC1_BUS_COLLISION,
    INTERRUPT_IIC1_SLAVE,
    INTERRUPT_IIC1_MASTER,
#endif
#ifdef _PORTA
    INTERRUPT_PORTA_INPUT_CHANGE,
#endif
#ifdef _PORTB   
    INTERRUPT_PORTB_INPUT_CHANGE,
#endif
#ifdef _PORTC   
    INTERRUPT_PORTC_INPUT_CHANGE,
#endif
#ifdef _PORTD   
    INTERRUPT_PORTD_INPUT_CHANGE,
#endif
#ifdef _PORTE 
    INTERRUPT_PORTE_INPUT_CHANGE,
#endif
#ifdef _PORTF 
    INTERRUPT_PORTF_INPUT_CHANGE,
#endif
#ifdef _PORTG 
    INTERRUPT_PORTG_INPUT_CHANGE,
#endif
#ifdef _PMP
    INTERRUPT_PARALLEL_MASTER_PORT,
    INTERRUPT_PARALLEL_MASTER_PORT_ERROR,
#endif
#ifdef _SPI2
    INTERRUPT_SPI2_FAULT,
    INTERRUPT_SPI2_RECEIVE_DONE,
    INTERRUPT_SPI2_TRANSMIT_DONE,
#endif
#ifdef _UART2
    INTERRUPT_UART2_FAULT,
    INTERRUPT_UART2_RECEIVE_DONE,
    INTERRUPT_UART2_TRANSFER_DONE,
#endif
#ifdef _I2C2
    INTERRUPT_IIC2_BUS_COLLISION,
    INTERRUPT_IIC2_SLAVE,
    INTERRUPT_IIC2_MASTER,
#endif
#ifdef _UART3
    INTERRUPT_UART3_FAULT,
    INTERRUPT_UART3_RECEIVE_DONE,
    INTERRUPT_UART3_TRANSFER_DONE,
#endif
#ifdef _UART4
    INTERRUPT_UART4_FAULT,
    INTERRUPT_UART4_RECEIVE_DONE,
    INTERRUPT_UART4_TRANSFER_DONE,
#endif
#ifdef _UART5
    INTERRUPT_UART5_FAULT,
    INTERRUPT_UART5_RECEIVE_DONE,
    INTERRUPT_UART5_TRANSFER_DONE,
#endif   
#ifdef _CTMU
    INTERRUPT_CTMU,
#endif
#ifdef _DMAC0
    INTERRUPT_DMA0,
#endif
#ifdef _DMAC1
    INTERRUPT_DMA1,
#endif
#ifdef _DMAC2
    INTERRUPT_DMA2,
#endif
#ifdef _DMAC3
    INTERRUPT_DMA3,
#endif
           
    INTERRUPT_REQUEST_COUNT
};

enum InterruptPriority
{
    INTERRUPT_PRIORITY_DISABLED = 0,
    INTERRUPT_PRIORITY_1,
    INTERRUPT_PRIORITY_2,
    INTERRUPT_PRIORITY_3,
    INTERRUPT_PRIORITY_4,
    INTERRUPT_PRIORITY_5,
    INTERRUPT_PRIORITY_6,
    INTERRUPT_PRIORITY_7,
            
    INTERRUPT_PRIORITY_COUNT
};

enum InterruptSubPriority
{
    INTERRUPT_SUB_PRIORITY_0,
    INTERRUPT_SUB_PRIORITY_1,
    INTERRUPT_SUB_PRIORITY_2,
    INTERRUPT_SUB_PRIORITY_3,
            
    INTERRUPT_SUB_PRIORITY_COUNT
};

/**
 * Enables an interrupt
 * @param intReq The interrupt to enable
 * @param priority The priority of the enabled interrupt
 */
void interrupt_enable(const enum InterruptRequest intReq, const enum InterruptPriority priority);

/**
 * Disables an interrupt
 * @param intReq The interrupt to disable
 */
void interrupt_disable(const enum InterruptRequest intReq);

/**
 * Sets the priority of an interrupt
 * @param intReq The interrupt to be changed from priority
 * @param priority The new priority
 */
void interrupt_set_priority(const enum InterruptRequest intReq, const enum InterruptPriority priority);

/**
 * Sets the sub-priority of an interrupt
 * @param intReq The interrupt to be changed from sub-priority
 * @param priority The new sub-priority
 */
void interrupt_set_sub_priority(const enum InterruptRequest intReq, const enum InterruptSubPriority priority);

/**
 * Get the state of the interrupt flag
 * @param intReq The interrupt to get the state from
 * @return Returns '1' when the interrupt flag is set, otherwise '0'
 */
inline unsigned char __attribute__((always_inline)) interrupt_get_flag(const enum InterruptRequest intReq);

/**
 * Clears the interrupt flag
 * @param intReq The interrupt (flag) to be cleared
 */
inline void __attribute__((always_inline)) interrupt_clr_flag(const enum InterruptRequest intReq);

inline void __attribute__((always_inline)) interrupt_enable_mvec();

inline void __attribute__((always_inline)) interrupt_disable_mvec();

inline void __attribute__((always_inline)) interrupt_global_enable();

inline void __attribute__((always_inline)) interrupt_global_disable();

#endif	/* INTERRUPT_H */

