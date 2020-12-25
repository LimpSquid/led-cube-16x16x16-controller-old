#include "kernel/scheduler/scheduler.h"
#include "kernel/utils/timer/timer.h"
#include "lib/types/queue.h"
#include "lib/print/print.h"
#include "peripheral/system/cfg/config.h"
#include "peripheral/system/watchdog/watchdog.h"
#include "peripheral/uart/uart.h"
#include "peripheral/uart/stream/uart_stream.h"
#include "peripheral/spi/spi.h"
#include "lib/std/stdtypes.h"
#include <xc.h>

struct Module 
{
    bool (*initialize)(void);
};

static bool initialize_modules();
static void scheduler_populate();
static void halt_processor();

static struct Module modules[] = 
{
    { config_cpu_init },
    { scheduler_init },
    { timer_init },
    { queue_init },
    { uart_init },
    { spi_init },
    { NULL } // Terminator
};

void main(void) 
{    
    if(!initialize_modules()) {
        print_f("Failed to initialize modules!");
        halt_processor();
    }
    
    // Redirect all print output to the uart module
    print_init_stream(&uart_stream);
    
    // Add custom events and tasks to the scheduler
    scheduler_populate();
   
    // Lastly enable interrupts
    interrupt_enable_mvec();
    interrupt_global_enable();
    
    while(true) {
        clear_wdt(); // Clear watchdog each pass
        scheduler_execute();
    }
    return;
}

bool initialize_modules()
{
    bool result = true;
    struct Module* module = modules;
    while(module->initialize != NULL) {
        if(!module->initialize()) {
            result = false;
            break;
        }
    }
    return result;
}

void scheduler_populate()
{
    
}
void halt_processor()
{
    while(true) {
        Nop();
        Nop();
        Nop();
    }
}