#ifndef TIMER_H
#define	TIMER_H

#include "../../../lib/std/stdtypes.h"

#define TIMER_TICK_INTERVAL     500 // In microseconds
#define TIMER_POOL_SIZE         10

struct Timer;

typedef void(*TimerHandle)(struct Timer* timer);

enum TimerType
{
    TIMER_SOFT = 0,     // A timer that keeps firing at a specific interval, handle is executed on each timeout
    TIMER_SINGLE_SHOT,  // A timer that fires only once after it expired, handle is executed on timeout
    TIMER_COUNTDOWN,    // A timer that simply counts down and sets the timeout flag once it has expired
    // @Todo: TIMER_MEASURE
    
    TIMER_COUNT
};

enum TimerUnit
{
    TIMER_UNIT_US = 0,  // Microseconds
    TIMER_UNIT_MS,      // Milliseconds
    TIMER_UNIT_S        // Seconds
};

/**
 * Initializes the timer pool
 * @return Returns 'true' on success, otherwise 'false'
 * @note This function will invalidate the timer pool, meaning no timer should be created before this function is called
 */
bool timer_init();

/**
 * A timer event that updates all the timers
 * @note This function must be called at a fixed time interval of 'TIMER_TICK_INTERVAL' microseconds.
 */
void timer_execute();

/**
 * Claims a timer from the pool and initializes it
 * @param type The type of the timer
 * @param handle The handle that has to be executed on a timer time-out
 * @note The handle should be 'NULL' when a countdown timer is used
 * @return Returns a pointer to the created timer or 'NULL' when an error occured
 */
struct Timer* timer_create(const enum TimerType type, const TimerHandle handle);

/**
 * Invalidates a timer and returns it to the pool
 * @param timer The timer to be invalidated
 */
void timer_invalidate(struct Timer* timer);

/**
 * Set a time-out interval of a timer
 * @param timer The timer where the time-out will be set
 * @param time The time-out interval
 * @param unit The time unit
 * @note When 'TIMER_UNIT_S' is used the time-out interval will be limited to a 12 bit maximum
 */
void timer_set_time(struct Timer* timer, const unsigned int time, const enum TimerUnit unit);

/**
 * Sets a time-out interval and starts the timer
 * @param time The time-out interval
 * @param unit The time unit
 * @note When 'TIMER_UNIT_S' is used the time-out interval will be limited to a 12 bit maximum
 */
void timer_start(struct Timer* timer, const unsigned int time, const enum TimerUnit unit);

/**
 * Stops a timer
 * @param timer The timer to be stopped
 */
void timer_stop(struct Timer* timer);

/**
 * Restarts a timer with the last time-out interval
 * @param timer The timer to be restarted
 */
void timer_restart(struct Timer* timer);

/**
 * Checks if a timer is timed out
 * @param timer The timer to be checked
 * @return Returns '1' if the timer is timed out otherwise '0'
 */
unsigned char timer_timed_out(const struct Timer* timer);

/**
 * Checks if a timer is valid
 * @param timer The timer to be checked
 * @return Returns '1' if the timer is valid otherwise '0'
 */
unsigned char timer_is_valid(const struct Timer* timer);
    
#endif	/* TIMER_H */

