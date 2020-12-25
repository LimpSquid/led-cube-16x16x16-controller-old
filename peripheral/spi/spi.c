#include "spi.h"
#include "mapping/spi_map.h"
#include "../interrupt/interrupt.h"
#include "../../lib/types/queue.h"
#include "../../lib/print/assert.h"

#define SPI_SS_EN_BIT           BIT_SHIFT(7)
#define SPI_SDI_DIS_BIT         BIT_SHIFT(4)
#define SPI_SDO_DIS_BIT         BIT_SHIFT(12)
#define SPI_MODULE_EN_BIT       BIT_SHIFT(15)

#define isMaster(sfr)           (sfr->spicon.reg & SPI_PROP_MODE_MASTER)

struct SpiModule
{
    struct Queue* rxFifo;
    struct Queue* txFifo;
    enum SpiChannel channel;
    unsigned char error;
    struct {
        unsigned char assigned :1;
    } opt;
};

enum InterruptEnable 
{
    SPI_INTERRUPT_FAULT             = BIT_SHIFT(0),
    SPI_INTERRUPT_RECEIVE_DONE      = BIT_SHIFT(1),
    SPI_INTERRUPT_TRANSFER_DONE     = BIT_SHIFT(2),
    
    SPI_INTERRUPT_ALL               = 0xFF
};

enum InterruptMode
{
    SPI_INT_MODE_RX_IS_EMPTY        = 0,
    SPI_INT_MODE_RX_NOT_EMPTY       = BIT_SHIFT(0),
    SPI_INT_MODE_RX_ONE_HALF        = BIT_SHIFT(1),
    SPI_INT_MODE_RX_FULL            = BIT_SHIFT(0) | BIT_SHIFT(1),
    SPI_INT_MODE_TX_COMPLETE        = 0,
    SPI_INT_MODE_TX_IS_EMPTY        = BIT_SHIFT(2),
    SPI_INT_MODE_TX_ONE_HALF        = BIT_SHIFT(3),
    SPI_INT_MODE_TX_NOT_FULL        = BIT_SHIFT(2) | BIT_SHIFT(3),
            
    SPI_INTERRUPT_REG_T_FORCE       = REG_T_MAX
};

enum SpiStatus
{
    SPI_STATUS_RX_FULL              = BIT_SHIFT(0),
    SPI_STATUS_TX_FULL              = BIT_SHIFT(1),
    SPI_STATUS_TX_EMPTY             = BIT_SHIFT(3),
    SPI_STATUS_RX_EMPTY             = BIT_SHIFT(5), // @Note: only valid when ENHBUF = 1
    SPI_STATUS_OVERRUN_ERROR        = BIT_SHIFT(6),
    SPI_STATUS_SR_EMPTY             = BIT_SHIFT(7), // @Note: only valid when ENHBUF = 1
    SPI_STATUS_UNDERRUN_ERROR       = BIT_SHIFT(9),
    SPI_STATUS_BUSY                 = BIT_SHIFT(11),
    SPI_STATUS_FRAMING_ERROR        = BIT_SHIFT(12),
            
    SPI_STATUS_REG_T_FORCE          = REG_T_MAX
};

static void spi_set_interrupt_mode(const struct SpiModule* module, const enum InterruptMode mask);
static void spi_enable_interrupt(const enum SpiChannel channel, enum InterruptEnable mask);
static void spi_disable_interrupt(const enum SpiChannel channel, enum InterruptEnable mask);
static void spi_clr_interrupt_flag(const enum SpiChannel channel, enum InterruptEnable mask);

static const enum InterruptRequest spiInterruptTable[] =
{
#if defined(_SPI1) && !defined(SPI_CHANNEL1_FORCE_DISABLE)
    INTERRUPT_SPI1_FAULT,
#endif
#if defined(_SPI2) && !defined(SPI_CHANNEL2_FORCE_DISABLE)
    INTERRUPT_SPI2_FAULT,
#endif
};

static struct SpiModule spiModulePool[SPI_CHANNEL_COUNT];
static const size_t nSpiModules = SPI_CHANNEL_COUNT;

bool spi_init()
{
    // Invalidate all spi modules
    size_t i;
    for(i = 0; i < nSpiModules; ++i) {
        spi_disable_interrupt(i, SPI_INTERRUPT_ALL);
        spiModulePool[i].opt.assigned = 0;
    }
    return true;
}

struct SpiModule* spi_create(const enum SpiChannel channel, unsigned int* rxBuffer, unsigned int* txBuffer, const unsigned int rxSize, const unsigned int txSize)
{
    struct SpiModule* module = NULL;
    if(channel >= SPI_CHANNEL_COUNT || rxBuffer == NULL || txBuffer == NULL || rxSize == 0 || txSize == 0)      
        return module;
    
    module = &spiModulePool[channel];
    if(!module->opt.assigned) { // Unused module was found
        module->rxFifo = queue_create(rxBuffer, rxSize, QUEUE_FIFO, QUEUE_UINT);
        module->txFifo = queue_create(txBuffer, txSize, QUEUE_FIFO, QUEUE_UINT);
        module->channel = channel;
        module->error = SPI_ERROR_OK;
        module->opt.assigned = 1;
    } else
        module = NULL;
    return module;
}

void spi_invalidate(struct SpiModule* module)
{
    if(module == NULL)
        return;

    if(module->opt.assigned) {
        // @Todo: Disable SPI module
        queue_invalidate(module->rxFifo);
        queue_invalidate(module->txFifo);
        module->opt.assigned = 0;
    }
}

void spi_configure(const struct SpiModule* module, const enum SpiConfiguration mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            // Reset both SPIxCON and SPIxCON2 registers
            spiSfr->spicon.clr = REG_T_MAX & 0x0000ffff;
            spiSfr->spicon2.clr = REG_T_MAX & 0x0000ffff;

            spiSfr->spicon.set = mask & UINT_MAX;
            spiSfr->spicon2.set = (mask >> 32) & UINT_MAX;
        }
    }
}

void spi_set_properties(const struct SpiModule* module, const enum SpiProperties mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            // Reset all the properties of UxMODE
            spiSfr->spicon.clr = SPI_PROP_MODE_MASTER | SPI_PROP_MODE_32 | SPI_PROP_MODE_16;
            spiSfr->spicon.set = mask;
        }
    }
}

unsigned int spi_set_baudrate(const struct SpiModule* module, const unsigned long clock, const unsigned int baudrate)
{
    unsigned int result = 0;
    if(module == NULL)
        return result;
    
    if(module->opt.assigned) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            reg_t brg = 0;
            spiSfr->spibrg.clr = REG_T_MAX & 0x000000ff;
            brg = (clock >> 1) / baudrate;
            if(brg > 0xff)
                brg = 0xff;
            result = (clock >> 1) / brg;
            brg = brg ? (brg - 1) : 0;
            spiSfr->spibrg.set = brg;
        }
    }
    return result;
}

void spi_enable(struct SpiModule* module, const enum SpiEnable mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            // Disable interrupts
            spi_disable_interrupt(module->channel, SPI_INTERRUPT_ALL);
            
            // Disable SPI
            spiSfr->spicon.clr = SPI_MODULE_EN_BIT;
            spiSfr->spicon.set = SPI_SDI_DIS_BIT | SPI_SDO_DIS_BIT;
            spiSfr->spicon.clr = SPI_SS_EN_BIT;
            
            // Error recovery
            if(module->error) {
                spiSfr->spistat.clr = SPI_STATUS_FRAMING_ERROR | SPI_STATUS_UNDERRUN_ERROR | SPI_STATUS_OVERRUN_ERROR;
                module->error = SPI_ERROR_OK;
            }
            
            // Flush queues
            queue_flush(module->rxFifo);
            queue_flush(module->txFifo);
            
            // Enable SPI and interrupts
            enum InterruptMode interruptMode = 0; // Default
            if(mask & SPI_ENABLE_SDI) {
                spiSfr->spicon.clr = SPI_SDI_DIS_BIT;
                interruptMode |=  SPI_INT_MODE_RX_NOT_EMPTY;
            }
            if(mask & SPI_ENABLE_SDO) {
                spiSfr->spicon.clr = SPI_SDO_DIS_BIT;
                interruptMode |= SPI_INT_MODE_TX_NOT_FULL;
            }
            if(mask & SPI_ENABLE_SS) 
                spiSfr->spicon.set = SPI_SS_EN_BIT;
            
            spi_set_interrupt_mode(module, interruptMode);
            spi_clr_interrupt_flag(module->channel, SPI_INTERRUPT_ALL);
            spi_enable_interrupt(module->channel, SPI_INTERRUPT_FAULT | SPI_INTERRUPT_RECEIVE_DONE);
            spiSfr->spicon.set = SPI_MODULE_EN_BIT;
        }
    }
}

void spi_disable(const struct SpiModule* module) 
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            spi_disable_interrupt(module->channel, SPI_INTERRUPT_ALL);
            spiSfr->spicon.clr = SPI_MODULE_EN_BIT;
            spiSfr->spicon.clr = SPI_MODULE_EN_BIT;
            spiSfr->spicon.set = SPI_SDI_DIS_BIT | SPI_SDO_DIS_BIT;
            spiSfr->spicon.clr = SPI_SS_EN_BIT;
        }
    }
}

unsigned char spi_error(const struct SpiModule* module)
{
    unsigned char error = SPI_ERROR_OK;
    if(module != NULL && module->opt.assigned)    
        error = module->error;
    return error;
}

void spi_reset(struct SpiModule* module)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned && module->error) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            enum SpiEnable enableMask =  spiSfr->spicon.reg & SPI_SS_EN_BIT ? SPI_ENABLE_SS : 0 |
                                         spiSfr->spicon.reg & SPI_SDI_DIS_BIT ? 0 : SPI_ENABLE_SDI |
                                         spiSfr->spicon.reg & SPI_SDO_DIS_BIT ? 0 : SPI_ENABLE_SDO;
            spi_disable(module);
            spi_enable(module, enableMask);
        }
    }
}

unsigned int spi_transmit(const struct SpiModule* module, const unsigned int* buffer, const unsigned int size)
{
    ASSERT(module != NULL);
    ASSERT(buffer != NULL);
    
    unsigned int rSize = 0;
    if(!module->opt.assigned || module->error)
        return rSize;
    
    spi_disable_interrupt(module->channel, SPI_INTERRUPT_TRANSFER_DONE); // @Todo: review if this is really necessary
    while(!queue_is_full(module->txFifo) && rSize < size) {
        queue_add(module->txFifo, &buffer[rSize]);
        rSize++;
    }
    spi_enable_interrupt(module->channel, SPI_INTERRUPT_TRANSFER_DONE);
    return rSize;
}

unsigned int spi_receive(const struct SpiModule* module, unsigned int* buffer, const unsigned int size)
{
    ASSERT(module != NULL);
    ASSERT(buffer != NULL);
    
    unsigned int rSize = 0;
    if(!module->opt.assigned || module->error)
        return rSize;
    
    spi_disable_interrupt(module->channel, SPI_INTERRUPT_RECEIVE_DONE);  // @Todo: review if this is really necessary
    while(!queue_is_empty(module->rxFifo) && rSize < size)
        queue_take(module->rxFifo, &buffer[rSize++]);
    spi_enable_interrupt(module->channel, SPI_INTERRUPT_RECEIVE_DONE);  // @Todo: review if this is really necessary
    return rSize;
}

void spi_set_interrupt_mode(const struct SpiModule* module, const enum InterruptMode mask)
{
    if(module == NULL)
        return;
    
    if(module->opt.assigned) {
        const struct SpiMap* spiMap = &spiMappingTable[module->channel];
        struct SpiSfr* spiSfr = spiMap->spiSfr;
        if(spiSfr != NULL) {
            // Reset all the interrupt modes of SPIxCON                    
            spiSfr->spicon.clr = SPI_INT_MODE_RX_FULL | SPI_INT_MODE_TX_NOT_FULL;
            spiSfr->spicon.set = mask;
        }
    }
}

void spi_enable_interrupt(const enum SpiChannel channel, enum InterruptEnable mask)
{
    const enum InterruptRequest baseInterrupt = spiInterruptTable[channel]; 
    if(mask & SPI_INTERRUPT_FAULT)
        interrupt_enable(baseInterrupt, SPI_FAULT_INTERRUPT_PRIORITY);
    if(mask & SPI_INTERRUPT_RECEIVE_DONE)
        interrupt_enable(baseInterrupt + 1, SPI_RX_INTERRUPT_PRIORITY);
    if(mask & SPI_INTERRUPT_TRANSFER_DONE)
        interrupt_enable(baseInterrupt + 2, SPI_TX_INTERRUPT_PRIORITY);
}

void spi_disable_interrupt(const enum SpiChannel channel, enum InterruptEnable mask)
{
    const enum InterruptRequest baseInterrupt = spiInterruptTable[channel]; 
    if(mask & SPI_INTERRUPT_FAULT)
        interrupt_disable(baseInterrupt);
    if(mask & SPI_INTERRUPT_RECEIVE_DONE)
        interrupt_disable(baseInterrupt + 1);
    if(mask & SPI_INTERRUPT_TRANSFER_DONE)  
        interrupt_disable(baseInterrupt + 2);
}

void spi_clr_interrupt_flag(const enum SpiChannel channel, enum InterruptEnable mask)
{
    const enum InterruptRequest baseInterrupt = spiInterruptTable[channel]; 
    if(mask & SPI_INTERRUPT_FAULT)
        interrupt_clr_flag(baseInterrupt);
    if(mask & SPI_INTERRUPT_RECEIVE_DONE)
        interrupt_clr_flag(baseInterrupt + 1);
    if(mask & SPI_INTERRUPT_TRANSFER_DONE)
        interrupt_clr_flag(baseInterrupt + 2);
}

#if defined(_SPI1) && !defined(SPI_CHANNEL1_FORCE_DISABLE)
void __ISR(_SPI_1_VECTOR, IPL7AUTO)SPI1interrupt(void)
{
    // Ignore NULL checks for performance
    struct SpiModule* module = &spiModulePool[SPI_CHANNEL1];
    const struct SpiMap* spiMap = &spiMappingTable[SPI_CHANNEL1];
    struct SpiSfr* spiSfr = spiMap->spiSfr;
    
    if(interrupt_get_flag(INTERRUPT_SPI1_FAULT)) {
        // Check all error flags
        if(SPI1STATbits.FRMERR)
            module->error |= SPI_ERROR_FRAME;
        if(SPI1STATbits.SPITUR)
            module->error |= SPI_ERROR_UNDERRUN;
        if(SPI1STATbits.SPIROV)
            module->error |= SPI_ERROR_OVERRUN;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == SPI_ERROR_OK)
            module->error = SPI_ERROR_UNKNOWN;
        
        spi_disable_interrupt(module->channel, SPI_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_SPI1_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_SPI1_RECEIVE_DONE)) {
            queue_add(module->rxFifo, (unsigned int*)&spiSfr->spibuf);
            interrupt_clr_flag(INTERRUPT_SPI1_RECEIVE_DONE);
        }
        if(interrupt_get_flag(INTERRUPT_SPI1_TRANSMIT_DONE)) {
             queue_take(module->txFifo, (unsigned int*)&spiSfr->spibuf);
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_SPI1_TRANSMIT_DONE);
            interrupt_clr_flag(INTERRUPT_SPI1_TRANSMIT_DONE);
        }
    }
}
#endif

#if defined(_SPI2) && !defined(SPI_CHANNEL2_FORCE_DISABLE)
void __ISR(_SPI_2_VECTOR, IPL7AUTO)SPI2interrupt(void)
{
    // Ignore NULL checks for performance
    struct SpiModule* module = &spiModulePool[SPI_CHANNEL2];
    const struct SpiMap* spiMap = &spiMappingTable[SPI_CHANNEL2];
    struct SpiSfr* spiSfr = spiMap->spiSfr;
    
    if(interrupt_get_flag(INTERRUPT_SPI2_FAULT)) {
        // Check all error flags
        if(SPI2STATbits.FRMERR)
            module->error |= SPI_ERROR_FRAME;
        if(SPI2STATbits.SPITUR)
            module->error |= SPI_ERROR_UNDERRUN;
        if(SPI2STATbits.SPIROV)
            module->error |= SPI_ERROR_OVERRUN;
        
        // If, in any case, no error was detected we still set the error byte to UNKNOWN.
        if(module->error == SPI_ERROR_OK)
            module->error = SPI_ERROR_UNKNOWN;
        
        spi_disable_interrupt(module->channel, SPI_INTERRUPT_ALL);
        interrupt_clr_flag(INTERRUPT_SPI2_FAULT);
    } else {
        if(interrupt_get_flag(INTERRUPT_SPI2_RECEIVE_DONE)) {
            queue_add(module->rxFifo, (unsigned int*)&spiSfr->spibuf);
            interrupt_clr_flag(INTERRUPT_SPI2_RECEIVE_DONE);
        }
        if(interrupt_get_flag(INTERRUPT_SPI2_TRANSMIT_DONE)) {
             queue_take(module->txFifo, (unsigned int*)&spiSfr->spibuf);
            if(queue_is_empty(module->txFifo))
                interrupt_disable(INTERRUPT_SPI2_TRANSMIT_DONE);
            interrupt_clr_flag(INTERRUPT_SPI2_TRANSMIT_DONE);
        }
    }
}
#endif