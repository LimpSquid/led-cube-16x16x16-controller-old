#include "uart.h"
#include "mapping/uart_map.h"
#include "../interrupt/interrupt.h"
#include "../../lib/types/queue.h"
#include "../../lib/print/assert.h"

#define UART_RX_EN_BIT          BIT_SHIFT(12)
#define UART_TX_EN_BIT          BIT_SHIFT(10)
#define UART_MODULE_EN_BIT      BIT_SHIFT(15)
#define UART_AUTO_BAUD_MASK     BIT_SHIFT(5)

#define uartAutoAddressMask(x)  ((reg_t)x << 16)

struct UartModule
{
    struct Queue* rxFifo;
    struct Queue* txFifo;
    enum UartChannel channel;
    enum UartError error;
    struct {
        unsigned char assigned :1;
    } opt;
};

enum InterruptEnable 
{
    UART_INTERRUPT_FAULT            = BIT_SHIFT(0),
    UART_INTERRUPT_RECEIVE_DONE     = BIT_SHIFT(1),
    UART_INTERRUPT_TRANSFER_DONE    = BIT_SHIFT(2),
    
    UART_INTERRUPT_ALL              = 0xFF
};

enum InterruptMode
{
    UART_INT_MODE_RX_NOT_EMPTY      = 0,
    UART_INT_MODE_RX_HALF           = BIT_SHIFT(6),
    UART_INT_MODE_RX_THREE_QUARTER  = BIT_SHIFT(7),
    UART_INT_MODE_TX_HAS_SPACE      = 0,
    UART_INT_MODE_TX_TRANSMITTED    = BIT_SHIFT(14),
    UART_INT_MODE_TX_EMPTY          = BIT_SHIFT(15),
            
    UART_INTERRUPT_REG_T_FORCE      = REG_T_MAX
};

enum UartStatus
{
    UART_STATUS_DATA_AVAILABLE      = BIT_SHIFT(0),
    UART_STATUS_OVERRUN_ERROR       = BIT_SHIFT(1),
    UART_STATUS_FRAMING_ERROR       = BIT_SHIFT(2),
    UART_STATUS_PARITY_ERROR        = BIT_SHIFT(3),
    UART_STATUS_RECEIVER_IDLE       = BIT_SHIFT(4),
    UART_STATUS_TRANSMIT_REG_EMPTY  = BIT_SHIFT(8),
    UART_STATUS_TRANSMIT_BUF_FULL   = BIT_SHIFT(9),
            
    UART_STATUS_REG_T_FORCE         = REG_T_MAX
};
             
static void uart_set_interrupt_mode(const struct UartModule* module, const enum InterruptMode mask);
static void uart_enable_interrupt(const enum UartChannel channel, enum InterruptEnable mask);
static void uart_disable_interrupt(const enum UartChannel channel, enum InterruptEnable mask);
static void uart_clr_interrupt_flag(const enum UartChannel channel, enum InterruptEnable mask);

static const enum InterruptRequest uartInterruptTable[] =
{
#if defined(_UART1) && !defined(UART_CHANNEL1_FORCE_DISABLE)
    INTERRUPT_UART1_FAULT,
#endif
#if defined(_UART2) && !defined(UART_CHANNEL2_FORCE_DISABLE)
    INTERRUPT_UART2_FAULT,
#endif
#if defined(_UART3) && !defined(UART_CHANNEL3_FORCE_DISABLE)
    INTERRUPT_UART3_FAULT,
#endif
#if defined(_UART4) && !defined(UART_CHANNEL4_FORCE_DISABLE)
    INTERRUPT_UART4_FAULT,
#endif
#if defined(_UART5) && !defined(UART_CHANNEL5_FORCE_DISABLE)
    INTERRUPT_UART5_FAULT,
#endif
#if defined(_UART6) && !defined(UART_CHANNEL6_FORCE_DISABLE)
    INTERRUPT_UART6_FAULT,
#endif
};

static struct UartModule uartModulePool[UART_CHANNEL_COUNT];
static const size_t nUartModules = UART_CHANNEL_COUNT;

bool uart_init()
{
    // Invalidate all uart modules
    size_t i;
    for(i = 0; i < nUartModules; ++i) {
        uart_disable_interrupt(i, UART_INTERRUPT_ALL);
        uartModulePool[i].opt.assigned = 0;
    }
    return true;
}

struct UartModule* uart_create(const enum UartChannel channel, union UartData* rxBuffer, union UartData* txBuffer, const unsigned int rxSize, const unsigned int txSize)
{
    struct UartModule* module = NULL;
    if(channel >= UART_CHANNEL_COUNT || rxBuffer == NULL || txBuffer == NULL || rxSize == 0 || txSize == 0)      
        return module;
    
    module = &uartModulePool[channel];
    if(!module->opt.assigned) { // Unused module was found
        module->rxFifo = queue_create(rxBuffer, rxSize, QUEUE_FIFO, QUEUE_UART_DATA);
        module->txFifo = queue_create(txBuffer, txSize, QUEUE_FIFO, QUEUE_UART_DATA);
        module->channel = channel;
        module->error = UART_ERROR_OK;
        module->opt.assigned = 1;
    } else
        module = NULL;
    return module;
}

void uart_invalidate(struct UartModule* module)
{
    if(module == NULL)
        return;

    if(module->opt.assigned) {
        uart_disable(module);
        queue_invalidate(module->rxFifo);
        queue_invalidate(module->txFifo);
        module->opt.assigned = 0;
    }
}

void uart_configure(const struct UartModule* module, const enum UartConfiguration mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            // Reset both UxMODE and UxSTA registers
            uartSfr->umode.clr = REG_T_MAX & 0x0000ffff;
            uartSfr->usta.clr = REG_T_MAX & 0x0000ffff;

            uartSfr->umode.set = mask & UINT_MAX;
            uartSfr->usta.set = (mask >> 32) & UINT_MAX;
        }
    }
}

void uart_set_properties(const struct UartModule* module, const enum UartProperties mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            // Reset all the properties of UxMODE
            uartSfr->umode.clr = UART_PROP_STOP_BITS_2 | UART_PROP_DATA_BITS_9;
            uartSfr->umode.set = mask;
        }
    }
}

unsigned int uart_set_baudrate(const struct UartModule* module, const unsigned long clock, const unsigned int baudrate)
{
    unsigned int result = 0;
    if(module == NULL)
        return result;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            reg_t brg = 0;
            uartSfr->ubrg.clr = REG_T_MAX & 0x0000ffff;
            if(uartSfr->umode.reg & UART_CONFIG_HIGH_SPEED) {
                brg = (clock >> 2) / baudrate;
                if(brg > 0xffff)
                    brg = 0xffff;
                result = (clock >> 2) / brg;
            } else {
                brg = (clock >> 4) / baudrate;
                if(brg > 0xffff)
                    brg = 0xffff;
                result = (clock >> 4) / brg;
            }
            brg = brg ? (brg - 1) : 0;
            uartSfr->ubrg.set = brg;
        }
    }
    return result;
}

void uart_set_auto_address(const struct UartModule* module, unsigned char address)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            uartSfr->usta.clr = uartAutoAddressMask(0xff);
            uartSfr->usta.set = uartAutoAddressMask(address);
        }
    }
}
void uart_start_auto_baud(const struct UartModule* module)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL)
            uartSfr->umode.set = UART_AUTO_BAUD_MASK;
    }
}

unsigned char uart_auto_baud_complete(const struct UartModule* module)
{
    unsigned char result = 1;
    if(module == NULL)
        return result;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL)
            result = !uartSfr->umode.reg & UART_AUTO_BAUD_MASK;
    }
    return result;
}

void uart_enable(struct UartModule* module, const enum UartEnable mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            // Disable interrupts
            uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
            
            // Disable UART
            uartSfr->umode.clr = UART_MODULE_EN_BIT;
            uartSfr->usta.clr = UART_RX_EN_BIT | UART_TX_EN_BIT;
            
            // Error recovery
            if(module->error) {
                uartSfr->usta.clr = UART_STATUS_OVERRUN_ERROR;
                module->error = UART_ERROR_OK;
            }
            
            // Flush queues
            queue_flush(module->rxFifo);
            queue_flush(module->txFifo);
            
            // Enable UART and interrupts
            enum InterruptMode interruptMode = 0; // Default
            if(mask & UART_ENABLE_RX) {
                uartSfr->usta.set = UART_RX_EN_BIT;
                interruptMode |= UART_INT_MODE_RX_NOT_EMPTY;
            }
            if(mask & UART_ENABLE_TX) {
                uartSfr->usta.set = UART_TX_EN_BIT;
                interruptMode |= UART_INT_MODE_TX_HAS_SPACE;
            }
            
            uart_set_interrupt_mode(module, interruptMode);
            uart_clr_interrupt_flag(module->channel, UART_INTERRUPT_ALL);
            uart_enable_interrupt(module->channel, UART_INTERRUPT_FAULT | UART_INTERRUPT_RECEIVE_DONE);
            uartSfr->umode.set = UART_MODULE_EN_BIT;
        }
    }
}

void uart_disable(const struct UartModule* module) 
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            
            uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
            uartSfr->umode.clr = UART_MODULE_EN_BIT;
            uartSfr->usta.clr = UART_RX_EN_BIT | UART_TX_EN_BIT;
        }
    }
}

enum UartError uart_error(const struct UartModule* module)
{
    enum UartError error = UART_ERROR_OK;
    if(module != NULL && module->opt.assigned)    
        error = module->error;
    return error;
}

void uart_reset(struct UartModule* module)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned && module->error) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            enum UartEnable enableMask = uartSfr->usta.reg & UART_RX_EN_BIT ? UART_ENABLE_RX : 0 |
                                         uartSfr->usta.reg & UART_TX_EN_BIT ? UART_ENABLE_TX : 0;
            uart_disable(module);
            uart_enable(module, enableMask);
        }
    }
}

unsigned int uart_transmit(const struct UartModule* module, const union UartData* data, const unsigned int length)
{
    ASSERT(module != NULL);
    ASSERT(data != NULL);
    
    unsigned int rLength = 0;
    if(!module->opt.assigned || module->error)
        return rLength;
    
    uart_disable_interrupt(module->channel, UART_INTERRUPT_TRANSFER_DONE); // @Todo: review if this is really necessary
    while(!queue_is_full(module->txFifo) && rLength < length)
        queue_add(module->txFifo, &data[rLength++]);
    uart_enable_interrupt(module->channel, UART_INTERRUPT_TRANSFER_DONE);
    return rLength;
}

unsigned int uart_receive(const struct UartModule* module, union UartData* data, const unsigned int length)
{
    ASSERT(module != NULL);
    ASSERT(data != NULL);
    
    unsigned int rLength = 0;
    if(!module->opt.assigned || module->error)
        return rLength;
    
    uart_disable_interrupt(module->channel, UART_INTERRUPT_RECEIVE_DONE);  // @Todo: review if this is really necessary
    while(!queue_is_empty(module->rxFifo) && rLength < length)
        queue_take(module->rxFifo, &data[rLength++]);
    uart_enable_interrupt(module->channel, UART_INTERRUPT_RECEIVE_DONE);  // @Todo: review if this is really necessary
    return rLength;
}

unsigned int uart_transmit_raw(const struct UartModule* module, const void* buffer, const unsigned int size)
{
    ASSERT(module != NULL);
    ASSERT(buffer != NULL);
    
    unsigned int rSize = 0;
    if(!module->opt.assigned || module->error)
        return rSize;
    
    const unsigned char* rawBuffer = (const unsigned char*)buffer;
    union UartData tx = { 0 };
    uart_disable_interrupt(module->channel, UART_INTERRUPT_TRANSFER_DONE); // @Todo: review if this is really necessary
    while(!queue_is_full(module->txFifo) && rSize < size) {
        tx.data = rawBuffer[rSize++];
        queue_add(module->txFifo, &tx);
    }
    uart_enable_interrupt(module->channel, UART_INTERRUPT_TRANSFER_DONE);
    return rSize;
}

unsigned int uart_receive_raw(const struct UartModule* module, unsigned char* buffer, const unsigned int size)
{
    ASSERT(module != NULL);
    ASSERT(buffer != NULL);
    
    unsigned int rSize = 0;
    if(!module->opt.assigned || module->error)
        return rSize;
    
    union UartData rx = { 0 };
    uart_disable_interrupt(module->channel, UART_INTERRUPT_RECEIVE_DONE);  // @Todo: review if this is really necessary
    while(!queue_is_empty(module->rxFifo) && rSize < size) {
        queue_take(module->rxFifo, &rx);
        buffer[rSize++] = rx.data;
    }
    uart_enable_interrupt(module->channel, UART_INTERRUPT_RECEIVE_DONE);  // @Todo: review if this is really necessary
    return rSize;
}

unsigned char uart_rx_available(const struct UartModule* module)
{
    return (module != NULL && !queue_is_empty(module->rxFifo));
}

unsigned char uart_tx_available(const struct UartModule* module)
{
    return (module != NULL && !queue_is_full(module->txFifo));
}

void uart_set_interrupt_mode(const struct UartModule* module, const enum InterruptMode mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct UartMap* uartMap = &uartMappingTable[module->channel];
        struct UartSfr* uartSfr = uartMap->uartSfr;
        if(uartSfr != NULL) {
            // Reset all the interrupt modes of UxSTA                    
            uartSfr->usta.clr = UART_INT_MODE_RX_HALF | UART_INT_MODE_RX_THREE_QUARTER | 
                                UART_INT_MODE_TX_TRANSMITTED |UART_INT_MODE_TX_EMPTY;
            uartSfr->usta.set = mask;
        }
    }
}

void uart_enable_interrupt(const enum UartChannel channel, enum InterruptEnable mask)
{
    const enum InterruptRequest baseInterrupt = uartInterruptTable[channel]; 
    if(mask & UART_INTERRUPT_FAULT)
        interrupt_enable(baseInterrupt, UART_FAULT_INTERRUPT_PRIORITY);
    if(mask & UART_INTERRUPT_RECEIVE_DONE)
        interrupt_enable(baseInterrupt + 1, UART_RX_INTERRUPT_PRIORITY);
    if(mask & UART_INTERRUPT_TRANSFER_DONE)
        interrupt_enable(baseInterrupt + 2, UART_TX_INTERRUPT_PRIORITY);
}

void uart_disable_interrupt(const enum UartChannel channel, enum InterruptEnable mask)
{
    const enum InterruptRequest baseInterrupt = uartInterruptTable[channel]; 
    if(mask & UART_INTERRUPT_FAULT)
        interrupt_disable(baseInterrupt);
    if(mask & UART_INTERRUPT_RECEIVE_DONE)
        interrupt_disable(baseInterrupt + 1);
    if(mask & UART_INTERRUPT_TRANSFER_DONE)
        interrupt_disable(baseInterrupt + 2);
}

void uart_clr_interrupt_flag(const enum UartChannel channel, enum InterruptEnable mask)
{
    const enum InterruptRequest baseInterrupt = uartInterruptTable[channel]; 
    if(mask & UART_INTERRUPT_FAULT)
        interrupt_clr_flag(baseInterrupt);
    if(mask & UART_INTERRUPT_RECEIVE_DONE)
        interrupt_clr_flag(baseInterrupt + 1);
    if(mask & UART_INTERRUPT_TRANSFER_DONE)
        interrupt_clr_flag(baseInterrupt + 2);
}

#if defined(_UART1) && !defined(UART_CHANNEL1_FORCE_DISABLE)
void __ISR(_UART_1_VECTOR, IPL7AUTO)UART1interrupt(void)
{
    // Ignore NULL checks for performance
    struct UartModule* module = &uartModulePool[UART_CHANNEL1];
    const struct UartMap* uartMap = &uartMappingTable[UART_CHANNEL1];
    struct UartSfr* uartSfr = uartMap->uartSfr;
              
    if(interrupt_get_flag(INTERRUPT_UART1_FAULT)) {
        // Check all error flags
        if(U1STAbits.OERR)
            module->error |= UART_ERROR_OVERRUN;
        if(U1STAbits.FERR)
            module->error |= UART_ERROR_FRAMING;
        if(U1STAbits.PERR)
            module->error |= UART_ERROR_PARITY;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == UART_ERROR_OK)
            module->error = UART_ERROR_UNKNOWN;
        
        uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_UART1_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_UART1_RECEIVE_DONE)) {
            union UartData rx = { 0 };
            rx._reg = uartSfr->rxreg;
            queue_add(module->rxFifo, &rx);
            interrupt_clr_flag(INTERRUPT_UART1_RECEIVE_DONE);
        } else if(interrupt_get_flag(INTERRUPT_UART1_TRANSFER_DONE)) {
            union UartData tx = { 0 };
            queue_take(module->txFifo, &tx);
            uartSfr->txreg = tx._reg;
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_UART1_TRANSFER_DONE);
            interrupt_clr_flag(INTERRUPT_UART1_TRANSFER_DONE);
        }
    }
}
#endif

#if defined(_UART2) && !defined(UART_CHANNEL2_FORCE_DISABLE)
void __ISR(_UART_2_VECTOR, IPL7AUTO)UART2interrupt(void)
{
    // Ignore NULL checks for performance
    struct UartModule* module = &uartModulePool[UART_CHANNEL2];
    const struct UartMap* uartMap = &uartMappingTable[UART_CHANNEL2];
    struct UartSfr* uartSfr = uartMap->uartSfr;
    
    if(interrupt_get_flag(INTERRUPT_UART2_FAULT)) {
        // Check all error flags
        if(U1STAbits.OERR)
            module->error |= UART_ERROR_OVERRUN;
        if(U1STAbits.FERR)
            module->error |= UART_ERROR_FRAMING;
        if(U1STAbits.PERR)
            module->error |= UART_ERROR_PARITY;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == UART_ERROR_OK)
            module->error = UART_ERROR_UNKNOWN;
        
        uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_UART2_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_UART2_RECEIVE_DONE)) {
            union UartData rx = { 0 };
            rx._reg = uartSfr->rxreg;
            queue_add(module->rxFifo, &rx);
            interrupt_clr_flag(INTERRUPT_UART2_RECEIVE_DONE);
        } else if(interrupt_get_flag(INTERRUPT_UART2_TRANSFER_DONE)) {
            union UartData tx = { 0 };
            queue_take(module->txFifo, &tx);
            uartSfr->txreg = tx._reg;
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_UART2_TRANSFER_DONE);
            interrupt_clr_flag(INTERRUPT_UART2_TRANSFER_DONE);
        }
    }
}
#endif

#if defined(_UART3) && !defined(UART_CHANNEL3_FORCE_DISABLE)
void __ISR(_UART_3_VECTOR, IPL7AUTO)UART3interrupt(void)
{
    // Ignore NULL checks for performance
    struct UartModule* module = &uartModulePool[UART_CHANNEL3];
    const struct UartMap* uartMap = &uartMappingTable[UART_CHANNEL3];
    struct UartSfr* uartSfr = uartMap->uartSfr;
    
    if(interrupt_get_flag(INTERRUPT_UART3_FAULT)) {
        // Check all error flags
        if(U1STAbits.OERR)
            module->error |= UART_ERROR_OVERRUN;
        if(U1STAbits.FERR)
            module->error |= UART_ERROR_FRAMING;
        if(U1STAbits.PERR)
            module->error |= UART_ERROR_PARITY;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == UART_ERROR_OK)
            module->error = UART_ERROR_UNKNOWN;
        
        uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_UART3_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_UART3_RECEIVE_DONE)) {
            union UartData rx = { 0 };
            rx._reg = uartSfr->rxreg;
            queue_add(module->rxFifo, &rx);
            interrupt_clr_flag(INTERRUPT_UART3_RECEIVE_DONE);
        }else if(interrupt_get_flag(INTERRUPT_UART3_TRANSFER_DONE)) {
            union UartData tx = { 0 };
            queue_take(module->txFifo, &tx);
            uartSfr->txreg = tx._reg;
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_UART3_TRANSFER_DONE);
            interrupt_clr_flag(INTERRUPT_UART3_TRANSFER_DONE);
        }
    }
}
#endif

#if defined(_UART4) && !defined(UART_CHANNEL4_FORCE_DISABLE)
void __ISR(_UART_4_VECTOR, IPL7AUTO)UART4interrupt(void)
{
    // Ignore NULL checks for performance
    struct UartModule* module = &uartModulePool[UART_CHANNEL4];
    const struct UartMap* uartMap = &uartMappingTable[UART_CHANNEL4];
    struct UartSfr* uartSfr = uartMap->uartSfr;
    
    if(interrupt_get_flag(INTERRUPT_UART4_FAULT)) {
        // Check all error flags
        if(U1STAbits.OERR)
            module->error |= UART_ERROR_OVERRUN;
        if(U1STAbits.FERR)
            module->error |= UART_ERROR_FRAMING;
        if(U1STAbits.PERR)
            module->error |= UART_ERROR_PARITY;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == UART_ERROR_OK)
            module->error = UART_ERROR_UNKNOWN;
        
        uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_UART4_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_UART4_RECEIVE_DONE)) {
            union UartData rx = { 0 };
            rx._reg = uartSfr->rxreg;
            queue_add(module->rxFifo, &rx);
            interrupt_clr_flag(INTERRUPT_UART4_RECEIVE_DONE);
        } else if(interrupt_get_flag(INTERRUPT_UART4_TRANSFER_DONE)) {
            union UartData tx = { 0 };
            queue_take(module->txFifo, &tx);
            uartSfr->txreg = tx._reg;
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_UART4_TRANSFER_DONE);
            interrupt_clr_flag(INTERRUPT_UART4_TRANSFER_DONE);
        }
    }
}
#endif

#if defined(_UART5) && !defined(UART_CHANNEL5_FORCE_DISABLE)
void __ISR(_UART_5_VECTOR, IPL7AUTO)UART5interrupt(void)
{
    // Ignore NULL checks for performance
    struct UartModule* module = &uartModulePool[UART_CHANNEL5];
    const struct UartMap* uartMap = &uartMappingTable[UART_CHANNEL5];
    struct UartSfr* uartSfr = uartMap->uartSfr;
    
    if(interrupt_get_flag(INTERRUPT_UART5_FAULT)) {
        // Check all error flags
        if(U1STAbits.OERR)
            module->error |= UART_ERROR_OVERRUN;
        if(U1STAbits.FERR)
            module->error |= UART_ERROR_FRAMING;
        if(U1STAbits.PERR)
            module->error |= UART_ERROR_PARITY;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == UART_ERROR_OK)
            module->error = UART_ERROR_UNKNOWN;
        
        uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_UART5_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_UART5_RECEIVE_DONE)) {
            union UartData rx = { 0 };
            rx._reg = uartSfr->rxreg;
            queue_add(module->rxFifo, &rx);
            interrupt_clr_flag(INTERRUPT_UART5_RECEIVE_DONE);
        }else if(interrupt_get_flag(INTERRUPT_UART5_TRANSFER_DONE)) {
            union UartData tx = { 0 };
            queue_take(module->txFifo, &tx);
            uartSfr->txreg = tx._reg;
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_UART5_TRANSFER_DONE);
            interrupt_clr_flag(INTERRUPT_UART5_TRANSFER_DONE);
        }
    }
}
#endif

#if defined(_UART6) && !defined(UART_CHANNEL6_FORCE_DISABLE)
void __ISR(_UART_6_VECTOR, IPL7AUTO)UART6interrupt(void)
{
    // Ignore NULL checks for performance
    struct UartModule* module = &uartModulePool[UART_CHANNEL6];
    const struct UartMap* uartMap = &uartMappingTable[UART_CHANNEL16];
    struct UartSfr* uartSfr = uartMap->uartSfr;
    
    if(interrupt_get_flag(INTERRUPT_UART6_FAULT)) {
        // Check all error flags
        if(U1STAbits.OERR)
            module->error |= UART_ERROR_OVERRUN;
        if(U1STAbits.FERR)
            module->error |= UART_ERROR_FRAMING;
        if(U1STAbits.PERR)
            module->error |= UART_ERROR_PARITY;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == UART_ERROR_OK)
            module->error = UART_ERROR_UNKNOWN;
        
        uart_disable_interrupt(module->channel, UART_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_UART6_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_UART6_RECEIVE_DONE)) {
            union UartData rx = { 0 };
            rx._reg = uartSfr->rxreg;
            queue_add(module->rxFifo, &rx);
            interrupt_clr_flag(INTERRUPT_UART6_RECEIVE_DONE);
        }else if(interrupt_get_flag(INTERRUPT_UART6_TRANSFER_DONE)) {
            union UartData tx = { 0 };
            queue_take(module->txFifo, &tx);
            uartSfr->txreg = tx._reg;
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_UART6_TRANSFER_DONE);
            interrupt_clr_flag(INTERRUPT_UART6_TRANSFER_DONE);
        }
    }
}
#endif