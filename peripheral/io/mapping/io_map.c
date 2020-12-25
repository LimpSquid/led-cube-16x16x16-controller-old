#include "io_map.h"
#include <xc.h>
#include <stddef.h>

#if defined(__PIC32MX__)
    #if (__PIC32_FEATURE_SET__ == 330)  ||  \
        (__PIC32_FEATURE_SET__ == 350)  ||  \
        (__PIC32_FEATURE_SET__ == 370)  ||  \
        (__PIC32_FEATURE_SET__ == 430)  ||  \
        (__PIC32_FEATURE_SET__ == 450)  ||  \
        (__PIC32_FEATURE_SET__ == 470)

        // Device filter table
        #define DEVICE_MASK_1   defined(__32MX330F064L__) || defined(__32MX350F128L__) || defined(__32MX350F256L__) || \
                                defined(__32MX370F512L__) || defined(__32MX430F064L__) || defined(__32MX450F128L__) || \
                                defined(__32MX450F256L__) || defined(__32MX470F512L__)
        #define DEVICE_MASK_2   defined(__32MX330F064H__) || defined(__32MX350F128H__) || defined(__32MX350F256H__) || \
                                defined(__32MX370F512H__) || defined(__32MX430F064H__) || defined(__32MX450F128H__) || \
                                defined(__32MX450F256H__) || defined(__32MX470F512H__)  
        #define DEVICE_MASK_3   defined(__32MX330F064L__) || defined(__32MX350F128L__) || defined(__32MX350F256L__) || \
                                defined(__32MX370F512L__)
        #define DEVICE_MASK_4   defined(__32MX430F064L__) || defined(__32MX450F128L__) || defined(__32MX450F256L__) || \
                                defined(__32MX470F512L__)
        #define DEVICE_MASK_5   defined(__32MX330F064H__) || defined(__32MX350F128H__) || defined(__32MX350F256H__) || \
                                defined(__32MX370F512H__)
        #define DEVICE_MASK_6   defined(__32MX430F064H__) || defined(__32MX450F128H__) || defined(__32MX450F256H__) || \
                                defined(__32MX470F512H__)

        const struct PortMap portMappingTable[] =
        {
        #ifdef _PORTA
            { (struct PortSfr*)&TRISA, atomic_regptr_cast(ANSELA), 0x0000c6ff, 0x00000600 },
        #endif
        #ifdef _PORTB
            { (struct PortSfr*)&TRISB, atomic_regptr_cast(ANSELB), 0x0000ffff, 0x0000ffff },
        #endif
        #if defined(_PORTC)
            #if DEVICE_MASK_1
                { (struct PortSfr*)&TRISC, NULL, 0x0000f01e, 0x00000000 },
            #elif DEVICE_MASK_2
                { (struct PortSfr*)&TRISC, NULL, 0x0000f000, 0x00000000 },
            #else
                #warning "Unknown device, PORTC is not supported by the IO library."
                { NULL, NULL, 0x00000000, 0x00000000 },
            #endif
        #endif
        #if defined(_PORTD)
            #if DEVICE_MASK_1
                { (struct PortSfr*)&TRISD, atomic_regptr_cast(ANSELD), 0x0000ffff, 0x0000000e },
            #elif DEVICE_MASK_2 
                { (struct PortSfr*)&TRISD, atomic_regptr_cast(ANSELD), 0x00000fff, 0x0000000e },
            #else
                #warning "Unknown device, PORTD is not supported by the IO library."
                { NULL, NULL, 0x00000000, 0x00000000 },
            #endif
        #endif
        #if defined(_PORTE)
            #if DEVICE_MASK_1
                { (struct PortSfr*)&TRISE, atomic_regptr_cast(ANSELE), 0x000003ff, 0x000000f4 },
            #elif DEVICE_MASK_2
                { (struct PortSfr*)&TRISE, atomic_regptr_cast(ANSELE), 0x000000ff, 0x000000f4 },
            #else
                #warning "Unknown device, PORTE is not supported by the IO library."
                { NULL, NULL, 0x00000000, 0x00000000 },
            #endif    
        #endif
        #if defined(_PORTF)
            #if DEVICE_MASK_3
                { (struct PortSfr*)&TRISF, NULL, 0x000031ff, 0x00000000 },
            #elif DEVICE_MASK_4
                { (struct PortSfr*)&TRISF, NULL, 0x0000313f, 0x00000000 },
            #elif DEVICE_MASK_5
                { (struct PortSfr*)&TRISF, NULL, 0x0000007f, 0x00000000 },
            #elif DEVICE_MASK_6
                { (struct PortSfr*)&TRISF, NULL, 0x0000003b, 0x00000000 },
            #else
                #warning "Unknown device, PORTF is not supported by the IO library."
                { NULL, NULL, 0x00000000, 0x00000000 },
            #endif
        #endif
        #if defined(_PORTG)
            #if DEVICE_MASK_1
                { (struct PortSfr*)&TRISG, atomic_regptr_cast(ANSELG), 0x0000f3cf, 0x000003c0 },
            #elif DEVICE_MASK_2
                { (struct PortSfr*)&TRISG, atomic_regptr_cast(ANSELG), 0x000003cc, 0x000003c0 },
            #else
                #warning "Unknown device, PORTG is not supported by the IO library."
                { NULL, NULL, 0x00000000, 0x00000000 },
            #endif
        #endif

                { NULL, NULL, 0x00000000, 0x00000000 } // Terminator
        };

        #undef DEVICE_MASK_1
        #undef DEVICE_MASK_2
        #undef DEVICE_MASK_3
        #undef DEVICE_MASK_4
        #undef DEVICE_MASK_5
        #undef DEVICE_MASK_6
    #else
        #error "The device's feature set is not supported by the IO library."
    #endif
#else
    #error "Device family is not supported by the IO library."
#endif

