#ifndef QUEUE_TYPES_H
#define	QUEUE_TYPES_H

#include "../../peripheral/uart/uart.h"

/**
 * @brief Defines the different custom Queue data types
 * @details To add a new data type use the QUEUE_NEW_TYPE macro. The first parameter should be the desired data type to be used.
 *          The second parameter defines the name to access the data type when creating a Queue.
 */
#define QUEUE_GLOBAL_CUSTOM_TYPE_TABLE  \
            QUEUE_NEW_TYPE(union UartData, UART_DATA)

#endif /* QUEUE_TYPES_H */