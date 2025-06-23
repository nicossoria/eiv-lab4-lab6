#include "clock.h"
#include <stddef.h>
#include <string.h>

struct clock_s {
    uint16_t clock_ticks;
    clock_time_t current_time;
    clock_time_t alarm_time;
    clock_time_t snooze_time;
    uint8_t snooze;
    bool valid;
    bool alarm_enabled;
    bool alarm_triggered;
    bool alarm_canceled;
    bool snooze_enabled;
};

clock_t ClockCreate(uint16_t tick_for_second, uint8_t snooze) {
    static struct clock_s self[1];

    (void)tick_for_second;

    memset(self, 0, sizeof(struct clock_s));
    self->valid = false;
    self->snooze = snooze;
    self->alarm_enabled = false;
    self->alarm_triggered = false;
    self->alarm_canceled = false;
    self->snooze_enabled = false;
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
                            self->alarm_canceled = false;
                        } else if (self->current_time.time.hours[0] > 9) {
                            self->current_time.time.hours[0] = 0;
                            self->current_time.time.hours[1]++;
                        }
                    }
                }
            }
        }
    }

    if (self->alarm_enabled && !self->alarm_canceled && !self->snooze_enabled &&
        memcmp(&self->current_time, &self->alarm_time, sizeof(clock_time_t)) == 0) {
        self->alarm_triggered = true;
    }

    if (self->snooze_enabled && memcmp(&self->current_time, &self->snooze_time, sizeof(clock_time_t)) == 0) {
        self->alarm_triggered = true;
        self->snooze_enabled = false;
    }
}

bool CLockSetAlarm(clock_t self, const clock_time_t * alarm) {
    memcpy(&self->alarm_time, alarm, sizeof(clock_time_t));
    self->alarm_enabled = true;
    return self->alarm_enabled;
}

bool ClockGetAlarm(clock_t self, clock_time_t * alarm_time) {
    memcpy(alarm_time, &self->alarm_time, sizeof(clock_time_t));
    return self->alarm_enabled;
}

bool ClockIsAlarmTriggered(clock_t self) {
    return self->alarm_triggered;
}

void ClockDisableAlarm(clock_t self) {
    self->alarm_enabled = false;
}

void ClockSnooze(clock_t self) {
    if (self->alarm_triggered && self->snooze > 0) {
        memcpy(&self->snooze_time, &self->current_time, sizeof(clock_time_t));
        self->snooze_time.time.minutes[0] += self->snooze;

        if (self->snooze_time.time.minutes[0] > 9) {
            self->snooze_time.time.minutes[0] -= 10;
            self->snooze_time.time.minutes[1]++;
            if (self->snooze_time.time.minutes[1] > 5) {
                self->snooze_time.time.minutes[1] = 0;
                self->snooze_time.time.hours[0]++;
                if (self->snooze_time.time.hours[0] > 9) {
                    self->snooze_time.time.hours[0] = 0;
                    self->snooze_time.time.hours[1]++;
                    if (self->snooze_time.time.hours[1] == 2 && self->snooze_time.time.hours[0] > 3) {
                        self->snooze_time.time.hours[0] = 0;
                        self->snooze_time.time.hours[1] = 0;
                    }
                }
            }
        }

        self->alarm_triggered = false;
        self->snooze_enabled = true;
    }
}

void ClockCancelAlarm(clock_t self) {
    if (self->alarm_triggered) {
        self->alarm_triggered = false;
        self->alarm_canceled = true;
    }
}
