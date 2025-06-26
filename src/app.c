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

/* === Macros definitions ========================================================================================== */

#define TICKS_FOR_SECOND 5
#define SNOOZE_TIME      5

/* === Private data type declarations ============================================================================== */

typedef enum {
    STATE_CLOCK_RUNNING,
    STATE_SET_TIME_MINUTES,
    STATE_SET_TIME_HOURS,
    STATE_SET_ALARM_MINUTES,
    STATE_SET_ALARM_HOURS,
    STATE_SNOOZE,
    STATE_CLOCK
} app_state_t;

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

static clock_t clock;
static app_state_t current_state = STATE_CLOCK_RUNNING;

/* === Public variable definitions ================================================================================= */

/* === Public function definitions ============================================================================== */

board_t AppInit(void) {
    clock = ClockCreate(TICKS_FOR_SECOND, SNOOZE_TIME);
    return board_create();
}

void AppRun(board_t board) {
    static uint16_t ticks = 0;
    static uint16_t f1_hold_tic = 0;
    static uint16_t idle_ticks = 0;
    static clock_time_t current_time = {0};
    for (int i = 0; i < 4; i++) {
        current_time.bcd[i] = 0;
    }

    ScreenRefresh(board->screen);

    ticks++;
    if (ticks >= TICKS_FOR_SECOND) //
    {
        ClockNewTick(clock);
        ticks = 0;
    }

    switch (current_state) {
    case STATE_CLOCK_RUNNING:

        if (ClockGetTime(clock, &current_time)) {

            ScreenWriteBCD(board->screen, current_time.bcd, 4);
            DisplayFlashPoints(board->screen, 1, 1, 20);

        } else {
            ScreenWriteBCD(board->screen, current_time.bcd, 4);
            DisplayFlashDigit(board->screen, 0, 3, 20);
        }

        if (DigitalInputGetIsActive(board->set_time)) {
            f1_hold_tic++;
            if (f1_hold_tic >= TICKS_FOR_SECOND * 3) {
                current_state = STATE_SET_TIME_MINUTES;
                f1_hold_tic = 0;
                idle_ticks = 0;
            }
        } else {
            f1_hold_tic = 0;
        }

        break;

    case STATE_SET_TIME_MINUTES:
        ScreenWriteBCD(board->screen, current_time.bcd, 4);
        DisplayFlashDigit(board->screen, 0, 1, 20);
        idle_ticks++;

        if (DigitalInputGetIsActive(board->increment)) {
            current_time.time.minutes[0]++;
            if (current_time.time.minutes[0] > 9) {
                current_time.time.minutes[0] = 0;
                current_time.time.minutes[1]++;
                if (current_time.time.minutes[1] > 5) {
                    current_time.time.minutes[1] = 0;
                }
            }
            idle_ticks = 0;
        }

        if (DigitalInputGetIsActive(board->decrement)) {
            if (current_time.time.minutes[0] == 0) {
                current_time.time.minutes[0] = 9;
                if (current_time.time.minutes[1] == 0) {
                    current_time.time.minutes[1] = 5;
                } else {
                    current_time.time.minutes[1]--;
                }
            } else {
                current_time.time.minutes[0]--;
            }
            idle_ticks = 0;
        }
        if (DigitalInputGetIsActive(board->accept)) {
            current_state = STATE_SET_TIME_HOURS;
            idle_ticks = 0;
        }

        if (DigitalInputGetIsActive(board->cancel) || idle_ticks >= TICKS_FOR_SECOND * 30) {
            current_state = STATE_CLOCK_RUNNING;
        }

        break;

    case STATE_SET_TIME_HOURS:
        ScreenWriteBCD(board->screen, current_time.bcd, 4);
        DisplayFlashDigit(board->screen, 2, 3, 20);
        idle_ticks++;

        if (DigitalInputGetIsActive(board->increment)) {
            current_time.time.hours[0]++;
            if ((current_time.time.hours[1] == 2 && current_time.time.hours[0] > 3) ||
                (current_time.time.hours[0] > 9)) {
                current_time.time.hours[0] = 0;
                current_time.time.hours[1]++;
                if (current_time.time.hours[1] > 2)
                    current_time.time.hours[1] = 0;
            }
            idle_ticks = 0;
        }

        if (DigitalInputGetIsActive(board->decrement)) {
            if (current_time.time.hours[0] == 0) {
                current_time.time.hours[0] = 9;
                if (current_time.time.hours[1] == 0) {
                    current_time.time.hours[1] = 2;
                } else {
                    current_time.time.hours[1]--;
                }
                if (current_time.time.hours[1] == 2 && current_time.time.hours[0] > 3) {
                    current_time.time.hours[0] = 3;
                }
            } else {
                current_time.time.hours[0]--;
            }
            idle_ticks = 0;
        }

        if (DigitalInputGetIsActive(board->accept)) {
            ClockSetTime(clock, &current_time);
            current_state = STATE_CLOCK_RUNNING;
        }

        if (DigitalInputGetIsActive(board->cancel) || idle_ticks >= TICKS_FOR_SECOND * 30) {
            current_state = STATE_CLOCK_RUNNING;
        }
        break;
    case STATE_CLOCK:
    case STATE_SET_ALARM_MINUTES:
    case STATE_SET_ALARM_HOURS:
    case STATE_SNOOZE:
        break;
    }
}
/* === Private function definitions ================================================================================
 */

/* === End of documentation ========================================================================================
 */
