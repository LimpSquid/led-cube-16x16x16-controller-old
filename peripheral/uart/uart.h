#ifndef UART_H
#define	UART_H

#include "cfg/uart_config.h"
#include "../../lib/utils/bitwise.h"
#include "../../lib/types/register.h"
#include "../../lib/std/stdtypes.h"
#include <xc.h>
#include <limits.h>

enum UartChannel {
#if defined(_UART1) && !defined(UART_CHANNEL1_FORCE_DISABLE)
    UART_CHANNEL1,
#endif
#if defined(_UART2) && !defined(UART_CHANNEL2_FORCE_DISABLE)
    UART_CHANNEL2,
#endif
#if defined(_UART3) && !defined(UART_CHANNEL3_FORCE_DISABLE)
    UART_CHANNEL3,
#endif
#if defined(_UART4) && !defined(UART_CHANNEL4_FORCE_DISABLE)
    UART_CHANNEL4,
#endif
#if defined(_UART5) && !defined(UART_CHANNEL5_FORCE_DISABLE)
    UART_CHANNEL5,
#endif
#if defined(_UART6) && !defined(UART_CHANNEL6_FORCE_DISABLE)
    UART_CHANNEL6,
#endif
    UART_CHANNEL_COUNT
};

struct UartModule;

union UartData
{
    // @Todo: endianness
    unsigned short _reg;
    struct {
        unsigned char data :8;
        unsigned char bit9 :1;
    };
};

enum UartConfiguration
{
    // UxMODE
    UART_CONFIG_HIGH_SPEED          = BIT_SHIFT(3),
    UART_CONFIG_RX_INV_IDLE         = BIT_SHIFT(4),
    UART_CONFIG_LOOPBACK_EN         = BIT_SHIFT(6),
    UART_CONFIG_WAKE_UP_EN          = BIT_SHIFT(7),
    UART_CONFIG_TX_RX_EN            = 0,
    UART_CONFIG_TX_RX_RTS_EN        = BIT_SHIFT(8),
    UART_CONFIG_TX_RX_RTS_CTS_EN    = BIT_SHIFT(9),
    UART_CONFIG_TX_RX_BCLK_EN       = BIT_SHIFT(8) | BIT_SHIFT(9),
    UART_CONFIG_RTS_FLOW            = 0,
    UART_CONFIG_RTS_SIMPLEX         = BIT_SHIFT(11),
    UART_CONFIG_IRDA_EN             = BIT_SHIFT(12),
    UART_CONFIG_STOP_IDLE_EN        = BIT_SHIFT(13),
      
    // UxSTA
    UART_CONFIG_ADDRESS_DETECT_EN   = (unsigned long long)BIT_SHIFT(5) << 32,
    UART_CONFIG_TX_INV_IDLE         = (unsigned long long)BIT_SHIFT(13) << 32,
    UART_CONFIG_AUTO_ADDRESS_EN     = (unsigned long long)BIT_SHIFT(24) << 32
};

enum UartProperties
{
    UART_PROP_STOP_BITS_1   = 0,
    UART_PROP_STOP_BITS_2   = BIT_SHIFT(0),
    UART_PROP_DATA_BITS_8   = 0,
    UART_PROP_DATA_BITS_9   = BIT_SHIFT(1) | BIT_SHIFT(2),
    UART_PROP_PARITY_ODD    = BIT_SHIFT(1),
    UART_PROP_PARITY_EVEN   = BIT_SHIFT(2)
};

enum UartEnable
{
    UART_ENABLE_RX = 0,
    UART_ENABLE_TX
};

enum UartError
{
    UART_ERROR_OK = 0,
    UART_ERROR_OVERRUN  = BIT_SHIFT(0),
    UART_ERROR_FRAMING  = BIT_SHIFT(1),
    UART_ERROR_PARITY   = BIT_SHIFT(2),
            
    UART_ERROR_UNKNOWN  = BIT_SHIFT(7)
};

/**
 * Initializes UART library
 * @return Returns 'true' on success, otherwise 'false'
 * @note This function will invalidate the UART pool, meaning no module should be created before this function is called
 */
bool uart_init();

/**
 * Claim an uart channel and initialize
 * @param channel The channel to be used
 * @param rxBuffer An UartData array that will be used as RX FIFO queue
 * @param txBuffer An UartData array that will be uses as TX FIFO queue
 * @param rxSize The size of the RX byte array
 * @param txSize The size of the TX byte array
 * @return Returns a pointer to the created UART module
 */
struct UartModule* uart_create(const enum UartChannel channel, union UartData* rxBuffer, union UartData* txBuffer, const unsigned int rxSize, const unsigned int txSize);

/**
 * Invalidates an UART module and returns it to the UART pool
 * @param module The module to be invalidated
 */
void uart_invalidate(struct UartModule* module);

/**
 * Configures the UART module
 * @param module The module to be configured
 * @param mask The configuration mask
 */
void uart_configure(const struct UartModule* module, const enum UartConfiguration mask);

/**
 * Configures the hardware properties of the UART module
 * @param module The module to be configured
 * @param mask The hardware properties mask
 */
void uart_set_properties(const struct UartModule* module, const enum UartProperties mask);

/**
 * Configures the baudrate of the UART module
 * @param module The module to be configured
 * @param clock The peripheral bus clock frequency
 * @param baudrate The desired baudrate
 * @returns Returns the actual floor-rounded baudrate
 */
unsigned int uart_set_baudrate(const struct UartModule* module, const unsigned long clock, const unsigned int baudrate);

/**
 * Configures the automatic address mask
 * @param module The module to be configured
 * @param address The address mask
 * @note The UART module must be configured with the 'UART_CONFIG_AUTO_ADDRESS_EN' flag.
 */
void uart_set_auto_address(const struct UartModule* module, unsigned char address);

/**
 * Start the automatic baud detection sequence
 * @param module The module to start the sequence on
 */
void uart_start_auto_baud(const struct UartModule* module);

/**
 * Checks if the auto-baud sequence is completed or disabled
 * @param module The module to be checked
 * @return Returns a '1' when the auto-baud sequence is completed or disabled, otherwise '0'
 */
unsigned char uart_auto_baud_complete(const struct UartModule* module);

/**
 * Enables the UART module
 * @param module The module to be enabled
 * @param mask The hardware enable mask
 */
void uart_enable(struct UartModule* module, const enum UartEnable mask);

/**
 * Disable the UART module
 * @param module The module to be disabled
 */
void uart_disable(const struct UartModule* module);

/**
 * Gets the last error from the UART module
 * @param module The module to get the error from
 * @return Returns a mask of errors which can be AND'ed against the fields from UartError enum
 */
enum UartError uart_error(const struct UartModule* module);

/**
 * Disables and re-enables the module
 * @param module The module to be resetted
 * @note This will flush both the RX and TX queues, hardware FIFO's and clears all errors
 */
void uart_reset(struct UartModule* module);

/**
 * Transmits UART data packets via the UART module
 * @param module The module to use for the transmission
 * @param data The UART data to be sent
 * @param length The number of UART data packets to write
 * @return Returns the actual number of UART data packets that were scheduled for transmission
 * @warning Can only be used in 8 bit mode
 */
unsigned int uart_transmit(const struct UartModule* module, const union UartData* data, const unsigned int length);

/**
 * Fill the UART data with packets read from the UART module
 * @param module The module to read data from
 * @param data The data the UART data packets will be placed in
 * @param length The number of UART data packets to read
 * @return Returns the actual number of UART data packets that were read
 * @warning Can only be used in 8 bit mode
 */
unsigned int uart_receive(const struct UartModule* module, union UartData* data, const unsigned int length);

/**
 * Transmits a buffer via the UART module
 * @param module The module to use for the transmission
 * @param buffer The buffer to be sent
 * @param size The size of the buffer in bytes
 * @return Returns the actual number of bytes that were scheduled for transmission
 * @warning Can only be used in 8 bit mode
 */
unsigned int uart_transmit_raw(const struct UartModule* module, const void* buffer, const unsigned int size);

/**
 * Fill the buffer with bytes read from the UART module
 * @param module The module to read data from
 * @param buffer The buffer the data will be placed in
 * @param size The number of bytes to read
 * @return Returns the actual number of bytes that were read
 * @warning Can only be used in 8 bit mode
 */
unsigned int uart_receive_raw(const struct UartModule* module, unsigned char* buffer, const unsigned int size);

/**
 * Checks if there is atleast one byte in the RX FIFO
 * @param module The module to be checked
 * @return Returns an '1' on success, otherwise '0'.
 */
inline unsigned char uart_rx_available(const struct UartModule* module);

/**
 * Checks if there is atleast one byte of space in the TX FIFO
 * @param module The module to be checked
 * @return Returns an '1' on success, otherwise '0'
 */
inline unsigned char uart_tx_available(const struct UartModule* module);

#endif	/* UART_H */