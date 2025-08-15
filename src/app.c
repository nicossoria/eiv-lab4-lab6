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

/** @file app.c
 ** @brief Codigo fuente del reloj
 **/

/* === Headers files inclusions ==================================================================================== */
#include "app.h"
#include "bsp.h"
#include "clock.h"
#include "screen.h"
#include "digital.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
/* === Macros definitions ========================================================================================== */


/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

typedef enum {
    UI_MODE_NORMAL = 0,
    UI_MODE_SET_TIME_MIN,
    UI_MODE_SET_TIME_HOUR,
    UI_MODE_SET_ALARM_MIN,
    UI_MODE_SET_ALARM_HOUR
} ui_mode_t;

static board_t g_board;
static screen_t g_screen;
static clock_t g_clock;
static ui_mode_t g_mode = UI_MODE_NORMAL;
static clock_time_t g_edit;
static clock_time_t g_alarm_cfg;
static bool g_blink_sec = false;
static TimerHandle_t g_timeout;

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ================================================================================= */

static void to_digits(const clock_time_t *t, uint8_t d[4]) {
    uint8_t hh = (uint8_t)(t->time.hours[1] * 10u + t->time.hours[0]);
    uint8_t mm = (uint8_t)(t->time.minutes[1] * 10u + t->time.minutes[0]);
    d[0] = (uint8_t)((hh / 10u) % 10u);
    d[1] = (uint8_t)(hh % 10u);
    d[2] = (uint8_t)((mm / 10u) % 10u);
    d[3] = (uint8_t)(mm % 10u);
}

static void ui_start_timeout(void) {
    xTimerStop(g_timeout, 0);
    xTimerStart(g_timeout, 0);
}

static void ui_timeout_cb(TimerHandle_t xTimer) {
    (void)xTimer;
    g_mode = UI_MODE_NORMAL;
}

static void ui_render(void) {
    uint8_t digits[4];
    bool valid_now = true;

    if (g_mode == UI_MODE_SET_TIME_MIN || g_mode == UI_MODE_SET_TIME_HOUR ||
        g_mode == UI_MODE_SET_ALARM_MIN || g_mode == UI_MODE_SET_ALARM_HOUR) {
        to_digits(&g_edit, digits);
    } else {
        clock_time_t now;
        valid_now = ClockGetTime(g_clock, &now);
        to_digits(&now, digits);
    }

    ScreenWriteBCD(g_screen, digits, 4);

    for (int i = 0; i < 4; i++) {
        ScreenDisablePoint(g_screen, i);
    }

    if (g_mode == UI_MODE_NORMAL && !valid_now) {
        DisplayFlashDigit(g_screen, 0, 3, 20);
        DisplayFlashPoints(g_screen, 0, 3, 20);
    }

    if (g_mode == UI_MODE_NORMAL) {
        if (g_blink_sec) ScreenEnablePoint(g_screen, 1);
    }

    if (ClockIsAlarmEnabled(g_clock)) {
        ScreenEnablePoint(g_screen, 0);
    }
    if (ClockIsAlarmTriggered(g_clock)) {
        ScreenEnablePoint(g_screen, 3);
    }

    if (g_mode == UI_MODE_SET_TIME_MIN || g_mode == UI_MODE_SET_ALARM_MIN) {
        DisplayFlashDigit(g_screen, 2, 3, 20);
    } else if (g_mode == UI_MODE_SET_TIME_HOUR || g_mode == UI_MODE_SET_ALARM_HOUR) {
        DisplayFlashDigit(g_screen, 0, 1, 20);
    }

    if (g_mode == UI_MODE_SET_ALARM_MIN || g_mode == UI_MODE_SET_ALARM_HOUR) {
        for (int i = 0; i < 4; i++) {
            ScreenEnablePoint(g_screen, i);
        }
    }
}

board_t AppInit(void) {
    g_board = board_create();
    g_screen = g_board->screen;
    g_clock = ClockCreate(1000, 5);
    memset(&g_edit, 0, sizeof(g_edit));
    memset(&g_alarm_cfg, 0, sizeof(g_alarm_cfg));
    g_timeout = xTimerCreate("inact", pdMS_TO_TICKS(30000), pdFALSE, NULL, ui_timeout_cb);
    return g_board;
}

void TaskClock(void *param) {
    (void)param;
    uint32_t ms = 0;
    for (;;) {
        ClockNewTick(g_clock);
        vTaskDelay(pdMS_TO_TICKS(1));
        ms++;
        if (ms >= 1000) {
            ms = 0;
            g_blink_sec = !g_blink_sec;
        }
        if (ClockIsAlarmTriggered(g_clock)) {
            AlarmLedOn(g_board->alarm_led);
        } else {
            AlarmLedOff(g_board->alarm_led);
        }
    }
}

void TaskButtons(void *param) {
    (void)param;
    uint32_t f1 = 0, f2 = 0;
    for (;;) {
        if (DigitalInputGetIsActive(g_board->set_time)) {
            f1 += 20;
            if (f1 >= 3000 && g_mode == UI_MODE_NORMAL) {
                ClockGetTime(g_clock, &g_edit);
                g_mode = UI_MODE_SET_TIME_MIN;
                ui_start_timeout();
            }
        } else {
            f1 = 0;
        }

        if (DigitalInputGetIsActive(g_board->set_alarm)) {
            f2 += 20;
            if (f2 >= 3000 && g_mode == UI_MODE_NORMAL) {
                ClockGetAlarm(g_clock, &g_edit);
                g_mode = UI_MODE_SET_ALARM_MIN;
                ui_start_timeout();
            }
        } else {
            f2 = 0;
        }

        if (DigitalWasActive(g_board->increment)) {
            if (g_mode == UI_MODE_SET_TIME_MIN || g_mode == UI_MODE_SET_ALARM_MIN) {
                int min = g_edit.time.minutes[0] + g_edit.time.minutes[1] * 10;
                min = (min + 1) % 60;
                g_edit.time.minutes[0] = (uint8_t)(min % 10);
                g_edit.time.minutes[1] = (uint8_t)(min / 10);
                ui_start_timeout();
            } else if (g_mode == UI_MODE_SET_TIME_HOUR || g_mode == UI_MODE_SET_ALARM_HOUR) {
                int hour = g_edit.time.hours[0] + g_edit.time.hours[1] * 10;
                hour = (hour + 1) % 24;
                g_edit.time.hours[0] = (uint8_t)(hour % 10);
                g_edit.time.hours[1] = (uint8_t)(hour / 10);
                ui_start_timeout();
            }
        }

        if (DigitalWasActive(g_board->decrement)) {
            if (g_mode == UI_MODE_SET_TIME_MIN || g_mode == UI_MODE_SET_ALARM_MIN) {
                int min = g_edit.time.minutes[0] + g_edit.time.minutes[1] * 10;
                min = (min + 59) % 60;
                g_edit.time.minutes[0] = (uint8_t)(min % 10);
                g_edit.time.minutes[1] = (uint8_t)(min / 10);
                ui_start_timeout();
            } else if (g_mode == UI_MODE_SET_TIME_HOUR || g_mode == UI_MODE_SET_ALARM_HOUR) {
                int hour = g_edit.time.hours[0] + g_edit.time.hours[1] * 10;
                hour = (hour + 23) % 24;
                g_edit.time.hours[0] = (uint8_t)(hour % 10);
                g_edit.time.hours[1] = (uint8_t)(hour / 10);
                ui_start_timeout();
            }
        }

        if (DigitalWasActive(g_board->accept)) {
            if (g_mode == UI_MODE_SET_TIME_MIN) {
                g_mode = UI_MODE_SET_TIME_HOUR;
                ui_start_timeout();
            } else if (g_mode == UI_MODE_SET_TIME_HOUR) {
                ClockSetTime(g_clock, &g_edit);
                g_mode = UI_MODE_NORMAL;
            } else if (g_mode == UI_MODE_SET_ALARM_MIN) {
                g_mode = UI_MODE_SET_ALARM_HOUR;
                ui_start_timeout();
            } else if (g_mode == UI_MODE_SET_ALARM_HOUR) {
                g_alarm_cfg = g_edit;
                g_mode = UI_MODE_NORMAL;
            } else {
                if (ClockIsAlarmTriggered(g_clock)) {
                    ClockSnooze(g_clock);
                } else {
                    ClockSetAlarm(g_clock, &g_alarm_cfg);
                }
            }
        }

        if (DigitalWasActive(g_board->cancel)) {
            if (g_mode == UI_MODE_SET_TIME_MIN || g_mode == UI_MODE_SET_TIME_HOUR ||
                g_mode == UI_MODE_SET_ALARM_MIN || g_mode == UI_MODE_SET_ALARM_HOUR) {
                g_mode = UI_MODE_NORMAL;
            } else {
                if (ClockIsAlarmTriggered(g_clock)) {
                    ClockCancelAlarm(g_clock);
                } else {
                    ClockDisableAlarm(g_clock);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void TaskUI(void *param) {
    (void)param;
    for (;;) {
        ui_render();
        ScreenRefresh(g_screen);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}


/* === Private function definitions ================================================================================
 */

/* === End of documentation ========================================================================================
 */
