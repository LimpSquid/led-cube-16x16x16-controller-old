#include "spi_map.h"
#include "../cfg/spi_config.h"
#include <xc.h>
#include <stddef.h>

#if defined(__PIC32MX__)
    #if (__PIC32_FEATURE_SET__ == 330)  ||  \
        (__PIC32_FEATURE_SET__ == 350)  ||  \
        (__PIC32_FEATURE_SET__ == 370)  ||  \
        (__PIC32_FEATURE_SET__ == 430)  ||  \
        (__PIC32_FEATURE_SET__ == 450)  ||  \
        (__PIC32_FEATURE_SET__ == 470)

        const struct SpiMap spiMappingTable[] =
        {
        #if defined(_SPI1) && !defined(SPI_CHANNEL1_FORCE_DISABLE)
            { (struct SpiSfr*)&SPI1CON },
        #endif
        #if defined(_SPI2) && !defined(SPI_CHANNEL2_FORCE_DISABLE)
            { (struct SpiSfr*)&SPI2CON },
        #endif

            { NULL } // Terminator
        };
    #else
        #error "The device's feature set is not supported by the SPI library."
    #endif
#else
    #error "Device family is not supported by the SPI library."
#endif
