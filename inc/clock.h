#include <stdint.h>
#include <stdbool.h>

typedef union {
struct {
    uint8_t seconds[2];
    uint8_t minutes[2];
    uint8_t hours[2];
} time;
uint8_t bcd[6];
} clock_time_t;

typedef struct clock_s * clock_t;

clock_t ClockCreate(void);
bool ClockGetTime(clock_t clock, clock_time_t *result);

bool ClockSetTime(clock_t clock, const clock_time_t *new_time);
