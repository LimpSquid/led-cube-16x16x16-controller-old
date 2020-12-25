#include "uart_stream.h"
#include "../uart.h"
#include "../cfg/uart_config.h"

#ifdef UART_STREAM_ENABLE

static void uart_stream_open();
static void uart_stream_close();
static int uart_stream_puts(void* module, const char* str, unsigned int size);

struct PrintStream uart_stream = 
{
    .data = NULL,
    .open = uart_stream_open,
    .close = uart_stream_close,
    .puts = uart_stream_puts
};

union UartData rxBuffer[1]; // @Todo: we should be able to create a module without a RX or TX buffer, we then should prohibit to enable the RX or TX
union UartData txBuffer[255];

void uart_stream_open()
{
    struct UartModule* module = uart_create(UART_STREAM_CHANNEL, rxBuffer, txBuffer, sizeof(rxBuffer) / sizeof(rxBuffer[0]), sizeof(txBuffer) / sizeof(txBuffer[0]));
    uart_configure(module, UART_CONFIG_TX_RX_EN);
    uart_set_properties(module, UART_PROP_DATA_BITS_8 | UART_PROP_STOP_BITS_1);
    uart_set_baudrate(module, (_SYS_CLK / _PB_DIV), UART_STREAM_BAUDRATE);
    uart_enable(module, UART_ENABLE_TX);
    
    uart_stream.data = module;
}

void uart_stream_close()
{
    uart_disable(uart_stream.data);
    uart_invalidate(uart_stream.data);
    uart_stream.data = NULL;
}

int uart_stream_puts(void* module, const char* str, unsigned int size)
{
    unsigned int index = 0;
    while(index != size)
        index += uart_transmit_raw(module, (str + index), (size - index));
    return index;
}

#endif