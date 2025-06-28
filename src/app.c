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

board_t AppInit() {
    board_t board = board_create();
    clock = ClockCreate(TICKS_FOR_SECOND, SNOOZE_TIME);
    
    // Configurar parpadeo inicial porque la hora no es válida al comienzo
    DisplayFlashDigit(board->screen, 0, 3, 50);
    DisplayFlashPoints(board->screen, 1, 1, 50);
    
    ScreenDisablePoint(board->screen, 0);
    ScreenDisablePoint(board->screen, 2);
    ScreenDisablePoint(board->screen, 3);

    return board; //n6
}

void AppRun(board_t board) {
    static uint16_t ticks = 0;
    static clock_time_t current_time;

    ticks++;
    if (ticks >= TICKS_FOR_SECOND) {
        ClockNewTick(clock);
        ticks = 0;
    }

    if (ClockGetTime(clock, &current_time)) {
        ScreenWriteBCD(board->screen, current_time.bcd, 4);
        ScreenEnablePoint(board->screen, 1);
    } else {
        uint8_t default_bcd[4] = {0, 0, 0, 0};
        ScreenWriteBCD(board->screen, default_bcd, 4);
    }

}



/* === Private function definitions ================================================================================
 */

/* === End of documentation ========================================================================================
 */
