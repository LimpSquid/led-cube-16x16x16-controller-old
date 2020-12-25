#include "interrupt_map.h"
#include <xc.h>

#if defined(__PIC32MX__)
    #if (__PIC32_FEATURE_SET__ == 330)  ||  \
        (__PIC32_FEATURE_SET__ == 350)  ||  \
        (__PIC32_FEATURE_SET__ == 370)  ||  \
        (__PIC32_FEATURE_SET__ == 430)  ||  \
        (__PIC32_FEATURE_SET__ == 450)  ||  \
        (__PIC32_FEATURE_SET__ == 470)

        const struct InterruptMap interruptMappingTable[] =
        {
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC0, (volatile regptr_t)&IPC0, 0x00000001, 0x00000001, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC0, (volatile regptr_t)&IPC0, 0x00000002, 0x00000002, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC0, (volatile regptr_t)&IPC0, 0x00000004, 0x00000004, 0x001c0000, 0x00030000 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC0, (volatile regptr_t)&IPC0, 0x00000008, 0x00000008, 0x1c000000, 0x03000000 },
        #ifdef _TMR1
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC1, (volatile regptr_t)&IPC1, 0x00000010, 0x00000010, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _ICAP1
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC1, (volatile regptr_t)&IPC1, 0x00000020, 0x00000020, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC1, (volatile regptr_t)&IPC1, 0x00000040, 0x00000040, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _OCMP1
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC1, (volatile regptr_t)&IPC1, 0x00000080, 0x00000080, 0x001c0000, 0x00030000 },
        #endif
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC1, (volatile regptr_t)&IPC1, 0x00000100, 0x00000100, 0x1c000000, 0x03000000 },
        #ifdef _TMR2
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC2, (volatile regptr_t)&IPC2, 0x00000200, 0x00000200, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _ICAP2
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC2, (volatile regptr_t)&IPC2, 0x00000400, 0x00000400, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC2, (volatile regptr_t)&IPC2, 0x00000800, 0x00000800, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _OCMP2
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC2, (volatile regptr_t)&IPC2, 0x00001000, 0x00001000, 0x001c0000, 0x00030000 },
        #endif
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC2, (volatile regptr_t)&IPC2, 0x00002000, 0x00002000, 0x1c000000, 0x03000000 },
        #ifdef _TMR3
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC3, (volatile regptr_t)&IPC3, 0x00004000, 0x00004000, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _ICAP3
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC3, (volatile regptr_t)&IPC3, 0x00008000, 0x00008000, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC3, (volatile regptr_t)&IPC3, 0x00010000, 0x00010000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _OCMP3
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC3, (volatile regptr_t)&IPC3, 0x00020000, 0x00020000, 0x001c0000, 0x00030000 },
        #endif
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC3, (volatile regptr_t)&IPC3, 0x00040000, 0x00040000, 0x1c000000, 0x03000000 },
        #ifdef _TMR4
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC4, (volatile regptr_t)&IPC4, 0x00080000, 0x00080000, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _ICAP4
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC4, (volatile regptr_t)&IPC4, 0x00100000, 0x00100000, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC4, (volatile regptr_t)&IPC4, 0x00200000, 0x00200000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _OCMP4
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC4, (volatile regptr_t)&IPC4, 0x00400000, 0x00400000, 0x001c0000, 0x00030000 },
        #endif
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC4, (volatile regptr_t)&IPC4, 0x00800000, 0x00800000, 0x1c000000, 0x03000000 },
        #ifdef _TMR5
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC5, (volatile regptr_t)&IPC5, 0x01000000, 0x01000000, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _ICAP5
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC5, (volatile regptr_t)&IPC5, 0x02000000, 0x02000000, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC5, (volatile regptr_t)&IPC5, 0x04000000, 0x04000000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _OCMP5
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC5, (volatile regptr_t)&IPC5, 0x08000000, 0x08000000, 0x001c0000, 0x00030000 },
        #endif
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC5, (volatile regptr_t)&IPC5, 0x10000000, 0x10000000, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC6, (volatile regptr_t)&IPC6, 0x20000000, 0x20000000, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC6, (volatile regptr_t)&IPC6, 0x40000000, 0x40000000, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS0, (volatile regptr_t)&IEC0, (volatile regptr_t)&IPC6, (volatile regptr_t)&IPC6, 0x80000000, 0x80000000, 0x001c0000, 0x00030000 },
        #ifdef _CMP1
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC6, (volatile regptr_t)&IPC6, 0x00000001, 0x00000001, 0x1c000000, 0x03000000 },
        #endif
        #ifdef _CMP2
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000002, 0x00000002, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _USB
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000004, 0x00000004, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _SPI1
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000008, 0x00000008, 0x001c0000, 0x00030000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000010, 0x00000010, 0x001c0000, 0x00030000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000020, 0x00000020, 0x001c0000, 0x00030000 },
        #endif
        #ifdef _UART1
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000040, 0x00000040, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000080, 0x00000080, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC7, (volatile regptr_t)&IPC7, 0x00000100, 0x00000100, 0x1c000000, 0x03000000 },
        #endif
        #ifdef _I2C1
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00000200, 0x00000200, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00000400, 0x00000400, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00000800, 0x00000800, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _PORTA
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00001000, 0x00001000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PORTB
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00002000, 0x00002000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PORTC
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00004000, 0x00004000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PORTD
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00008000, 0x00008000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PORTE
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00010000, 0x00010000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PORTF
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00020000, 0x00020000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PORTG
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00040000, 0x00040000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _PMP
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00080000, 0x00080000, 0x001c0000, 0x00030000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00100000, 0x00100000, 0x001c0000, 0x00030000 },
        #endif
        #ifdef _SPI2
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00200000, 0x00200000, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00400000, 0x00400000, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC8, (volatile regptr_t)&IPC8, 0x00800000, 0x00800000, 0x1c000000, 0x03000000 },
        #endif
        #ifdef _UART2
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x01000000, 0x01000000, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x02000000, 0x02000000, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x04000000, 0x04000000, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _I2C2
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x08000000, 0x08000000, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x10000000, 0x10000000, 0x00001c00, 0x00000300 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x20000000, 0x20000000, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _UART3
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x40000000, 0x40000000, 0x001c0000, 0x00030000 },
            { (volatile regptr_t)&IFS1, (volatile regptr_t)&IEC1, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x80000000, 0x80000000, 0x001c0000, 0x00030000 },
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x00000001, 0x00000001, 0x001c0000, 0x00030000 },
        #endif
        #ifdef _UART4
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x00000002, 0x00000002, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x00000004, 0x00000004, 0x1c000000, 0x03000000 },
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC9, (volatile regptr_t)&IPC9, 0x00000008, 0x00000008, 0x1c000000, 0x03000000 },
        #endif
        #ifdef _UART5
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC10, (volatile regptr_t)&IPC10, 0x00000010, 0x00000010, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC10, (volatile regptr_t)&IPC10, 0x00000020, 0x00000020, 0x0000001c, 0x00000003 },
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC10, (volatile regptr_t)&IPC10, 0x00000040, 0x00000040, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _CTMU
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC10, (volatile regptr_t)&IPC10, 0x00000080, 0x00000080, 0x00001c00, 0x00000300 },
        #endif
        #ifdef _DMAC0
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC10, (volatile regptr_t)&IPC10, 0x00000100, 0x00000100, 0x001c0000, 0x00030000 },
        #endif
        #ifdef _DMAC1
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC10, (volatile regptr_t)&IPC10, 0x00000200, 0x00000200, 0x1c000000, 0x03000000 },
        #endif
        #ifdef _DMAC2
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC11, (volatile regptr_t)&IPC11, 0x00000400, 0x00000400, 0x0000001c, 0x00000003 },
        #endif
        #ifdef _DMAC3
            { (volatile regptr_t)&IFS2, (volatile regptr_t)&IEC2, (volatile regptr_t)&IPC11, (volatile regptr_t)&IPC11, 0x00000800, 0x00000800, 0x00001c00, 0x00000300 },
        #endif

            { NULL, NULL, NULL, NULL, 0x00000000, 0x00000000, 0x00000000, 0x00000000} // Terminator
        };
    #else
        #error "The device's feature set is not supported by the INTERRUPT library."
    #endif
#else
    #error "Device family is not supported by the INTERRUPT library."
#endif
