#include "clock.h"
#include <stddef.h>
#include <string.h>

clock_t ClockCreate(void){
    return NULL;
}

bool ClockGetTime(clock_t clock, clock_time_t *result){
    (void) clock;
    memset(result, 0, sizeof(clock_time_t));
    return false;
}
