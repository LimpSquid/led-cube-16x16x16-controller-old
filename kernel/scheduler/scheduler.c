#include "scheduler.h"
#include "cfg/scheduler_config.h"
#include "cfg/scheduler_timer_def.h"
#include <xc.h>
#include <stddef.h>

#define EVENT_DEF_POOL_SIZE         5
#define EVENT_MAX_POOL_SIZE         50
#define ROBIN_TASK_DEF_POOL_SIZE    5
#define ROBIN_TASK_MAX_POOL_SIZE    50

#ifdef SCHEDULER_CUSTOM_TIMER
    #define HW_TIMER            TMR_REG
    #define HW_TIMER_CFG_REG    TMR_CFG_REG
    #define HW_TIMER_CFG_WORD   TMR_CFG_WORD
    #define HW_TIMER_PRESCALER  TMR_PRESCALER
    #define HW_TIMER_CFG_EN_BIT TMR_CFG_EN_BIT
#elif !defined(SCHEDULER_NO_HW_TIMER)
    #define HW_TIMER            _TMR_REG
    #define HW_TIMER_CFG_REG    _TMR_CFG_REG
    #define HW_TIMER_CFG_WORD   _TMR_CFG_WORD
    #define HW_TIMER_PRESCALER  _TMR_PRESCALER
    #define HW_TIMER_CFG_EN_BIT _TMR_CFG_EN_BIT
#else
    #error "No valid hardware timer configuration found."
#endif

#if defined(_SYS_CLK) && defined(_PB_DIV)
    #define SYSTEM_TICK ((1000000.0F / (_SYS_CLK / _PB_DIV)) * HW_TIMER_PRESCALER)
#elif defined(PBCLK_FREQUENCY)
    #define SYSTEM_TICK ((1000000.0F / PBCLK_FREQUENCY) * HW_TIMER_PRESCALER)
#else
    #error "System tick could not be calculated, please define the PBCLK_FREQUENCY in scheduler_config.h or _SYS_CLK and _PB_DIV globally."
#endif

static unsigned long calc_sys_ticks(unsigned short time, enum SchedulerIntervalUnit unit);
static void sort_events();
static struct Event* find_event(unsigned int identifier);
static void sort_robin_tasks();
static struct RobinTask* find_robin_task(unsigned int identifier);

struct Event
{
    SchedulerHandle handle;
    unsigned long interval;
    unsigned long ticks;
    unsigned int identifier;
    
    struct {
        unsigned char priority :2;
        unsigned char assigned :1;
        unsigned char reserved :5;
    } opt;
};

struct RobinTask
{
    SchedulerHandle handle;
    unsigned int identifier;
    struct {
        unsigned char assigned :1;
        unsigned char reserved :7;
    } opt;
};

#ifdef EVENT_POOL_SIZE
    #if (EVENT_POOL_SIZE < 1)
        #error "Event pool size must be a non negative integer with a minimum of 1"
    #elif (EVENT_POOL_SIZE > EVENT_MAX_POOL_SIZE)
        #error "Maximum number of events is exceeded, increase maximum or lower the pool size."
    #else
        static struct Event eventPool[EVENT_POOL_SIZE];
        static const size_t nEvents = EVENT_POOL_SIZE;
    #endif
#else
    static struct Event eventPool[EVENT_DEF_POOL_SIZE];
    static const unsigned int nEvents = EVENT_DEF_POOL_SIZE;
#endif
    
#ifdef ROBIN_TASK_POOL_SIZE
    #if (ROBIN_TASK_POOL_SIZE < 1)
        #error "Robin task pool size must be a non negative integer with a minimum of 1"
    #elif (ROBIN_TASK_POOL_SIZE > ROBIN_TASK_MAX_POOL_SIZE)
        #error "Maximum number of robin tasks is exceeded, increase maximum or lower the pool size."
    #else
        static struct RobinTask robinTaskPool[ROBIN_TASK_POOL_SIZE];
        static const size_t nRobinTasks = ROBIN_TASK_POOL_SIZE;
    #endif
#else
    static struct RobinTask robinTaskPool[ROBIN_TASK_DEF_POOL_SIZE];
    static const unsigned int nRobinTasks = ROBIN_TASK_DEF_POOL_SIZE;
#endif
    
static unsigned short lastTickCount = 0;
static unsigned short elapsedTicks = 0;
static unsigned short robinTaskOffset = 0;
static unsigned short nValidEvents = 0;
static unsigned short nValidRobinTasks = 0;

bool scheduler_init()
{
    size_t i;
    
    // Invalidate all items from the pools and set the identifier
    for(i = 0; i < nEvents; ++i) {
        eventPool[i].opt.assigned = 0;
        eventPool[i].identifier = i;
    }
    for(i = 0; i < nRobinTasks; ++i) {
        robinTaskPool[i].opt.assigned = 0;
        robinTaskPool[i].identifier = i;
    }
    
    // Initialize variables
    lastTickCount = 0;
    elapsedTicks = 0;
    robinTaskOffset = 0;
    nValidEvents = 0;
    nValidRobinTasks = 0;
    
    // Configure timer
    HW_TIMER_CFG_REG &= ~(1 << HW_TIMER_CFG_EN_BIT);
    HW_TIMER_CFG_REG = HW_TIMER_CFG_WORD;
    HW_TIMER_CFG_REG |= 1 << HW_TIMER_CFG_EN_BIT;
    
    return true;
}

void scheduler_execute()
{
    SchedulerHandle handle = NULL;
    
     // Calculate elapsed ticks since last call
    elapsedTicks = HW_TIMER - lastTickCount;
    lastTickCount += elapsedTicks;
    
    // Service events
    struct Event* event = eventPool; // Pool will always contain atleast one item
    while(event->opt.assigned) { // Eventpool will always be contiguous with valid events
        if(event->ticks <= elapsedTicks) {
            if(handle == NULL) {
                event->ticks = event->interval;
                handle = event->handle;
            } else
                event->ticks = 0;
        } else
            event->ticks -= elapsedTicks;
        
        // Increment event pointer
        event++;
    }
    
    // Service robin tasks
    if(handle == NULL) {
        struct RobinTask* task = &robinTaskPool[robinTaskOffset]; // Pool will always contain atleast one item
        if(task->opt.assigned) {
            handle = task->handle;
            robinTaskOffset++;
        } else 
            robinTaskOffset = 0;
    }
    
    // Yay, finally execute handle!
    if(handle != NULL)
        (*handle)();
}

struct Event* scheduler_create_event(const SchedulerHandle handle, const unsigned short interval, const enum SchedulerIntervalUnit unit, const unsigned char priority)
{
    struct Event* event = NULL;
    
    if(handle == NULL || nValidEvents >= nEvents)
        return event;
    
    size_t i;
    for(i = 0; i < nEvents; ++i) {
        if(!eventPool[i].opt.assigned) {
            event = &eventPool[i];
            break;
        }
    }
    
    if(event != NULL) {
        event->handle = handle;
        event->interval = calc_sys_ticks(interval, unit);
        event->ticks = 0;
        event->opt.priority = priority;
        event->opt.assigned = 1;
        nValidEvents++;
        unsigned int identifier = event->identifier;
        sort_events(); // Might cause the event pointer to change
        event = find_event(identifier);
    }
    return event;
}

void scheduler_remove_event(struct Event* event)
{
    if(event == NULL)
        return;
    
    if(event->opt.assigned)
        event->opt.assigned = 0; // Invalidate event
}

struct RobinTask* scheduler_create_robin_task(const SchedulerHandle handle)
{
    struct RobinTask* task = NULL;
    
    if(handle == NULL || nValidRobinTasks >= nRobinTasks)
        return task;
    
    size_t i;
    for(i = 0; i < nRobinTasks; ++i) {
        if(!robinTaskPool[i].opt.assigned) {
            task = &robinTaskPool[i];
            break;
        }
    }
    
    if(task != NULL) {
        task->handle = handle;
        task->opt.assigned = 1;
        nValidRobinTasks++;
        unsigned int identifier = task->identifier;
        sort_robin_tasks(); // Might cause the event pointer to change
        task = find_robin_task(identifier);
    }
    return task;
}

void scheduler_remove_robin_task(struct RobinTask* task)
{
    if(task == NULL)
        return;
    
    if(task->opt.assigned)
        task->opt.assigned = 0; // Invalidate event
}

unsigned long calc_sys_ticks(unsigned short time, enum SchedulerIntervalUnit unit)
{
    unsigned long ticks;
    switch(unit) {
        default: // Default to microseconds
        case SCHEDULER_UNIT_US: ticks = time / SYSTEM_TICK;                 break;
        case SCHEDULER_UNIT_MS: ticks = (time * 1000LU) / SYSTEM_TICK;      break;
        case SCHEDULER_UNIT_S:  ticks = (time * 1000000LU) / SYSTEM_TICK;   break; // @Todo: overflow might be possible!
    }
    return ticks;
}

void sort_events()
{
    unsigned short outer;
    unsigned short inner;
    struct Event* current;
    struct Event* next;
    struct Event temp;
    
    // Sort so that all the valid events are the first entries in the pool
    for(outer = 0; outer < nEvents; ++outer) {
        for(inner = 0; inner < (nEvents - outer - 1); ++inner) {
            current = &eventPool[inner];
            next = &eventPool[inner + 1];
            if(current->opt.assigned < next->opt.assigned) {
                temp = *next;
                *next = *current;
                *current = temp;
            }
        }
    }
    
    // Lastly sort so that the priorities are from high to low
    for(outer = 0; outer < nValidEvents; ++outer) {        
        for(inner = 0; inner < (nValidEvents - outer - 1); ++inner) {
            current = &eventPool[inner];
            next = &eventPool[inner + 1];
            if(current->opt.priority > next->opt.priority) {
                temp = *next;
                *next = *current;
                *current = temp;
            }
        }
    }
}

struct Event* find_event(unsigned int identifier)
{
    struct Event* event = NULL;
    size_t i;
    for(i = 0; i < nEvents; ++i) {
        if(eventPool[i].identifier == identifier) {
            event = &eventPool[i];
            break;
        }
    }
    return event;
}

void sort_robin_tasks()
{
    unsigned short outer;
    unsigned short inner;
    struct RobinTask* current;
    struct RobinTask* next;
    struct RobinTask temp;
    
    for(outer = 0; outer < nRobinTasks; ++outer) {
        for(inner = 0; inner < (nRobinTasks - outer - 1); ++inner) {
            current = &robinTaskPool[inner];
            next = &robinTaskPool[inner + 1];
            if(current->opt.assigned < next->opt.assigned) {
                temp = *next;
                *next = *current;
                *current = temp;
            }
        }
    }
}

struct RobinTask* find_robin_task(unsigned int identifier)
{
    struct RobinTask* robinTask = NULL;
    size_t i;
    for(i = 0; i < nRobinTasks; ++i) {
        if(robinTaskPool[i].identifier == identifier) {
            robinTask = &robinTaskPool[i];
            break;
        }
    }
    return robinTask;
}