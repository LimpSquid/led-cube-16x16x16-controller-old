#include "timer.h"
#include "../../scheduler/scheduler.h"
#include "../../../lib/print/assert.h"
#include <stddef.h>

#define TIMER_POOL_SIZE_DEFAULT     25   
#define TIMER_POOL_MAX              100

struct Timer
{
    unsigned long interval;
    unsigned long ticks;
    TimerHandle handle;
    struct {
        unsigned char type        :3;
        unsigned char assigned    :1;
        unsigned char suspended   :1;
        unsigned char timedout    :1;
        unsigned char reserved    :2;
    } opt;
};

static unsigned long timer_calc_systicks(unsigned int time, const enum TimerUnit unit);

#ifdef TIMER_POOL_SIZE
    #if (TIMER_POOL_SIZE < 1)
        #error "Timer pool size must be a non negative integer with a minimum of 1"
    #elif (TIMER_POOL_SIZE > TIMER_POOL_MAX)
        #error "Maximum number of timers is exceeded, increase maximum or lower the pool size."
    #else
        static struct Timer timerPool[TIMER_POOL_SIZE];
        static const size_t nTimers = TIMER_POOL_SIZE;
    #endif
#else
    static struct Timer timerPool[TIMER_POOL_SIZE_DEFAULT];
    static const size_t nTimers = TIMER_POOL_SIZE_DEFAULT;
#endif

bool timer_init()
{
    // Invalidate all timers
    size_t i;
    for(i = 0; i < nTimers; ++i)
        timerPool[i].opt.assigned = 0;
    
    return (bool)(scheduler_create_event(timer_execute, TIMER_TICK_INTERVAL, SCHEDULER_UNIT_US, PRIO_NORMAL));
}

void timer_execute()
{
    TimerHandle handle = NULL;
    
    size_t i;
    struct Timer* timer = timerPool;
    for(i = 0; i < nTimers; ++i) {
        if(timer->opt.assigned && !timer->opt.suspended) {
            
            // Decrement tick count
            if(timer->ticks > 0)
                timer->opt.timedout = !!!(--timer->ticks); // Determine if timer timed out after decrement.
            else 
                timer->opt.timedout = 1;
            
            if(timer->opt.timedout) {
                switch(timer->opt.type) {
                    case TIMER_SOFT:
                        if(handle == NULL) { // Yay, we can execute this timer's handle
                            timer->ticks = timer->interval; // Reset tick count
                            timer->opt.timedout = 0;
                            handle = timer->handle; 
                        }
                        break;
                    case TIMER_SINGLE_SHOT:
                        if(handle == NULL) {
                            timer->opt.suspended = 1;
                            handle = timer->handle;
                        } 
                        break;
                    case TIMER_COUNTDOWN:
                        timer->opt.suspended = 1;
                        break;
                    default:
                        break;
                }
            }
        }
        timer++; // Advance to next timer
    }
    
    if(handle != NULL)
        (*handle)(timer);
}

struct Timer* timer_create(const enum TimerType type, const TimerHandle handle)
{
    struct Timer* timer = NULL;
    if(type >= TIMER_COUNT)
        return timer;
    
    size_t i;
    for(i = 0; i < nTimers; ++i) {
        if(!timerPool[i].opt.assigned) {
            timer = &timerPool[i];
            break;
        }
    }
    
    if(timer != NULL) { // Timer was found
        timer->interval = 0;
        timer->ticks = 0;
        timer->handle = handle;
        timer->opt.type = type;
        timer->opt.suspended = 1;
        timer->opt.timedout = 0;
        timer->opt.assigned = 1;
    }
     
    return timer;
}

void timer_invalidate(struct Timer* timer)
{
    ASSERT(timer != NULL);
    
    timer->opt.assigned = 0;
}

void timer_set_time(struct Timer* timer, const unsigned int time, const enum TimerUnit unit)
{
    ASSERT(timer != NULL);
    
    timer->interval = timer_calc_systicks(time, unit);
    timer->ticks = timer->interval;
}

void timer_start(struct Timer* timer, const unsigned int time, const enum TimerUnit unit)
{
    ASSERT(timer != NULL);
    
    timer->interval = timer_calc_systicks(time, unit);
    timer->ticks = timer->interval;
    timer->opt.timedout = 0;
    timer->opt.suspended = 0;
}

void timer_stop(struct Timer* timer)
{
    ASSERT(timer != NULL);
    
    timer->opt.suspended = 1; // Suspend timer
}

void timer_restart(struct Timer* timer)
{
    ASSERT(timer != NULL);
    
    timer->ticks = timer->interval;
    timer->opt.timedout = 0;
    timer->opt.suspended = 0;
}

unsigned char timer_timed_out(const struct Timer* timer)
{
    ASSERT(timer != NULL);
    
    unsigned char result = 0;

    if(timer->opt.assigned) 
        result = timer->opt.timedout;
    return result;
}

unsigned char timer_is_valid(const struct Timer* timer)
{
    ASSERT(timer != NULL);
    
    return timer->opt.assigned;
}

static unsigned long timer_calc_systicks(unsigned int time, const enum TimerUnit unit)
{
    unsigned long ticks;
    switch(unit) {
        default: // Default to microseconds
        case TIMER_UNIT_US:
            ticks = time / TIMER_TICK_INTERVAL; 
            break;
        case TIMER_UNIT_MS:
            ticks = (time * 1000LU) / TIMER_TICK_INTERVAL;
            break;
        case TIMER_UNIT_S:
            //@Todo: Currently limited to 12 bits, fixme?
            if(time > 4096)
                time = 4096;
            ticks = (time * 1000000LU) / TIMER_TICK_INTERVAL;
            break;
    }
    return ticks;
}