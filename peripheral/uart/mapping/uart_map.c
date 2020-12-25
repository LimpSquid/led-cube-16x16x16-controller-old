#include "uart_map.h"
#include "../cfg/uart_config.h"
#include <xc.h>
#include <stddef.h>

#if defined(__PIC32MX__)
    #if (__PIC32_FEATURE_SET__ == 330)  ||  \
        (__PIC32_FEATURE_SET__ == 350)  ||  \
        (__PIC32_FEATURE_SET__ == 370)  ||  \
        (__PIC32_FEATURE_SET__ == 430)  ||  \
        (__PIC32_FEATURE_SET__ == 450)  ||  \
        (__PIC32_FEATURE_SET__ == 470)

        const struct UartMap uartMappingTable[] =
        {
        #if defined(_UART1) && !defined(UART_CHANNEL1_FORCE_DISABLE)
            { (struct UartSfr*)&U1MODE },
        #endif
        #if defined(_UART2) && !defined(UART_CHANNEL2_FORCE_DISABLE)
            { (struct UartSfr*)&U2MODE },
        #endif
        #if defined(_UART3) && !defined(UART_CHANNEL3_FORCE_DISABLE)
            { (struct UartSfr*)&U3MODE },
        #endif
        #if defined(_UART4) && !defined(UART_CHANNEL4_FORCE_DISABLE)
            { (struct UartSfr*)&U4MODE },
        #endif
        #if defined(_UART5) && !defined(UART_CHANNEL5_FORCE_DISABLE)
            { (struct UartSfr*)&U5MODE },
        #endif
        #if defined(_UART6) && !defined(UART_CHANNEL6_FORCE_DISABLE)
            { (struct UartSfr*)&U6MODE },
        #endif

            { NULL } // Terminator
        };
    #else
        #error "The device's feature set is not supported by the UART library."
    #endif
#else
    #error "Device family is not supported by the UART library."
#endif