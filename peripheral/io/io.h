#ifndef IO_H
#define	IO_H

#include "../../lib/utils/bitwise.h"
#include "../../lib/types/register.h"
#include <xc.h>
#include <limits.h>

enum IoPort
{
#ifdef _PORTA
    IO_PORTA,
#endif
#ifdef _PORTB
    IO_PORTB,
#endif
#ifdef _PORTC
    IO_PORTC,
#endif
#ifdef _PORTD
    IO_PORTD,
#endif
#ifdef _PORTE
    IO_PORTE,
#endif
#ifdef _PORTF
    IO_PORTF,
#endif
#ifdef _PORTG
    IO_PORTG,
#endif
    IO_PORT_COUNT
};

enum IoBit{
    IO_BIT0     = BIT_SHIFT(0),
    IO_BIT1     = BIT_SHIFT(1),
    IO_BIT2     = BIT_SHIFT(2),
    IO_BIT3     = BIT_SHIFT(3),
    IO_BIT4     = BIT_SHIFT(4),
    IO_BIT5     = BIT_SHIFT(5),
    IO_BIT6     = BIT_SHIFT(6),
    IO_BIT7     = BIT_SHIFT(7),
    IO_BIT8     = BIT_SHIFT(8),
    IO_BIT9     = BIT_SHIFT(9),
    IO_BIT10    = BIT_SHIFT(10),
    IO_BIT11    = BIT_SHIFT(11),
    IO_BIT12    = BIT_SHIFT(12),
    IO_BIT13    = BIT_SHIFT(13),
    IO_BIT14    = BIT_SHIFT(14),
    IO_BIT15    = BIT_SHIFT(15),
    IO_BIT_ALL  = 0x0000ffff,
            
    IO_BIT_REG_T_FORCE = REG_T_MAX
};

enum IoMode{
    IO_DIGITAL_OUTPUT = 0,
    IO_DIGITAL_INPUT,
    IO_ANALOG_OUTPUT,
    IO_ANALOG_INPUT,
    IO_OPEN_DRAIN
    //@Todo: Implement weak pull-up and pull-down
};

enum IoDirection {
    IO_LOW = 0,
    IO_HIGH,
    IO_INVERT,
    
    IO_DIRECTION_COUNT      
};

/**
 * Configures a single I/O pin or multiple from the same port
 * @param port The port of the I/O pin(s)
 * @param mask A mask that contains the bit number of the I/O pin, bit numbers can be OR'ed to configure multiple pins at the same time
 * @param mode The mode of the I/O pin(s)
 * @return Returns a mask of which pins are actually configured.
 * @note If the input I/O mask doesn't match the return value, it means some pins couldn't be configure to the desired mode
 */
enum IoBit io_configure(const enum IoPort port, const enum IoBit mask, const enum IoMode mode);

/**
 * Writes a single I/O pin or multiple from the same port
 * @param port The port of the I/O pin(s)
 * @param mask A mask that contains the bit number of the I/O pin, bit numbers can be OR'ed to write multiple pins at the same time 
 * @param direction The direction of the write action
 * @note The I/O pin(s) must be configured with mode 'IO_DIGITAL_OUTPUT' 
 */
void io_digital_write(const enum IoPort port, const enum IoBit mask, const enum IoDirection direction);

/**
 * Reads a single I/O pin or multiple from the same port
 * @param port The port of the I/O pin(s)
 * @param mask A mask that contains the bit number of the I/O pin, bit numbers can be OR'ed to read multiple pins at the same time 
 * @return Returns the state of the I/O pin(s)
 * @note The I/O pin(s) must be configured with mode 'IO_DIGITAL_INPUT'
 */
enum IoBit io_digital_read(const enum IoPort port, const enum IoBit mask);

/**
 * Unlocks the peripheral pin select module
 * Note: Unlocking and locking is recursive, meaning that unlocking multiple times have to result in an equal number of lock actions 
 */
void io_unlock_pps();

/**
 * Locks the peripheral pin select module
 * Note: Unlocking and locking is recursive, meaning that unlocking multiple times have to result in an equal number of lock actions 
 */
void io_lock_pps();

#endif	/* IO_H */

