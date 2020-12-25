#include "queue.h"
#include "../print/assert.h"
#include <stddef.h>

#define QUEUE_POOL_SIZE_DEFAULT     25   
#define QUEUE_POOL_MAX              100

struct Queue
{
    void* buffer;
    unsigned int head;
    unsigned int tail;
    unsigned int length;
    struct {
        unsigned char assigned :1;
    } opt;
    enum QueueType type;
    enum QueueDataType dataType;
};

static inline void __attribute__((always_inline)) buffer_add(void* buffer, const enum QueueDataType type, const unsigned int index, const void* data);
static inline void __attribute__((always_inline)) buffer_take(const void* buffer, const enum QueueDataType type, const unsigned int index, void* data);

static unsigned char take_back(struct Queue* queue, void* data);
static unsigned char take_front(struct Queue* queue, void* data);

#ifdef QUEUE_POOL_SIZE
    #if (QUEUE_POOL_SIZE < 1)
        #error "Queue pool size must be a non negative integer with a minimum of 1"
    #elif (QUEUE_POOL_SIZE > QUEUE_POOL_MAX)
        #error "Maximum number of queues is exceeded, increase maximum or lower the pool size."
    #else
        static struct Queue queuePool[QUEUE_POOL_SIZE];
        static const size_t nQueues = QUEUE_POOL_SIZE;
    #endif
#else
    static struct Queue queuePool[QUEUE_POOL_SIZE_DEFAULT];
    static const size_t nQueues = QUEUE_POOL_SIZE_DEFAULT;
#endif

bool queue_init()
{
    // Invalidate all queues
    size_t i;
    for(i = 0; i < nQueues; ++i)
        queuePool[i].opt.assigned = 0;
    return true;
}
    
struct Queue* queue_create(void* buffer, const unsigned int length, const enum QueueType type, const enum QueueDataType dataType)
{
    struct Queue* queue = NULL;
    if(buffer == NULL || length == 0 || type >= QUEUE_TYPE_COUNT || dataType >= QUEUE_DATA_TYPE_COUNT)      
        return queue;
    
    size_t i;
    for(i = 0; i < nQueues; ++i) {
        if(!queuePool[i].opt.assigned) {
            queue = &queuePool[i];
            break;
        }
    }
    
    if(queue != NULL) { // Queue was found
        queue->buffer = buffer;
        queue->head = 0;
        queue->tail = 0;
        queue->length = length;
        queue->type = type;
        queue->dataType = dataType;
        queue->opt.assigned = 1;
    }
    return queue;
}

void queue_invalidate(struct Queue* queue)
{
    ASSERT(queue != NULL);
    
    queue->opt.assigned = 0;
}

unsigned char queue_add(struct Queue* queue, const void* data)
{
    ASSERT(queue != NULL);
    
    unsigned char result = 0;
    if(data == NULL)
        return result;
    
    if(queue->opt.assigned) {
        unsigned int next = (queue->head + 1) % queue->length;
        if(next != queue->tail) {
            buffer_add(queue->buffer, queue->dataType, queue->head, data);
            queue->head = next;
            result = 1;
        }
    }
    return result;
}

unsigned char queue_take(struct Queue* queue, void* data)
{
    ASSERT(queue != NULL);
    
    unsigned char result = 0;
    if(data == NULL)
        return result;
    
    if(queue->opt.assigned) {
        switch(queue->type) {
            case QUEUE_FIFO:    result = take_back(queue, data);    break;
            case QUEUE_LIFO:    result = take_front(queue, data);   break;
            default:                                                break;
        }
    }
    return result;
}

void queue_flush(struct Queue* queue)
{
    ASSERT(queue != NULL);
    
    queue->head = 0;
    queue->tail = 0;
}

unsigned char queue_is_empty(const struct Queue* queue)
{
    ASSERT(queue != NULL);
    
    if(!queue->opt.assigned)
        return 1;
    
    return (queue->head == queue->tail);
}

unsigned char queue_is_full(const struct Queue* queue)
{
    ASSERT(queue != NULL);
    
    if(!queue->opt.assigned)
        return 0;
    
    unsigned int next = (queue->head + 1) % queue->length;
    return (next == queue->tail);
}

unsigned char queue_is_valid(const struct Queue* queue)
{
    ASSERT(queue != NULL);
    
    return queue->opt.assigned;
}

inline void __attribute__((always_inline)) buffer_add(void* buffer, const enum QueueDataType type, const unsigned int index, const void* data)
{
    switch(type) {
#define QUEUE_NEW_TYPE(type, name)                          \
        case QUEUE_##name:                                  \
            ((type*)buffer)[index] = *((const type*)data);  \
            break;
    QUEUE_TYPE_TABLE
#undef QUEUE_NEW_TYPE
        default:
            break;
    };
}

inline void __attribute__((always_inline)) buffer_take(const void* buffer, const enum QueueDataType type, const unsigned int index, void* data)
{
    switch(type) {
#define QUEUE_NEW_TYPE(type, name)                          \
        case QUEUE_##name:                                  \
            *((type*)data) = ((const type*)buffer)[index];  \
            break;
    QUEUE_TYPE_TABLE
#undef QUEUE_NEW_TYPE
        default:
            break;
    };
}

unsigned char take_back(struct Queue* queue, void* data)
{
    ASSERT(queue != NULL);
    
    // Queue is empty
    if(queue->head == queue->tail)
        return 0;
    
    buffer_take(queue->buffer, queue->dataType, queue->tail, data);
    queue->tail = (queue->tail + 1) % queue->length;
    return 1;
}

unsigned char take_front(struct Queue* queue, void* data)
{
    ASSERT(queue != NULL);
    
    // Queue is empty
    if(queue->head == queue->tail)
        return 0;
    
    if(queue->head > 0) 
        queue->head = queue->head - 1;
    else
        queue->head = queue->length - 1;
    buffer_take(queue->buffer, queue->dataType, queue->head, data);
    return 1;
}
