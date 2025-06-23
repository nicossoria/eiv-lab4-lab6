/*********************************************************************************************************************
Copyright (c) 2025, Martin Nicolas Soria <soria.m.nicolas@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, disponiblestribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file test_reloj.c
 ** @brief Implementación de la biblioteca de gestión de reloj despertador con alarma.
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */

/**
 * @brief Estructura privada que representa el reloj.
 */
struct clock_s {
    uint16_t clock_ticks;           /**< Contador de ticks */
    uint16_t ticks_for_seconds;     /**< Ticks necesarios para un segundo */
    clock_time_t current_time;      /**< Hora actual */
    clock_time_t alarm_time;        /**< Hora de la alarma */
    clock_time_t snooze_time;       /**< Hora a la que sonará la alarma pospuesta */
    uint8_t snooze;                 /**< Tiempo de posposición de la alarma en minutos */
    bool valid;                     /**< Indica si la hora es válida */
    bool alarm_enabled;             /**< Indica si la alarma está habilitada */
    bool alarm_triggered;           /**< Indica si la alarma debe sonar */
    bool alarm_canceled;            /**< Indica si la alarma fue cancelada */
    bool snooze_enabled;            /**< Indica si la alarma está pospuesta */
};
/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ============================================================================== */

clock_t ClockCreate(uint16_t tick_for_second, uint8_t snooze) {
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));

    self->clock_ticks=0;
    self->ticks_for_seconds=tick_for_second;
    self->valid = false;
    self->snooze = snooze;
    self->alarm_enabled = false;
    self->alarm_triggered = false;
    self->alarm_canceled = false;
    self->snooze_enabled = false;
    return self;
}

bool ClockGetTime(clock_t self, clock_time_t * result) {
    if (result == NULL) {
        return false;
    }
    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}

bool ClockSetTime(clock_t self, const clock_time_t * new_time) {
    if (new_time == NULL) {
        return false;
    }
    uint8_t seconds = new_time->time.seconds[1] * 10 + new_time->time.seconds[0];
    uint8_t minutes = new_time->time.minutes[1] * 10 + new_time->time.minutes[0];
    uint8_t hours = new_time->time.hours[1] * 10 + new_time->time.hours[0];
    if (seconds > 59 || minutes > 59 || hours > 23) {
        self->valid = false;
    } else {
        self->valid = true;
        memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    }
    return self->valid;
}

void ClockNewTick(clock_t self) {
    self->clock_ticks++;
    if (self->clock_ticks == self->ticks_for_seconds) {
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

bool ClockSetAlarm(clock_t self, const clock_time_t * alarm) {
    if (alarm == NULL) {
        return false;
    }
    uint8_t seconds = alarm->time.seconds[1] * 10 + alarm->time.seconds[0];
    uint8_t minutes = alarm->time.minutes[1] * 10 + alarm->time.minutes[0];
    uint8_t hours = alarm->time.hours[1] * 10 + alarm->time.hours[0];
    if (seconds > 59 || minutes > 59 || hours > 23) {
        return false;
    } else {
        memcpy(&self->alarm_time, alarm, sizeof(clock_time_t));
        self->alarm_enabled = true;
        return true;
    }
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

bool ClockIsAlarmEnabled(clock_t self){
    return self->alarm_enabled;
}

/* === Private function definitions ================================================================================ */

/* === End of documentation ======================================================================================== */
