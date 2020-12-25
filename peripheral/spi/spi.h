#ifndef SPI_H
#define SPI_H

// @Todo: Every option or configuration that is commented or left out is currently not supported

#include "cfg/spi_config.h"
#include "../../lib/utils/bitwise.h"
#include "../../lib/std/stdtypes.h"
#include <xc.h>
#include <limits.h>

enum SpiChannel {
#if defined(_SPI1) && !defined(SPI_CHANNEL1_FORCE_DISABLE)
    SPI_CHANNEL1,
#endif
#if defined(_SPI2) && !defined(SPI_CHANNEL2_FORCE_DISABLE)
    SPI_CHANNEL2,
#endif
    SPI_CHANNEL_COUNT
};

struct SpiModule;

enum SpiConfiguration
{
    // SPIxCON
    SPI_CONFIG_CLK_IDLE_HIGH            = BIT_SHIFT(6),
    SPI_CONFIG_CLK_IDLE_LOW             = 0,
    SPI_CONFIG_CLK_EDGE_AI              = BIT_SHIFT(8),
    SPI_CONFIG_CLK_EDGE_IA              = 0,
    //SPI_CONFIG_INPUT_SAMPLE_PHASE_END   = BIT_SHIFT(9),
    //SPI_CONFIG_INPUT_SAMPLE_PHASE_MID   = 0,
    SPI_CONFIG_STOP_IDLE                = BIT_SHIFT(13),
    SPI_CONFIG_CONTINUE_IDLE            = 0,
    SPI_CONFIG_ENHANCED_BUFFER          = BIT_SHIFT(16),
    //SPI_CONFIG_FRAME_SYNC_COINCIDE      = BIT_SHIFT(17),
    //SPI_CONFIG_FRAME_SYNC_PRECEDE       = 0,
    SPI_CONFIG_BAUD_GEN_REFCLK          = BIT_SHIFT(23),
    SPI_CONFIG_BAUD_GEN_PBCLK           = 0,
    //SPI_CONFIG_FRAME_COUNT_PULSE_32     = BIT_SHIFT(26) | BIT_SHIFT(24), // @Todo: We might want to move this to the SPI properties
    //SPI_CONFIG_FRAME_COUNT_PULSE_16     = BIT_SHIFT(26),
    //SPI_CONFIG_FRAME_COUNT_PULSE_8      = BIT_SHIFT(25) | BIT_SHIFT(24),
    //SPI_CONFIG_FRAME_COUNT_PULSE_4      = BIT_SHIFT(25),
    //SPI_CONFIG_FRAME_COUNT_PULSE_2      = BIT_SHIFT(24),
    //SPI_CONFIG_FRAME_COUNT_PULSE_1      = 0,
    //SPI_CONFIG_FRAME_SYNC_WIDTH_CHAR    = BIT_SHIFT(27),
    //SPI_CONFIG_FRAME_SYNC_WIDTH_CLK     = 0,
    //SPI_CONFIG_FRAME_SYNC_ACTIVE_HIGH   = BIT_SHIFT(29),
    //SPI_CONFIG_FRAME_SYNC_ACTIVE_LOW    = 0,
    //SPI_CONFIG_FRAME_SYNC_DIR_INPUT     = BIT_SHIFT(30),
    //SPI_CONFIG_FRAME_SYNC_DIR_OUTPUT    = 0,
    //SPI_CONFIG_FRAME_EN                 = BIT_SHIFT(31),
            
    // SPIxCON2
    //SPI_CONFIG_AUDIO_MODE_PCM_DSP       = (unsigned long long)BIT_SHIFT(1) << 32 | (unsigned long long)BIT_SHIFT(0) << 32, // @Todo: Make separate functions en configs for audio
    //SPI_CONFIG_AUDIO_MODE_R_JUSTIFIED   = (unsigned long long)BIT_SHIFT(1) << 32,
    //SPI_CONFIG_AUDIO_MODE_L_JUSTIFIED   = (unsigned long long)BIT_SHIFT(0) << 32,
    //SPI_CONFIG_AUDIO_MODE_IIS           = 0,
    //SPI_CONFIG_AUDIO_MONO               = (unsigned long long)BIT_SHIFT(3) << 32,
    //SPI_CONFIG_AUDIO_STEREO             = 0,
    //SPI_CONFIG_AUDIO_EN                 = (unsigned long long)BIT_SHIFT(7) << 32,
    //SPI_CONFIG_AUDIO_IGNORE_TX_UNDERRUN = (unsigned long long)BIT_SHIFT(8) << 32,
    //SPI_CONFIG_AUDIO_IGNORE_RX_UNDERRUN = (unsigned long long)BIT_SHIFT(9) << 32,
    //SPI_CONFIG_AUDIO_INT_TX_UNDERRUN_EN = (unsigned long long)BIT_SHIFT(10) << 32,
    //SPI_CONFIG_AUDIO_INT_RX_UNDERRUN_EN = (unsigned long long)BIT_SHIFT(11) << 32,
    //SPI_CONFIG_FRAME_INT_EN             = (unsigned long long)BIT_SHIFT(12) << 32,
    SPI_CONFIG_RX_SIGN_EXT_EN           = (unsigned long long)BIT_SHIFT(15) << 32
};

enum SpiProperties
{
    SPI_PROP_MODE_MASTER        = BIT_SHIFT(5),
    SPI_PROP_MODE_SLAVE         = 0,
    SPI_PROP_MODE_32            = BIT_SHIFT(11),
    SPI_PROP_MODE_16            = BIT_SHIFT(10),
    SPI_PROP_MODE_8             = 0
    //SPI_PROP_AUDIO_MODE_24      = BIT_SHIFT(11) | BIT_SHIFT(10), // @Todo: Make separate functions en configs for audio
    //SPI_PROP_AUDIO_MODE_32      = BIT_SHIFT(11),
    //SPI_PROP_AUDIO_MODE_16_2    = BIT_SHIFT(10),
    //SPI_PROP_AUDIO_MODE_16      = 0,
            
};

enum SpiEnable
{
    SPI_ENABLE_SS = 0,
    SPI_ENABLE_SDI,
    SPI_ENABLE_SDO
};

enum SpiError
{
    SPI_ERROR_OK        = 0,
    SPI_ERROR_FRAME     = BIT_SHIFT(0),
    SPI_ERROR_UNDERRUN  = BIT_SHIFT(1),
    SPI_ERROR_OVERRUN   = BIT_SHIFT(2),
            
    SPI_ERROR_UNKNOWN   = BIT_SHIFT(7)
};

/**
 * Initializes the SPI library
 * @return Returns 'true' on success, otherwise 'false'
 * @note This function will invalidate the SPI pool, meaning no module should be created before this function is called
 */
bool spi_init();
/**
 * Claim a SPI channel and initialize
 * @param channel The channel to be used
 * @param rxBuffer A uint array that will be used as RX FIFO queue
 * @param txBuffer A uint array that will be used as TX FIFO queue
 * @param rxSize The size of the RX uint array
 * @param txSize The size of the TX uint array
 * @return Returns a pointer to the created SPI module
 */
struct SpiModule* spi_create(const enum SpiChannel channel, unsigned int* rxBuffer, unsigned int* txBuffer, const unsigned int rxSize, const unsigned int txSize);

/**
 * Invalidates a SPI module and returns it to the SPI pool
 * @param module The module to be invalidated
 */
void spi_invalidate(struct SpiModule* module);

/**
 * Configures the SPI module
 * @param module The module to be configured
 * @param mask The configuration mask
 */
void spi_configure(const struct SpiModule* module, const enum SpiConfiguration mask);

/**
 * Configures the hardware properties of the SPI module
 * @param module The module to be configured
 * @param mask The hardware properties mask
 */
void spi_set_properties(const struct SpiModule* module, const enum SpiProperties mask);

/**
 * Configures the baudrate of the SPI module
 * @param module The module to be configured
 * @param clock The peripheral bus or reference clock frequency
 * @param baudrate The desired baudrate
 * @returns Returns the actual floor-rounded baudrate
 * @note The configured baudrate will only be used when the SPI module is configured as a master.
 */
unsigned int spi_set_baudrate(const struct SpiModule* module, const unsigned long clock, const unsigned int baudrate);

/**
 * Enables the SPI module
 * @param module The module to be enabled
 * @param mask The hardware enable mask
 */
void spi_enable(struct SpiModule* module, const enum SpiEnable mask);

/**
 * Disable the SPI module
 * @param module The module to be disabled
 */
void spi_disable(const struct SpiModule* module);

/**
 * Gets the last error from the SPI module
 * @param module The module to get the error from
 * @return Returns a mask of errors which can be AND'ed against the fields from SpiError enum
 */
unsigned char spi_error(const struct SpiModule* module);

/**
 * Disables and re-enables the module
 * @param module The module to be resetted
 * @note This will flush both the RX and TX queues, hardware FIFO's and clears all errors
 */
void spi_reset(struct SpiModule* module);

/**
 * Transmits a buffer via the SPI module
 * @param module The module to use for the transmission
 * @param buffer The buffer to be sent
 * @param size The size of the buffer in bytes
 * @return Returns the actual number of bytes that were scheduled for transmission
 */
unsigned int spi_transmit(const struct SpiModule* module, const unsigned int* buffer, const unsigned int size);

/**
 * Fill the buffer with bytes read from the SPI module
 * @param module The module to read data from
 * @param buffer The buffer the data will be placed in
 * @param size The number of bytes to read
 * @return Returns the actual number of bytes that were read
 */
unsigned int spi_receive(const struct SpiModule* module, unsigned int* buffer, const unsigned int size);

#endif /* SPI_H */