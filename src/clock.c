#include "clock.h"
#include <stddef.h>
#include <string.h>

struct clock_s {
    uint16_t clock_ticks;
    clock_time_t current_time;
    clock_time_t alarm_time;
    bool valid;
    bool alarm_enabled;
};

clock_t ClockCreate(uint16_t tick_for_second) {
    (void)tick_for_second;
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));
    self->valid = false;
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t * result) {
    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}

bool ClockSetTime(clock_t self, const clock_time_t * new_time) {
    self->valid = true;
    memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    return self->valid;
}

void ClockNewTick(clock_t self) {
    self->clock_ticks++;
    if (self->clock_ticks == 5) {
        self->clock_ticks = 0;
        self->current_time.time.seconds[0]++;
        if (self->current_time.time.seconds[0] > 9) {
            self->current_time.time.seconds[0] = 0;
            self->current_time.time.seconds[1]++;

            if (self->current_time.time.seconds[1] > 5) {
                self->current_time.time.seconds[1] = 0;
                self->current_time.time.minutes[0]++;
                if (self->current_time.time.minutes[0] > 9) {
                    self->current_time.time.minutes[0] = 0;
                    self->current_time.time.minutes[1]++;
                    if (self->current_time.time.minutes[1] > 5) {
                        self->current_time.time.minutes[1] = 0;

                        self->current_time.time.hours[0]++;
                        if (self->current_time.time.hours[1] == 2 && self->current_time.time.hours[0] > 3) {
                            self->current_time.time.hours[0] = 0;
                            self->current_time.time.hours[1] = 0;
                        } else if (self->current_time.time.hours[0] > 9) {
                            self->current_time.time.hours[0] = 0;
                            self->current_time.time.hours[1]++;
                        }
                    }
                }
            }
        }
    }
}

bool CLockSetAlarm(clock_t self, const clock_time_t * alarm) {
    memcpy(&self->alarm_time, alarm, sizeof(clock_time_t));
    self->alarm_enabled = true;
    return self->alarm_enabled;
}
