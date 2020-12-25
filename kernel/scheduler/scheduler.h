#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../../lib/std/stdtypes.h"

#define PRIO_HIGH   0
#define PRIO_NORMAL 1
#define PRIO_LOW    2

struct Event;
struct RobinTask;
typedef void (*SchedulerHandle)();

enum SchedulerIntervalUnit {
    SCHEDULER_UNIT_US = 0,
    SCHEDULER_UNIT_MS,
    SCHEDULER_UNIT_S
};

/**
 * Initialization of the scheduler
 * @return Returns 'true' on success, otherwise 'false'
 * @note Both the event and robin task pool will be invalidated, meaning no event should be created before the init function
 */
bool scheduler_init();

/**
 * Execution of the scheduler
 */
void scheduler_execute();

/**
 * Adds an event to the scheduler with a given interval time and priority
 * @param handle A handle that will be executed at the given interval
 * @param interval Interval in microseconds this event should be executed
 * @param priority The priority of the event
 * @return Returns a pointer to the created event
 */
struct Event* scheduler_create_event(const SchedulerHandle handle, const unsigned short interval, const enum SchedulerIntervalUnit unit, const unsigned char priority);

/**
 * Removes an event from the scheduler, freeing up one place in the pool
 * @param event The event to be removed
 */
void scheduler_remove_event(struct Event* event);

/**
 * Adds an robin task to the scheduler
 * @param handle A handle that will be executed at the given interval
 * @return Returns a pointer to the created robin task
 */
struct RobinTask* scheduler_create_robin_task(const SchedulerHandle handle);

/**
 * Removes a robin task from the scheduler, freeing up one place in the pool
 * @param task The robin task to be removed
 */
void scheduler_remove_robin_task(struct RobinTask* task);

#endif /* SCHEDULER_H */