#ifndef QUEUE_H
#define	QUEUE_H

#include "queue_types.h"
#include "../std/stdtypes.h"

#define QUEUE_POOL_SIZE         5

#define QUEUE_DEFAULT_TYPE_TABLE                    \
            QUEUE_NEW_TYPE(unsigned char, UCHAR)    \
            QUEUE_NEW_TYPE(char, CHAR)              \
            QUEUE_NEW_TYPE(unsigned int, UINT)      \
            QUEUE_NEW_TYPE(int, INT)                \
            QUEUE_NEW_TYPE(unsigned short, USHORT)  \
            QUEUE_NEW_TYPE(short, SHORT)            \
            QUEUE_NEW_TYPE(unsigned long, ULONG)    \
            QUEUE_NEW_TYPE(long, LONG)              \
            QUEUE_NEW_TYPE(double, DOUBLE)          \
            QUEUE_NEW_TYPE(float, FLOAT)

#if defined(QUEUE_GLOBAL_CUSTOM_TYPE_TABLE)
    #define QUEUE_TYPE_TABLE                        \
                QUEUE_GLOBAL_CUSTOM_TYPE_TABLE      \
                QUEUE_DEFAULT_TYPE_TABLE
#else
    #define QUEUE_TYPE_TABLE                        \
            QUEUE_DEFAULT_TYPE_TABLE
#endif

struct Queue;

 enum QueueType
{
    QUEUE_FIFO = 0, // @note FIFO is not thread and interrupt safe because an add and take action happen both on the same head variable
    QUEUE_LIFO, // @note LIFO is not thread and interrupt safe because an add and take action happen both on the same head variable
            
    QUEUE_TYPE_COUNT
};

enum QueueDataType
{
#define QUEUE_NEW_TYPE(type, name) QUEUE_##name,
    QUEUE_TYPE_TABLE
#undef QUEUE_NEW_TYPE
            
    QUEUE_DATA_TYPE_COUNT
};

/**
 * Initializes the queue pool
 * @return Returns 'true' on success, otherwise 'false'
 * @note This function will invalidate the queue pool, meaning no queue should be created before this function is called
 */
bool queue_init();

/**
 * Claims a queue from the pool and initializes it
 * @param buffer A buffer where the data will be stored in
 * @param length The length of the buffer
 * @param type The type of the queue
 * @param dataType The data type of the queue
 * @return Returns a pointer to the created queue or 'NULL' when an error occured
 * @warning Make sure the buffer pointer is of the same datatype as the chosen QueueDataType
 */
struct Queue* queue_create(void* buffer, const unsigned int length, const enum QueueType type, const enum QueueDataType dataType);

/**
 * Invalidates a queue and returns it to the pool
 * @param queue The queue to be invalidated
 */
void queue_invalidate(struct Queue* queue);

/**
 * Add data to a queue
 * @param queue The queue where the data will be inserted
 * @param data The data to be added
 * @return Returns '1' if successful otherwise '0'
 * @warning Make sure the data is of the same datatype as the Queue's QueueDataType
 */
unsigned char queue_add(struct Queue* queue, const void* data);

/**
 * Take data out of the queue
 * @param queue The queue where the data will be taken out
 * @param data Put the data in this field
 * @return Returns '1' if successful otherwise '0'
 * @note If the queue is empty data will remain untouched
 * @warning Make sure the data is of the same datatype as the Queue's QueueDataType
 */
unsigned char queue_take(struct Queue* queue, void* data);

/**
 * Flushes the queue
 * @param queue The queue which is to be flushed
 */
void queue_flush(struct Queue* queue);

/**
 * Checks if a queue is empty
 * @param queue The queue to be checked
 * @return Returns '1' if the queue is empty otherwise '0'
 */
unsigned char queue_is_empty(const struct Queue* queue);

/**
 * Checks if a queue is full
 * @param queue The queue to be checked
 * @return Returns '1' if the queue is full otherwise '0'
 */
unsigned char queue_is_full(const struct Queue* queue);

/**
 * Checks if a queue is valid
 * @param queue The queue to be checked
 * @return Returns '1' if the queue is valid otherwise '0'
 */
unsigned char queue_is_valid(const struct Queue* queue);

#endif	/* QUEUE_H */

